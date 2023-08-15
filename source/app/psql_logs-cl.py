#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
import os
import re
import sys
import math
import time
import base64
import argparse
import curses
import datetime
import decimal
import textwrap
import psycopg2

# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def _positive_int(self, x):
        try:
            assert(int(x) > 0)
            return int(x)
        except:
            self.error("`%s' is not a positive integer value" % str(x))
    # -------------------------------------------------------------------------
    def _valid_msg_id(self, x):
        try:
            found = self._msg_re1.match(x)
            if found:
                return (None, None, found.group(1))
            found = self._msg_re2.match(x)
            if found:
                return (None, found.group(1), None)
            found = self._msg_re3.match(x)
            if found:
                return (None, found.group(1), found.group(2))
            found = self._msg_re4.match(x)
            if found:
                return (found.group(1), found.group(2), found.group(3))
            assert False
        except:
            self.error("`%s' is not a valid message identifier" % str(x))

    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        self._msg_re1 = re.compile("^([A-Za-z0-9_]{1,10})$")
        self._msg_re2 = re.compile("^([A-Za-z0-9_]{1,10})\.$")
        self._msg_re3 = re.compile("^([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10})$")
        self._msg_re4 = re.compile("^([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10})$")

        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None
        )

        self.add_argument(
            "--db-host", "-H",
            metavar='HOSTNAME',
            dest='db_host',
            action="store",
            default="localhost"
        )

        self.add_argument(
            "--db-port", "-P",
            metavar='PORT-NUMBER',
            dest='db_port',
            type=self._positive_int,
            action="store",
            default=5432
        )

        self.add_argument(
            "--db-name", "-D",
            metavar='DATABASE',
            dest='db_name',
            action="store",
            default="eagilog"
        )

        self.add_argument(
            "--db-user", "-u",
            metavar='USERNAME',
            dest='db_user',
            action="store",
            default="eagilog"
        )

        self.add_argument(
            "--db-password", "-w",
            metavar='PASSWORD',
            dest='db_password',
            action="store",
            default=None
        )

        self.add_argument(
            "--db-pass-file",
            metavar='FILE-PATH',
            dest='db_password_file',
            action="store",
            default=None
        )

        self.add_argument(
            "--block", "-B",
            metavar='IDENTIFIER',
            dest='block_list',
            nargs='+',
            action="append",
            type=self._valid_msg_id,
            default=[]
        )

        self.add_argument(
            "--hide-progress",
            dest='show_progress',
            action="store_false",
            default=True
        )

        self.add_argument(
            "--sort-progress",
            dest='sort_progress',
            action="store",
            choices=["source", "value", "remaining", "update"],
            default="source"
        )

        commands = self.add_mutually_exclusive_group()

        commands.add_argument(
            "--dump",
            dest='dump',
            action="store_true",
            default=False
        )

        commands.add_argument(
            "--recent",
            dest='recent',
            action="store_true",
            default=False
        )

        commands.add_argument(
            "--tail",
            dest='tail',
            action="store_true",
            default=False
        )

        commands.add_argument(
            "--watch",
            metavar='SECONDS',
            dest='watch_interval',
            type=self._positive_int,
            action="store",
            default=None
        )

    # --------------------------------------------------------------------------
    def processMessageLists(self, options):
        for cfg_path in ["/etc/eagine/xml_logs.json","~/.config/eagine/xml_logs.json"]:
            try:
                with open(os.path.expanduser(cfg_path), "r") as cfgfd:
                    cfg = json.load(cfgfd)
                    l = cfg.get("block_list")
                    if l is not None:
                        options.block_list.append([self._valid_msg_id(e) for e in l])
            except:
                pass

        options.block_list = [i for sl in options.block_list for i in sl]

    # --------------------------------------------------------------------------
    def processParsedOptions(self, options):
        self.processMessageLists(options)

        return options

    # --------------------------------------------------------------------------
    def parseArgs(self):
        class _Options(object):
            # ------------------------------------------------------------------
            def __init__(self, options):
                self.__dict__.update(options.__dict__)

            # ------------------------------------------------------------------
            def dbPassword(self):
                if self.db_password is None:
                    def _getpwd(passfd):
                        return passfd.readline().rstrip().split(":")[-1]

                    try:
                        with open(self.db_password_file, "r") as passfd:
                            self.db_password = _getpwd(passfd)
                    except:
                        try:
                            with open(os.path.expanduser("~/.pgpass"), "r") as passfd:
                                self.db_password = _getpwd(passfd)
                        except:
                            self.db_password = getpass.getpass("psql password: ")
                return self.db_password

            # ------------------------------------------------------------------
            def isScreenCommand(self):
                return self.watch_interval is not None
            # ------------------------------------------------------------------
            def command(self):
                if self.watch_interval is not None:
                    return "watch"
                if self.dump:
                    return "dump"
                if self.recent:
                    return "recent"
                return "tail"

        return _Options(self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)))

# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Formats the EAGine logging entries from a PostgreSQL database.
        """
    )
# ------------------------------------------------------------------------------
# Formatting utilities
# ------------------------------------------------------------------------------
class LogFormattingUtils(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._initFormatters()

    # --------------------------------------------------------------------------
    def _ttyEsc(self, escseq):
        if self._istty:
            return escseq
        return ""

    # --------------------------------------------------------------------------
    def columns(self):
        return 90

    # --------------------------------------------------------------------------
    def centerText(self, t, w):
        pr = (w - len(t)) // 2
        pl = w - pr - len(t)
        return " "*pl + t + " "*pr

    # --------------------------------------------------------------------------
    def getCenterText(self, w):
        if w > 0:
            return lambda t: self.centerText(t, w)
        return lambda t: t

    # --------------------------------------------------------------------------
    def formatDuration(self, s, w = 0, n_a = "N/A"):
        c = self.getCenterText(w)
        if s is None:
            return c(n_a)
        if isinstance(s, str):
            s = float(s)
        if isinstance(s, datetime.timedelta):
            s = s.total_seconds()
        if s >= 60480000:
            return c("%dw" % (int(s) / 604800))
        if s >= 604800:
            return c("%2dw %2dd" % (int(s) / 604800, (int(s) % 604800) / 86400))
        if s >= 86400:
            return c("%2dd %2dh" % (int(s) / 86400, (int(s) % 86400) / 3600))
        if s >= 3600:
            return c("%2dh %02dm" % (int(s) / 3600, (int(s) % 3600) / 60))
        if s >= 60:
            return c("%2dm %02ds" % (int(s) / 60, int(s) % 60))
        if s >= 10:
            return c("%3ds" % int(s))
        if s >= 0.01:
            return c("%4dms" % int(s*10**3))
        if s <= 0.0:
            return c("0")
        return c("%dμs" % int(s*10**6))

    # --------------------------------------------------------------------------
    def formatIdentifier(self, i, w = 0):
        c = self.getCenterText(w)
        return c(i)

    # --------------------------------------------------------------------------
    def formatYesNoMaybe(self, v, w = 0):
        s = "maybe"
        if isinstance(v, bool):
            if v == True:
                s = "yes"
            elif v == False:
                s = "no"
        c = self.getCenterText(w)
        return c(s)

    # --------------------------------------------------------------------------
    def formatLogSeverity(self, level, w = 0):
        c = self.getCenterText(w)
        if level == "stat":
            return c("statistic")
        return c(level)

    # --------------------------------------------------------------------------
    def formatInstance(self, instance):
        instance = hash(instance) % ((sys.maxsize + 1) * 2)
        instance = instance.to_bytes(8, byteorder='big')
        instance = base64.b64encode(instance, altchars=b"'-")
        instance = instance.decode("utf-8")
        return instance

    # --------------------------------------------------------------------------
    def formatProgressValue(self, name, value, ctx):
        try:
            vmin = ctx["args"][name]["min"]
            vmax = ctx["args"][name]["max"]
            return "%3.1f%%" % (100.0 * (value - vmin) / (vmax - vmin), )
        except:
            return "N/A"

    # --------------------------------------------------------------------------
    def formatProgressBar(self, width, vmin, vmax, value):
        try:
            try:
                coef = (value - vmin) / (vmax - vmin)
            except ZeroDivisionError:
                coef = 0.0

            result = "% 5.1f%%├" % (100.0 * coef,)
            width = max(width - 7, 0)

            pos = coef * float(width)
            cnt = int(pos)

            i = cnt
            result += "█" * cnt

            if i < width:
                parts = [" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"]
                result += parts[int(9 * (pos - float(cnt)))]
                i += 1

            result += "┈" * max(width - i, 0)
            result += "┤"

            return result
        except Exception as err:
            return ("░" * width) + "│"

    # --------------------------------------------------------------------------
    def _initFormatters(self):
        self._formatters = {
            "duration": lambda n,v,c: self.formatDuration(v),
            "identifier": lambda n,v,c: self.formatIdentifier(v),
            "YesNo": lambda n,v,c: self.formatYesNoMaybe(v),
            "YesNoMaybe": lambda n,v,c: self.formatYesNoMaybe(v),
            "MainPrgrss": lambda n,v,c: self.formatProgressValue(n, v, c),
            "Progress": lambda n,v,c: self.formatProgressValue(n, v, c)
        }

    # --------------------------------------------------------------------------
    def formatMessageArg(self, name, typ, value, ctx = None):
        try:
            return self._formatters[typ](name, value, ctx)
        except KeyError:
            return str(value)

# ------------------------------------------------------------------------------
# StdOut log output
# ------------------------------------------------------------------------------
class LogStdOut(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._out = sys.stdout
        self._istty = self._out.isatty()
        self. _color_esc = {
            "Red": "\x1b[0;31m",
            "Green": "\x1b[0;32m",
            "Yellow": "\x1b[0;33m",
            "Blue": "\x1b[0;34m",
            "Cyan": "\x1b[0;36m",
            "White": "\x1b[0;37m",
            "Gray": "\x1b[0;30m",
            "BoldRed": "\x1b[1;31m",
            "BoldGreen": "\x1b[1;32m",
            "BoldYellow": "\x1b[1;33m",
            "BoldBlue": "\x1b[1;34m",
            "BoldCyan": "\x1b[1;36m",
            "BoldWhite": "\x1b[1;37m",
            "BoldGray": "\x1b[0;37m"
        }

    # --------------------------------------------------------------------------
    def _ttyEsc(self, escseq):
        if self._istty:
            return escseq
        return ""

    # --------------------------------------------------------------------------
    def _ttyReset(self):
        return self._ttyEsc("\x1b[0m")

    # --------------------------------------------------------------------------
    def _ttyInvert(self):
        return self._ttyEsc("\x1b[7m")

    # --------------------------------------------------------------------------
    def _ttyColor(self, color):
        return self._ttyEsc(self._color_esc[color])

    # --------------------------------------------------------------------------
    def lines(self):
        try:
            return os.get_terminal_size().lines
        except:
            try:
                return int(os.getenv("LINES", 50))
            except:
                return 50

    # --------------------------------------------------------------------------
    def columns(self):
        try:
            return os.get_terminal_size().columns
        except:
            try:
                return int(os.getenv("COLUMNS", 80))
            except:
                return 80

    # --------------------------------------------------------------------------
    def begin(self):
        pass

    # --------------------------------------------------------------------------
    def write(self, what):
        self._out.write(what)

    # --------------------------------------------------------------------------
    def writeColor(self, what, color):
        self._out.write(self._ttyColor(color) + what + self._ttyReset())

    # --------------------------------------------------------------------------
    def writeProgress(self, what, index):
        pass

    # --------------------------------------------------------------------------
    def finish(self):
        pass

# ------------------------------------------------------------------------------
# Curses window log output
# ------------------------------------------------------------------------------
class LogCursesOut(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, window):
        self._window = window
        self._line = None
        self._lines = None
        self._re_color = re.compile("[^{]*(([+-]){{([A-Za-z]+)}}).*")
        self._color_ids = {
            "White": 0,
            "Red": 1,
            "Yellow": 2,
            "Green": 3,
            "Cyan": 4,
            "Blue": 5,
            "Gray": 0
        }

    # --------------------------------------------------------------------------
    def lines(self):
        return curses.LINES

    # --------------------------------------------------------------------------
    def columns(self):
        return curses.COLS

    # --------------------------------------------------------------------------
    def begin(self):
        self._line = ""
        self._lines = []
        self._progress = {}

    # --------------------------------------------------------------------------
    def write(self, what):
        temp = what.split('\n')
        if len(temp) > 1:
            self._lines.append(self._line + temp[0])
            for line in temp[1:-1]:
                self._lines.append(line)
            self._line = temp[-1]
        else:
            self._line += what

    # --------------------------------------------------------------------------
    def writeColor(self, what, color):
        return self.write("+{{" + color + "}}" + what + "-{{" + color + "}}")

    # --------------------------------------------------------------------------
    def writeProgress(self, what, index):
        try:
            self._progress[index] += what
        except KeyError:
            self._progress[index] = what

    # --------------------------------------------------------------------------
    def changeAttrib(self, attrib, on):
        f = self._window.attron if on else self._window.attroff
        if attrib.startswith("Bold"):
            f(curses.A_BOLD)
            attrib = attrib[4:]
        f(curses.color_pair(self._color_ids.get(attrib, 0)))

    # --------------------------------------------------------------------------
    def finish(self):
        self._window.clear()
        for line in self._lines[-curses.LINES+1:]:
            found = re.match(self._re_color, line)
            while found:
                self._window.addstr(line[:found.start(1)])
                self.changeAttrib(found.group(3), found.group(2) == '+')
                line = line[found.end(1):]
                found = re.match(self._re_color, line)

            self._window.addstr(line+'\n')
        for prog_index, line in sorted(self._progress.items(), key = lambda kv: kv[0]):
            self._window.addstr(prog_index, 0, line)
        self._window.refresh()
        self._line = None

# ------------------------------------------------------------------------------
# Log entry renderer
# ------------------------------------------------------------------------------
class LogRenderer(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, output):
        self._options = options
        self._output = output
        self._utils = LogFormattingUtils(options)
        self._re_var = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")
        self._progress = {}
        self._all_streams = set()
        self._current_stream_attrib = DbMetadata.getDefaultAttrib()
        self._severity_colors = {
            "backtrace": "Cyan",
            "trace": "Cyan",
            "debug": "BoldCyan",
            "change": "BoldYellow",
            "info": "White",
            "stat": "BoldWhite",
            "warning": "Yellow",
            "error": "BoldRed",
            "critical": "Red"
        }

    # --------------------------------------------------------------------------
    def linesPerEntry(self):
        return 3

    # --------------------------------------------------------------------------
    def minEntriesPerScreen(self):
        return 1 + self._output.lines() // self.linesPerEntry()

    # --------------------------------------------------------------------------
    def write(self, what):
        self._output.write(what)

    # --------------------------------------------------------------------------
    def writeColor(self, what, color):
        self._output.writeColor(what, color)

    # --------------------------------------------------------------------------
    def writeProgress(self, what, index):
        self._output.writeProgress(what, index)

    # --------------------------------------------------------------------------
    def alwaysTranslateAsList(self, values):
        # TODO
        return False

    # --------------------------------------------------------------------------
    def doFormatMsgArg(self, name, typ, value, ctx):
        return self._utils.formatMessageArg(name, typ, value, ctx)

    # --------------------------------------------------------------------------
    def formatMessageArg(self, name, arg_info, ctx):
        arg_info["info"] = True
        arg_type = arg_info.get("type")
        values = arg_info.get("values", [])
        if len(values) == 1 and not self.alwaysTranslateAsList(values):
            return self.doFormatMsgArg(name, arg_type, values[0], ctx)
        values = [self.doFormatMsgArg(arg, arg_type, v, ctx) for v in values]
        return '[' + ", ".join(values) + ']'

    # --------------------------------------------------------------------------
    def formatMessage(self, data):
        message = data["format"]
        args = data["args"]
        found = re.match(self._re_var, message)
        while found:
            prev = message[:found.start(1)]
            repl = self.formatMessageArg(
                found.group(2),
                args.get(found.group(2), {}),
                data)
            folw = message[found.end(1):]
            message = prev + repl + folw
            found = re.match(self._re_var, message)
        return message

    # --------------------------------------------------------------------------
    def renderLogSeverity(self, level, w = 0):
        return self.writeColor(
            self._utils.formatLogSeverity(level, w),
            self._severity_colors[level])

    # --------------------------------------------------------------------------
    def renderEntryConnectorsHead(self, data):
        curr_streams = data["streams"]
        stream_id = data["stream_id"]

        l = len(curr_streams)
        i = 0
        self.write("┊")
        while i < l:
            if curr_streams[i] == stream_id:
                break
            self.write(" │")
            i += 1
        self.write(" ┝")
        while i < l - 1:
            self.write("━━")
            i += 1
        self.write("━┑")

    # --------------------------------------------------------------------------
    def renderEntryConnectorsPass(self, l):
        i = 0
        self.write("┊")
        while i < l:
            self.write(" │")
            i += 1

    # --------------------------------------------------------------------------
    def renderEntryConnectorsTail(self, data):
        curr_streams = data["streams"]
        stream_id = data["stream_id"]

        l = len(curr_streams)
        i = 0
        self.write("┊")
        switch = False
        while i < l:
            if curr_streams[i] == stream_id:
                switch = True
                self.write(" ┕")
            elif switch:
                self.write("━━")
            else:
                self.write(" │")
            i += 1

    # --------------------------------------------------------------------------
    def entryAgeColor(self, age):
        if isinstance(age, datetime.timedelta):
            age = age.total_seconds()
        if age < 10:
            return "BoldGreen"
        if age < 60:
            return "Green"
        if age < 120:
            return "BoldYellow"
        if age < 300:
            return "Yellow"
        if age < 900:
            return "BoldRed"
        return "Red"

    # --------------------------------------------------------------------------
    def renderEntry(self, data):
        ls = len(data["streams"])
        age = data["age"]
        tag = data["tag"]

        self.renderEntryConnectorsHead(data)
        self.writeColor(
            self._utils.formatDuration(age, 9),
            self.entryAgeColor(age))
        self.write("│")
        self.write(self._utils.formatDuration(data["time_since_start"], 9))
        self.write("│")
        self.write(self._utils.formatDuration(data["time_since_prev"], 9))
        self.write("│")
        self.renderLogSeverity(data["severity"], 9)
        self.write("│")
        self.writeColor(
            self._utils.formatIdentifier(data["application_id"], 10),
            "Blue")
        self.write("│")
        self.writeColor(
            self._utils.formatIdentifier(data["source_id"], 10),
            "BoldBlue")
        self.write("│")

        if tag is not None:
            self.writeColor(
                self._utils.formatIdentifier(tag, 10),
                "Cyan")
            self.write("│")

        self.write("%12s│" % self._utils.formatInstance(data["instance"]))
        self.write("\n")

        self.renderEntryConnectorsPass(ls)
        self.write(" ├─────────┴─────────┴─────────┴─────────┴")
        if tag is not None:
            self.write("──────────┴")
        self.write("──────────┴──────────┴────────────╯\n")

        cols = self._utils.columns() - (ls * 2)
        lno = 0
        for line in textwrap.wrap(self.formatMessage(data), cols):
            self.renderEntryConnectorsPass(ls)
            if lno == 0:
                self.write(" ╰╴")
            else:
                self.write("   ")
            lno += 1
            self.write(line)
            self.write("\n")

    # --------------------------------------------------------------------------
    def renderStreamHead(self, data):
        ls = len(data["streams"])

        self.renderEntryConnectorsPass(ls-1)
        self.write("   ╭────────────┬──────────╮\n")
        #
        i = 1
        self.write("┝")
        while i < ls:
            self.write("━━")
            i += 1
        self.write("━┯━┥")
        self._output.writeColor("  starting  ", "Yellow")
        self.write("│")
        self.write("% 10s" % data["application_id"])
        self.write("│\n")
        #
        self.renderEntryConnectorsPass(ls-1)
        self.write(" │ ╰────────────┴──────────╯\n")

    # --------------------------------------------------------------------------
    def renderStreamTail(self, data):
        ls = len(data["streams"])

        self.renderEntryConnectorsPass(ls)
        self.write(" ╭────────────┬──────────╮\n")
        self.renderEntryConnectorsTail(data)
        self.write("━┥")
        self._output.writeColor("  finished  ", "Blue")
        self.write("│")
        self.write("% 10s" % data["application_id"])
        self.write("│\n")
        self.renderEntryConnectorsPass(ls-1)
        self.write("   ╰────────────┴──────────╯\n")

    # --------------------------------------------------------------------------
    def trackProgress(self, data):
        for arg_data in data.get("args", {}).values():
            key = (data["application_id"], data["source_id"], data["instance"])
            entry_time = data["entry_time"]
            is_progress = arg_data.get("type") == "MainPrgrss"
            try:
                item = self._progress[key]
            except KeyError:
                if not is_progress:
                    key = data["application_id"]
                item = self._progress[key] = {
                    "start_time": entry_time,
                    "update_time": entry_time
                }
            item["stream_id"] = data["stream_id"]
            item["streams"] = data["streams"]
            item["time_since_stream_start"] = data["time_since_start"]
            item["update_time"] = entry_time
            if is_progress:
                try: del self._progress[data["application_id"]]
                except: pass
                value = arg_data["values"][0]
                if "min" in item:
                    item["min"] = min(item["min"], arg_data["min"])
                else:
                    item["min"] = arg_data["min"]
                if "max" in item:
                    item["max"] = max(item["max"], arg_data["max"])
                else:
                    item["max"] = arg_data["max"]
                if "start_value" not in item:
                    item["start_value"] = value
                item["value"] = value

    # --------------------------------------------------------------------------
    def progressStalled(self, data):
        try:
            now = datetime.datetime.now(datetime.timezone.utc)
            return now - data["update_time"]
        except:
            return None

    # --------------------------------------------------------------------------
    def progressElapsed(self, data):
        time_since_start = data.get("time_since_stream_start")
        try:
            now = datetime.datetime.now(datetime.timezone.utc)
            duration1 = time_since_start
            duration1 = duration1.total_seconds()
            div1 = data["value"] - data["min"]

            duration2 = now - data["start_time"]
            duration2 = duration.total_seconds()
            div2 = data["value"] - data["start_value"]

            coef = data["value"] - data["min"]

            def _blend(l, r, f):
                return l * (1.0 - f) + r * f

            return _blend(
                duration1 * coef / div1,
                duration2 * coef / div2,
                math.exp(-coef / div2))

            return duration * (1.0 + coef)
        except:
            return time_since_start

    # --------------------------------------------------------------------------
    def progressEstimated(self, data):
        try:
            now = datetime.datetime.now(datetime.timezone.utc)
            duration1 = data.get("time_since_stream_start")
            duration1 = duration1.total_seconds()
            div1 = data["value"] - data["min"]

            duration2 = now - data["start_time"]
            duration2 = duration2.total_seconds()
            div2 = data["value"] - data["start_value"]

            coef = data["max"] - data["value"]

            def _blend(l, r, f):
                return l * (1.0 - f) + r * f

            return _blend(
                duration1 * coef / div1,
                duration2 * coef / div2,
                math.exp(-coef / div2))
        except:
            return None

    # --------------------------------------------------------------------------
    def keyProgressByStreamId(self, entry):
        return entry[1]["stream_id"]

    # --------------------------------------------------------------------------
    def keyProgressByValue(self, entry):
        data = entry[1]
        try:
            return (data["value"] - data["min"]) / (data["max"] - data["min"])
        except:
            return data["value"]

    # --------------------------------------------------------------------------
    def keyProgressByRemaining(self, entry):
        data = entry[1]
        try:
            return (data["max"] - data["value"]) / (data["max"] - data["min"])
        except:
            return data["value"]

    # --------------------------------------------------------------------------
    def keyProgressByUpdate(self, entry):
        return entry[1]["update_time"]

    # --------------------------------------------------------------------------
    def progressSorter(self):
        if self._options.sort_progress == "value":
            return self.keyProgressByValue
        if self._options.sort_progress == "remaining":
            return self.keyProgressByRemaining
        if self._options.sort_progress == "update":
            return self.keyProgressByUpdate
        return self.keyProgressByStreamId

    # --------------------------------------------------------------------------
    def formatCurrentStreamAttrib(self, metadata, stream_id):
        attr_name, attr_type, attr_value = metadata.getStreamInfo(
            stream_id,
            self._current_stream_attrib)
        return "%s: %s" % (
            attr_name,
            self._utils.formatMessageArg(attr_name, attr_type, attr_value))

    # --------------------------------------------------------------------------
    def renderProgress(self, metadata):
        idx = 0
        curr_streams = [x for x in sorted(self._all_streams)]
        self._progress = {
            k: v for k, v in self._progress.items()\
            if v["stream_id"] in self._all_streams}

        def _writeProgress(what):
            self.writeProgress(what, idx)

        stream_attrib = 'os_pid'

        for key, data in sorted(self._progress.items(), key=self.progressSorter()):
            stream_id = data["stream_id"]
            l = len(curr_streams)
            i = 0
            _writeProgress("┊")
            width = 1
            while i < l:
                if curr_streams[i] == stream_id:
                    break
                _writeProgress(" │")
                width += 2
                i += 1
            _writeProgress(" ┝")
            width += 2
            while i < l - 1:
                _writeProgress("━━")
                width += 2
                i += 1
            _writeProgress("━┥")
            width += 2
            stalled = self.progressStalled(data)
            _writeProgress(self._utils.formatDuration(stalled, 9))
            _writeProgress("│")
            width += 10
            elapsed = self.progressElapsed(data)
            _writeProgress(self._utils.formatDuration(elapsed, 9))
            _writeProgress("│")
            width += 10
            estimated = self.progressEstimated(data)
            _writeProgress(self._utils.formatDuration(estimated, 9))
            _writeProgress("│")
            width += 10
            width = self._output.columns() - width - 1
            _writeProgress(
                self._utils.formatProgressBar(
                    55, data.get("min"), data.get("max"), data.get("value")))
            width += 55
            attribute = self.formatCurrentStreamAttrib(metadata, stream_id)
            _writeProgress(attribute)
            width += len(attribute)
            _writeProgress("\n")
            idx += 1
        _writeProgress("┊")
        for unused  in curr_streams:
            _writeProgress(" ╎")
        _writeProgress("\n")
        self._current_stream_attrib = metadata.getNextAttrib(self._current_stream_attrib)

    # --------------------------------------------------------------------------
    def processBegin(self):
        self._output.begin()
        self._all_streams.clear()

    # --------------------------------------------------------------------------
    def processEntry(self, data, metadata):
        if data["is_first"]:
            self.renderStreamHead(data)

        for stream_id in data["streams"]:
            self._all_streams.add(stream_id)
        self.trackProgress(data)
        self.renderEntry(data)

        if data["is_last"]:
            self._all_streams.remove(data["stream_id"])
            self.renderStreamTail(data)

    # --------------------------------------------------------------------------
    def processEnd(self, metadata):
        if self._options.show_progress:
            self.renderProgress(metadata)
        self._output.finish()

# ------------------------------------------------------------------------------
#  Metadata
# ------------------------------------------------------------------------------
class DbMetadata(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, db_conn):
        self._db_conn = db_conn
        self._attrib_map = {
            "os_pid": ("PID", "int64"),
            "hostname": ("Host", "Hostname"),
            "os_name": ("OS", "string"),
            "application_id": ("App.", "identifier"),
            "architecture": ("Arch.", "string"),
            "git_version": ("Version", "string"),
            "debug_build": ("Debug", "YesNoMaybe"),
            "low_profile_build": ("Low-profile", "YesNoMaybe"),
            "running_on_valgrind": ("Valgrind", "YesNoMaybe")
        }
        self._attrib_columns = list(self._attrib_map.keys())
        self._stream_metadata = {}

    # --------------------------------------------------------------------------
    @staticmethod
    def getDefaultAttrib():
        return 'os_pid'

    # --------------------------------------------------------------------------
    def getNextAttrib(self, current):
        newidx = self._attrib_columns.index(current) + 1
        newidx = newidx % len(self._attrib_columns)
        return self._attrib_columns[newidx]

    # --------------------------------------------------------------------------
    def queryStreamMetadata(self, stream_id):
        with self._db_conn.cursor() as stream_attr:
            query = "SELECT %s FROM eagilog.stream WHERE stream_id = %%s" % (
                ", ".join(self._attrib_columns))
            stream_attr.execute(query, (stream_id,))
            attrib_values = stream_attr.fetchone()
            return {a: v for a, v in zip(self._attrib_columns, attrib_values)}

    # --------------------------------------------------------------------------
    def getStreamMetadata(self, stream_id):
        try:
            return self._stream_metadata[stream_id]
        except KeyError:
            self._stream_metadata[stream_id] = self.queryStreamMetadata(stream_id)
            return self._stream_metadata[stream_id]

    # --------------------------------------------------------------------------
    def getStreamAttribute(self, stream_id, attrib):
        return self.getStreamMetadata(stream_id).get(attrib)

    # --------------------------------------------------------------------------
    def getStreamPID(self, stream_id):
        return self.getStreamAttribute(stream_id, 'os_pid')

    # --------------------------------------------------------------------------
    def getStreamInfo(self, stream_id, attrib):
        name, typ = self._attrib_map.get(attrib, ("unknown", "string"))
        return name, typ, self.getStreamAttribute(stream_id, attrib)

# ------------------------------------------------------------------------------
# Database reader
# ------------------------------------------------------------------------------
class DbReader(object):
    # --------------------------------------------------------------------------
    def __init__(self, options, db_conn):
        self._options = options
        self._db_conn = db_conn
        self._pg_curr = db_conn.cursor()
        self._pg_curr.__enter__()
        self._db_metadata = DbMetadata(options, db_conn)

        self.applyBlockList()

    # --------------------------------------------------------------------------
    def __del__(self):
        self.finishSession()
        self._pg_curr.__exit__()

    # --------------------------------------------------------------------------
    def applyBlockList(self):
        for application_id, source_id, tag in self._options.block_list:
            assert application_id or source_id or tag
            self._pg_curr.execute(
                "SELECT eagilog.client_session_block_entry_message(%s, %s, %s)",
                (application_id, source_id, tag))

    # --------------------------------------------------------------------------
    def finishSession(self):
        self._pg_curr.execute("SELECT eagilog.finish_client_session()")

    # --------------------------------------------------------------------------
    def castArgValue(self, v):
        if isinstance(v, decimal.Decimal):
            return int(v)
        return v

    # --------------------------------------------------------------------------
    def processEntry(self, processor, data):
        args = {}
        with self._db_conn.cursor() as entry_args:
            entry_args.execute("""
                SELECT
                    arg_id,
                    arg_order,
                    arg_type,
                    min_value,
                    max_value,
                    value_integer,
                    value_string,
                    value_float,
                    value_duration,
                    value_boolean
                FROM eagilog.args_of_entry(%s)
            """, (data["entry_id"],))
            while True:
                arg_data = entry_args.fetchone()
                if arg_data is None:
                    break
                try:
                    arg = args[arg_data[0]]
                except KeyError:
                    arg = args[arg_data[0]] = {
                        "used": False,
                        "order": arg_data[1],
                        "type": arg_data[2],
                        "values": []}
                arg["values"] +=\
                    [self.castArgValue(v) for v in arg_data[5:] if v is not None]
                if arg_data[3] is not None:
                    if "min" in arg:
                        arg["min"] = min(arg["min"], arg_data[3])
                    else:
                        arg["min"] = arg_data[3]
                if arg_data[4] is not None:
                    if "max" in arg:
                        arg["max"] = max(arg["max"], arg_data[4])
                    else:
                        arg["max"] = arg_data[4]

        data["args"] = args
        return processor.processEntry(data, self._db_metadata)

    # --------------------------------------------------------------------------
    def processEntries(self, processor, entries):
        columns = [
            "streams",
            "stream_id",
            "entry_id",
            "severity",
            "application_id",
            "source_id",
            "instance",
            "tag",
            "format",
            "entry_time",
            "time_since_start",
            "is_first",
            "is_last",
            "failed"
        ]
        processor.processBegin()
        stream_prev_times = {}
        while True:
            data = entries.fetchone()
            if data is None:
                break
            data = {k:v for k,v in zip(columns, data)}
            stream_id = data["stream_id"]

            time_since_start = data["time_since_start"]
            data["time_since_prev"] =\
                time_since_start -\
                stream_prev_times.get(stream_id, datetime.timedelta(0))
            data["age"] = datetime.datetime.now(datetime.timezone.utc) - data["entry_time"]
            self.processEntry(processor, data)
            stream_prev_times[stream_id] = time_since_start
        processor.processEnd(self._db_metadata)

    # --------------------------------------------------------------------------
    def processQuery(self, processor, query, args = None):
            self._pg_curr.execute(query, args)
            self.processEntries(processor, self._pg_curr)

    # --------------------------------------------------------------------------
    def read(self, processor):
        self.processQuery(
            processor, """
                SELECT eagilog.contemporary_streams(entry_time), *
                FROM eagilog.stream_entry
                ORDER BY entry_time
            """)
# ------------------------------------------------------------------------------
# Database connection
# ------------------------------------------------------------------------------
class DbConnection(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._pg_conn = psycopg2.connect(
            user=options.db_user,
            password=options.dbPassword(),
            database=options.db_name,
            host=options.db_host,
            port=options.db_port)
        self._pg_conn.__enter__()
    # --------------------------------------------------------------------------
    def __del__(self):
        self._pg_conn.close()
    # --------------------------------------------------------------------------
    def cursor(self):
        return self._pg_conn.cursor()

# ------------------------------------------------------------------------------
# screen commands
# ------------------------------------------------------------------------------
def watch(options, reader, window):
    renderer = LogRenderer(options, LogCursesOut(options, window))
    while True:
        reader.processQuery(
            renderer,
            "SELECT * FROM eagilog.latest_client_stream_entries(%s)",
            (renderer.minEntriesPerScreen(),))
        time.sleep(float(options.watch_interval))
# ------------------------------------------------------------------------------
# stdout commands
# ------------------------------------------------------------------------------
def dump(options, reader):
    renderer = LogRenderer(options, LogStdOut(options))
    reader.processQuery(
        renderer, """
                SELECT eagilog.contemporary_streams(entry_time), *
                FROM eagilog.stream_entry
                ORDER BY entry_time
        """, (renderer.minEntriesPerScreen(),))
# ------------------------------------------------------------------------------
def recent(options, reader):
    renderer = LogRenderer(options, LogStdOut(options))
    reader.processQuery(
        renderer, """
                SELECT *
                FROM eagilog.recent_stream_entries()
        """, (renderer.minEntriesPerScreen(),))
# ------------------------------------------------------------------------------
def tail(options, reader):
    renderer = LogRenderer(options, LogStdOut(options))
    reader.processQuery(
        renderer,
        "SELECT * FROM eagilog.latest_client_stream_entries(%s)",
        (renderer.minEntriesPerScreen(),))
# ------------------------------------------------------------------------------
# command dispatch
# ------------------------------------------------------------------------------
def screenCommand(screen, options, db_conn):
    curses.initscr()
    curses.start_color()
    curses.use_default_colors()
    curses.init_pair(0, curses.COLOR_WHITE, -1)
    curses.init_pair(1, curses.COLOR_RED, -1)
    curses.init_pair(2, curses.COLOR_YELLOW, -1)
    curses.init_pair(3, curses.COLOR_GREEN, -1)
    curses.init_pair(4, curses.COLOR_CYAN, -1)
    curses.init_pair(5, curses.COLOR_BLUE, -1)
    curses.curs_set(False)
    try:
        reader = DbReader(options, db_conn)
        watch(options, reader, screen)
    except KeyboardInterrupt:
        pass
    finally:
        return [screen.instr(l+2, 0).decode("utf-8") for l in range(curses.LINES-2)]
# ------------------------------------------------------------------------------
def normalCommand(options, db_conn):
    cmd = options.command()
    try:
        reader = DbReader(options, db_conn)
        if cmd == "recent":
            return recent(options, reader)
        if cmd == "dump":
            return dump(options, reader)
        if cmd == "tail":
            pass
        return tail(options, reader)
    finally:
        pass
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_psql_logs_cl",
            "eagine-psql-logs-cl",
            ["--print-bash-completion"],
            fd)
    if options.print_bash_completion == "-":
        _printIt(sys.stdout)
    else:
        with open(options.print_bash_completion, "wt") as fd:
            _printIt(fd)

# ------------------------------------------------------------------------------
# main
# ------------------------------------------------------------------------------
def main():
    argparser = getArgumentParser()
    options = argparser.parseArgs()
    if options.print_bash_completion:
        printBashCompletion(argparser, options)
    else:
        db_conn = DbConnection(options)
        if options.isScreenCommand():
            print(str("\n").join(curses.wrapper(screenCommand, options, db_conn)))
        else:
            normalCommand(options, db_conn)
    return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
    sys.exit(main())


