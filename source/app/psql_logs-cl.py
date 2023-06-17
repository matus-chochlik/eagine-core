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
    # -------------------------------------------------------------------------
    def _positive_int(self, x):
        try:
            assert(int(x) > 0)
            return int(x)
        except:
            self.error("`%s' is not a positive integer value" % str(x))
    # -------------------------------------------------------------------------
    def __init__(self, **kw):

        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--db-host", "-H",
            dest='db_host',
            action="store",
            default="localhost"
        )

        self.add_argument(
            "--db-port", "-P",
            dest='db_port',
            type=self._positive_int,
            action="store",
            default=5432
        )

        self.add_argument(
            "--db-name", "-D",
            dest='db_name',
            action="store",
            default="eagilog"
        )

        self.add_argument(
            "--db-user", "-u",
            dest='db_user',
            action="store",
            default="eagilog"
        )

        self.add_argument(
            "--db-password", "-w",
            dest='db_password',
            action="store",
            default=None
        )

        self.add_argument(
            "--db-pass-file",
            dest='db_password_file',
            action="store",
            default=None
        )

        self.add_argument(
            "--watch",
            dest='watch_interval',
            type=self._positive_int,
            action="store",
            default=None
        )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, options):

        if options.db_password is None:
            def _getpwd(passfd):
                return passfd.readline().rstrip().split(":")[-1]

            try:
                with open(options.db_password_file, "r") as passfd:
                    options.db_password = _getpwd(passfd)
            except:
                try:
                    with open(os.path.expanduser("~/.pgpass"), "r") as passfd:
                        options.db_password = _getpwd(passfd)
                except:
                    options.db_password = getpass.getpass("psql password: ")

        return options

    # -------------------------------------------------------------------------
    def parse_args(self):
        return self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self))

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
    # -------------------------------------------------------------------------
    def __init__(self, window, options):
        self._window = window
        self._options = options
        self._out = sys.stdout
        self._istty = window is None and self._out.isatty()

    # --------------------------------------------------------------------------
    def _ttyEsc(self, escseq):
        if self._istty:
            return escseq
        return ""

    # -------------------------------------------------------------------------
    def columns(self):
        return 80

    # --------------------------------------------------------------------------
    def ttyReset(self):
        return self._ttyEsc("\x1b[0m")

    # --------------------------------------------------------------------------
    def ttyInvert(self):
        return self._ttyEsc("\x1b[7m")

    # --------------------------------------------------------------------------
    def ttyGray(self):
        return self._ttyEsc("\x1b[0;30m")

    # --------------------------------------------------------------------------
    def ttyRed(self):
        return self._ttyEsc("\x1b[0;31m")

    # --------------------------------------------------------------------------
    def ttyGreen(self):
        return self._ttyEsc("\x1b[0;32m")

    # --------------------------------------------------------------------------
    def ttyYellow(self):
        return self._ttyEsc("\x1b[0;33m")

    # --------------------------------------------------------------------------
    def ttyBlue(self):
        return self._ttyEsc("\x1b[0;34m")

    # --------------------------------------------------------------------------
    def ttyCyan(self):
        return self._ttyEsc("\x1b[0;36m")

    # --------------------------------------------------------------------------
    def ttyWhite(self):
        return self._ttyEsc("\x1b[0;37m")

    # --------------------------------------------------------------------------
    def ttyBoldGray(self):
        return self._ttyEsc("\x1b[0;37m")

    # --------------------------------------------------------------------------
    def ttyBoldRed(self):
        return self._ttyEsc("\x1b[1;31m")

    # --------------------------------------------------------------------------
    def ttyBoldGreen(self):
        return self._ttyEsc("\x1b[1;32m")

    # --------------------------------------------------------------------------
    def ttyBoldYellow(self):
        return self._ttyEsc("\x1b[1;33m")

    # --------------------------------------------------------------------------
    def ttyBoldBlue(self):
        return self._ttyEsc("\x1b[1;34m")

    # --------------------------------------------------------------------------
    def ttyBoldCyan(self):
        return self._ttyEsc("\x1b[1;36m")

    # --------------------------------------------------------------------------
    def ttyBoldWhite(self):
        return self._ttyEsc("\x1b[1;37m")

    # -------------------------------------------------------------------------
    def centerText(self, t, w):
        pr = (w - len(t)) // 2
        pl = w - pr - len(t)
        return " "*pl + t + " "*pr

    # -------------------------------------------------------------------------
    def getCenterText(self, w):
        return lambda t: self.centerText(t, w) if w > 0 else lambda t: t

    # -------------------------------------------------------------------------
    def formatDuration(self, s, w = 0):
        c = self.getCenterText(w)
        if s is None:
            return c("N/A")
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

    # -------------------------------------------------------------------------
    def formatIdentifier(self, i, w = 0):
        c = self.getCenterText(w)
        return self.ttyBoldCyan() + c(i) + self.ttyReset()

    # --------------------------------------------------------------------------
    def formatLogSeverity(self, level, w = 0):
        c = self.getCenterText(w)
        if level == "debug":
            return self.ttyBoldCyan() + c(level) + self.ttyReset()
        if level == "info":
            return self.ttyBoldWhite() + c(level) + self.ttyReset()
        if level == "stat":
            return self.ttyBoldWhite() + c("statistic") + self.ttyReset()
        if level == "change":
            return self.ttyBoldYellow() + c(level) + self.ttyReset()
        if level == "warning":
            return self.ttyYellow() + c(level) + self.ttyReset()
        if level == "error":
            return self.ttyBoldRed() + c(level) + self.ttyReset()
        if level == "trace":
            return self.ttyCyan() + c(level) + self.ttyReset()
        if level == "critical":
            return self.ttyRed() + c(level) + self.ttyReset()
        if level == "backtrace":
            return self.ttyCyan() + c(level) + self.ttyReset()
        return c(level)

    # --------------------------------------------------------------------------
    def formatInstance(self, instance):
        instance = hash(instance) % ((sys.maxsize + 1) * 2)
        instance = instance.to_bytes(8, byteorder='big')
        instance = base64.b64encode(instance, altchars=b"'-")
        instance = instance.decode("utf-8")
        return instance

# ------------------------------------------------------------------------------
# Log entry renderer
# ------------------------------------------------------------------------------
class LogRenderer(object):
    # -------------------------------------------------------------------------
    def __init__(self, window, options):
        self._window = window
        self._utils = LogFormattingUtils(window, options)
        self._re_var = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")

    # -------------------------------------------------------------------------
    def windowLines(self):
        return curses.LINES

    # -------------------------------------------------------------------------
    def linesPerEntry(self):
        return 3

    # -------------------------------------------------------------------------
    def minEntriesPerScreen(self):
        return 1 + self.windowLines() // self.linesPerEntry()

    # -------------------------------------------------------------------------
    def write(self, s):
        temp = s.split('\n')
        if len(temp) > 1:
            self._lines.append(self._line + temp[0])
            for line in temp[1:-1]:
                self._lines.append(line)
            self._line = temp[-1]
        else:
            self._line += s

    # -------------------------------------------------------------------------
    def alwaysTranslateAsList(self, values):
        # TODO
        return False

    # -------------------------------------------------------------------------
    def formatArg(self, name, value, ctx):
        # TODO
        return str(value)

    # -------------------------------------------------------------------------
    def translateArg(self, name, arg_info, ctx):
        arg_info["info"] = True
        values = arg_info.get("values", [])
        if len(values) == 1 and not self.alwaysTranslateAsList(values):
            return self.formatArg(name, values[0], ctx)
        values = [self.doTranslateArg(arg, v, ctx) for v in values]
        return '[' + ", ".join(values) + ']'

    # -------------------------------------------------------------------------
    def formatMessage(self, data):
        message = data["format"]
        args = data["args"]
        found = re.match(self._re_var, message)
        while found:
            prev = message[:found.start(1)]
            repl = self.translateArg(
                found.group(2),
                args.get(found.group(2), {}),
                data)
            folw = message[found.end(1):]
            message = prev + repl + folw
            found = re.match(self._re_var, message)
        return message

    # -------------------------------------------------------------------------
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

    # -------------------------------------------------------------------------
    def renderEntryConnectorsPass(self, l):
        i = 0
        self.write("┊")
        while i < l:
            self.write(" │")
            i += 1

    # -------------------------------------------------------------------------
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

    # -------------------------------------------------------------------------
    def renderEntry(self, data):
        ls = len(data["streams"])
        tag = data["tag"]

        self.renderEntryConnectorsHead(data)
        self.write("%s│" % self._utils.formatDuration(data["time_since_start"], 9))
        self.write("%s│" % self._utils.formatDuration(data["time_since_prev"], 9))
        self.write("%s│" % self._utils.formatLogSeverity(data["severity"], 9))
        self.write("%s│" % self._utils.formatIdentifier(data["application_id"], 10))
        self.write("%s│" % self._utils.formatIdentifier(data["source_id"], 10))
        if tag is not None:
            self.write("%s│" % self._utils.formatIdentifier(tag, 10))
        self.write("%12s│" % self._utils.formatInstance(data["instance"]))
        self.write("\n")
        self.renderEntryConnectorsPass(ls)
        if tag is not None:
            self.write(" ├─────────┴─────────┴─────────┴──────────┴──────────┴──────────┴────────────╯\n")
        else:
            self.write(" ├─────────┴─────────┴─────────┴──────────┴──────────┴────────────╯\n")

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

    # -------------------------------------------------------------------------
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
        self.write(self._utils.ttyYellow()+"  starting  "+self._utils.ttyReset())
        self.write("│")
        self.write("% 10s" % data["application_id"])
        self.write("│\n")
        #
        self.renderEntryConnectorsPass(ls-1)
        self.write(" │ ╰────────────┴──────────╯\n")

    # -------------------------------------------------------------------------
    def renderStreamTail(self, data):
        ls = len(data["streams"])

        self.renderEntryConnectorsPass(ls)
        self.write(" ╭────────────┬──────────╮\n")
        self.renderEntryConnectorsTail(data)
        self.write("━┥")
        self.write(self._utils.ttyBlue()+"  finished  "+self._utils.ttyReset())
        self.write("│")
        self.write("% 10s" % data["application_id"])
        self.write("│\n")
        self.renderEntryConnectorsPass(ls-1)
        self.write("   ╰────────────┴──────────╯\n")

    # -------------------------------------------------------------------------
    def processBegin(self):
        self._line = ""
        self._lines = []
        self._prev_streams = []

    # -------------------------------------------------------------------------
    def processEntry(self, data):
        if data["is_first"]:
            self.renderStreamHead(data)

        self.renderEntry(data)

        if data["is_last"]:
            self.renderStreamTail(data)

        self._prev_streams = data["streams"]

    # -------------------------------------------------------------------------
    def processEnd(self):
        self._window.clear()
        for line in self._lines[-curses.LINES+1:]:
            self._window.addstr(line+'\n')
        self._window.refresh()
        self._prev_streams = None

# ------------------------------------------------------------------------------
# Database reader
# ------------------------------------------------------------------------------
class DbReader(object):
    # -------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._pg_conn = None
        self._pg_conn = psycopg2.connect(
            user=options.db_user,
            password=options.db_password,
            database=options.db_name,
            host=options.db_host,
            port=options.db_port)

    # -------------------------------------------------------------------------
    def castArgValue(self, v):
        if isinstance(v, decimal.Decimal):
            return int(v)
        return v

    # -------------------------------------------------------------------------
    def processEntry(self, processor, data):
        args = {}
        with self._pg_conn.cursor() as entry_args:
            entry_args.execute("""
                SELECT
                    arg_id,
                    arg_order,
                    arg_type,
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
                    [self.castArgValue(v) for v in arg_data[3:] if v is not None]
        data["args"] = args
        return processor.processEntry(data)

    # -------------------------------------------------------------------------
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
            self.processEntry(processor, data)
            stream_prev_times[stream_id] = time_since_start
        processor.processEnd()

    # -------------------------------------------------------------------------
    def processQuery(self, processor, query, args = None):
        pg_conn = self._pg_conn
        with pg_conn:
            with pg_conn.cursor() as entries:
                entries.execute(query, args)
                self.processEntries(processor, entries)

    # -------------------------------------------------------------------------
    def tail(self, processor, count):
        self.processQuery(
            processor,
            "SELECT * FROM eagilog.latest_stream_entries(%s)",
            (count,))

    # -------------------------------------------------------------------------
    def read(self, processor):
        self.processQuery(
            processor, """
                SELECT eagilog.contemporary_streams(entry_time), *
                FROM eagilog.stream_entry
                ORDER BY entry_time
            """)
# ------------------------------------------------------------------------------
def watch(window, options):
    reader = DbReader(options)
    renderer = LogRenderer(window, options)
    while True:
        reader.tail(renderer, renderer.minEntriesPerScreen())
        i = options.watch_interval
        time.sleep(float(i) if i is not None else 1.0)
# ------------------------------------------------------------------------------
def screen_command(screen, options):
    curses.initscr()
    try:
       watch(screen, options)
    except KeyboardInterrupt:
        pass
    finally:
        return [screen.instr(l+2, 0).decode("utf-8") for l in range(curses.LINES-2)]
# ------------------------------------------------------------------------------
def main():
    options = getArgumentParser().parse_args()
    print(str("\n").join(curses.wrapper(screen_command, options)))
    return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
    sys.exit(main())


