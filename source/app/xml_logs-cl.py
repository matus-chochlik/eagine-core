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
import stat
import time
import json
import errno
import socket
import signal
import string
import base64
import xml.sax
import argparse
import textwrap
import threading

try:
    import selectors
except ImportError:
    import selectors2 as selectors

# ------------------------------------------------------------------------------
def permanentTranslations():
    return {
        "bool": {
            "type": int,
            "opts": {
                0: "False",
                1: "True"
            }
        },
        "DbgOutSrce": {
            "type": int,
            "opts": {
                0x8246: "API",
                0x8247: "window system",
                0x8248: "shader compiler",
                0x8249: "third party",
                0x824A: "application",
                0x824B: "other"
            }
        },
        "DbgOutType": {
            "type": int,
            "opts": {
                0x824C: "error",
                0x824D: "deprecated behavior",
                0x824E: "undefined  behavior",
                0x824F: "portability",
                0x8250: "performance",
                0x8251: "other"
            }
        },
        "DbgOutSvrt": {
            "type": int,
            "opts": {
                0x9146: "high",
                0x9147: "medium",
                0x9148: "low",
                0x826B: "notification"
            }
        }
    }
# ------------------------------------------------------------------------------
def formatRelTime(s):
    if s >= 60480000:
        return "%dw" % (int(s) / 604800)
    if s >= 604800:
        return "%2dw %2dd" % (int(s) / 604800, (int(s) % 604800) / 86400)
    if s >= 86400:
        return "%2dd %2dh" % (int(s) / 86400, (int(s) % 86400) / 3600)
    if s >= 3600:
        return "%2dh %02dm" % (int(s) / 3600, (int(s) % 3600) / 60)
    if s >= 60:
        return "%2dm %02ds" % (int(s) / 60, int(s) % 60)
    if s >= 10:
        return "%3ds" % int(s)
    if s >= 0.01:
        return "%4dms" % int(s*10**3)
    if s <= 0.0:
        return "0"
    return "%dμs" % int(s*10**6)

# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # -------------------------------------------------------------------------
    def _valid_msg_id(self, x):
        try:
            found = self._msg_re1.match(x)
            if found:
                return (None, found.group(1))
            found = self._msg_re2.match(x)
            if found:
                return (found.group(1), found.group(2))
            found = self._msg_re3.match(x)
            if found:
                return (found.group(1), None)
            assert False
        except:
            self.error("`%s' is not a valid message identifier" % str(x))

    # -------------------------------------------------------------------------
    def _positive_float(self, x):
        try:
            value = float(x)
            assert value > 0.0
            return value
        except:
            self.error("`%s' is not a valid positive number" % str(x))

    # -------------------------------------------------------------------------
    def _positive_int(self, x):
        try:
            assert(int(x) > 0)
            return int(x)
        except:
            self.error("`%s' is not a positive integer value" % str(x))

    # -------------------------------------------------------------------------
    def __init__(self, **kw):
        self._msg_re1 = re.compile("^([A-Za-z0-9_]{1,10})$")
        self._msg_re2 = re.compile("^([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10})$")
        self._msg_re3 = re.compile("^([A-Za-z0-9_]{1,10})\.$")

        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            dest='print_bash_completion',
            nargs='?',
            metavar='FILE',
            default=None
        )

        self.add_argument(
            "--local-socket", "-L",
            dest='local_socket',
            action="store",
            default="/tmp/eagine-xmllog"
        )

        self.add_argument(
            "--network-socket", "-n",
            dest='network_socket',
            action="store_true",
            default=False
        )

        self.add_argument(
            "--port", "-p",
            dest='socket_port',
            type=self._positive_int,
            action="store",
            default=34917
        )

        self.add_argument(
            "--output", "-o",
            metavar='OUTPUT-FILE',
            dest='output_path',
            nargs='?',
            type=os.path.realpath,
            default=None
        )

        self.add_argument(
            "--keep-running", "-k",
            dest="keep_running",
            action="store_true",
            default=False,
            help="""
            Keeps running even after all logging back-ends have disconnected.
            """
        )

        self.add_argument(
            "--allow", "-A",
            metavar='IDENTIFIER',
            dest='allow_list',
            nargs='+',
            action="append",
            type=self._valid_msg_id,
            default=[]
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
            "--interval", "-I",
            metavar='SECONDS',
            dest='interval_print_period',
            action="store",
            type=self._positive_float,
            default=60.0,
        )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, options):
        for cfg_path in ["/etc/eagine/xml_logs.json","~/.config/eagine/xml_logs.json"]:
            try:
                with open(os.path.expanduser(cfg_path), "r") as cfgfd:
                    cfg = json.load(cfgfd)
                    l = cfg.get("block_list")
                    if l is not None:
                        options.block_list.append([self._valid_msg_id(e) for e in l])
                    l = cfg.get("allow_list")
                    if l is not None:
                        options.allow_list.append([self._valid_msg_id(e) for e in l])
            except:
                pass

        options.block_list = [i for sl in options.block_list for i in sl]
        options.allow_list = [i for sl in options.allow_list for i in sl]

        if options.output_path is None:
            options.log_output = sys.stdout
        else:
            options.log_output = open(options.output_path, "wt")
        return options

    # -------------------------------------------------------------------------
    def parseArgs(self):
        return self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)
        )

# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Process formatting the XML log output from one or several
            EAGine logger backends.
        """
    )
# ------------------------------------------------------------------------------
keepRunning = True
# ------------------------------------------------------------------------------
class XmlLogFormatter(object):
    # --------------------------------------------------------------------------
    def _ttyEsc(self, escseq):
        if self._out.isatty():
            return escseq
        return ""

    # --------------------------------------------------------------------------
    def _ttyReset(self):
        return self._ttyEsc("\x1b[0m")

    # --------------------------------------------------------------------------
    def _ttyInvert(self):
        return self._ttyEsc("\x1b[7m")

    # --------------------------------------------------------------------------
    def _ttyGray(self):
        return self._ttyEsc("\x1b[0;30m")

    # --------------------------------------------------------------------------
    def _ttyRed(self):
        return self._ttyEsc("\x1b[0;31m")

    # --------------------------------------------------------------------------
    def _ttyGreen(self):
        return self._ttyEsc("\x1b[0;32m")

    # --------------------------------------------------------------------------
    def _ttyYellow(self):
        return self._ttyEsc("\x1b[0;33m")

    # --------------------------------------------------------------------------
    def _ttyBlue(self):
        return self._ttyEsc("\x1b[0;34m")

    # --------------------------------------------------------------------------
    def _ttyCyan(self):
        return self._ttyEsc("\x1b[0;36m")

    # --------------------------------------------------------------------------
    def _ttyWhite(self):
        return self._ttyEsc("\x1b[0;37m")

    # --------------------------------------------------------------------------
    def _ttyBoldGray(self):
        return self._ttyEsc("\x1b[0;37m")

    # --------------------------------------------------------------------------
    def _ttyBoldRed(self):
        return self._ttyEsc("\x1b[1;31m")

    # --------------------------------------------------------------------------
    def _ttyBoldGreen(self):
        return self._ttyEsc("\x1b[1;32m")

    # --------------------------------------------------------------------------
    def _ttyBoldYellow(self):
        return self._ttyEsc("\x1b[1;33m")

    # --------------------------------------------------------------------------
    def _ttyBoldBlue(self):
        return self._ttyEsc("\x1b[1;34m")

    # --------------------------------------------------------------------------
    def _ttyBoldCyan(self):
        return self._ttyEsc("\x1b[1;36m")

    # --------------------------------------------------------------------------
    def _ttyBoldWhite(self):
        return self._ttyEsc("\x1b[1;37m")

    # --------------------------------------------------------------------------
    def _formatAppName(self, n, src_id):
        try:
            parts = n.split("-")
            assert len(parts) > 1
            return self._ttyBoldBlue() + parts[0] + self._ttyReset() + "-" +\
                self._ttyBoldCyan() + "-".join(parts[1:]) + self._ttyReset()

        except:
            return self._ttyBoldCyan() + n + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatFsPath(self, p, src_id):
        try:
            if not os.path.isabs(p):
                p = os.path.normpath(os.path.join(self._work_dirs[src_id], p))
        except:
            pass
        if os.path.exists(p):
            if os.path.islink(p):
                return self._ttyBlue() + p + self._ttyReset()
            if os.path.isdir(p):
                return self._ttyGreen() + p + self._ttyReset()
            return self._ttyBoldGreen() + p + self._ttyReset()

        return self._ttyBoldRed() + p + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatHostname(self, p, src_id):
        return self._ttyBoldBlue() + p + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatURL(self, p, src_id):
        return self._ttyBoldBlue() + p + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatGitBranch(self, b, src_id):
        if b == "develop":
            return self._ttyBoldYellow() + b + self._ttyReset()
        if b == "main":
            return self._ttyBoldGreen() + b + self._ttyReset()

        return self._ttyBoldRed() + b + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatHash(self, p, src_id):
        return self._ttyBoldCyan() + p + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatIdentifier(self, p, src_id):
        return "\"" + self._ttyBoldBlue() + p + self._ttyReset() + "\""

    # --------------------------------------------------------------------------
    def _formatProgArg(self, p, src_id):
        return "«" + self._ttyBoldWhite() + p + self._ttyReset() + "»"

    # --------------------------------------------------------------------------
    def _formatRatio(self, x, src_id):
        return self._ttyBoldWhite() + ("%3.1f%%" % float(x * 100)) + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatRFC2822(self, dt, src_id):
        try:
            import email.utils
            dt = email.utils.format_datetime(email.utils.parsedate_to_datetime(dt))
        except:
            pass
        return self._ttyBoldWhite() + dt + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatDuration(self, sec, src_id):
        return self._ttyBoldWhite() + formatRelTime(float(sec)) + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatYesNoMaybe(self, v, src_id):
        if v == "yes":
            return self._ttyBoldGreen() + v + self._ttyReset()
        if v == "no":
            return self._ttyBoldRed() + v + self._ttyReset()
        return self._ttyYellow() + "maybe" + self._ttyReset()
    # --------------------------------------------------------------------------
    def _formatByteSize(self, n, src_id):
        result = None
        if n > 0:
            umult = ["GiB", "MiB", "kiB"]
            l = len(umult)
            for i in range(l):
                m = 1024**(l-i)
                if (n / m > 1024) or (n % m == 0):
                    result = str(int(n / m)) + " " + umult[i]
                    break
        if not result:
            result = str(n) + " B"

        return self._ttyBoldWhite() + result + self._ttyReset()

    # --------------------------------------------------------------------------
    def _formatInteger(self, s, src_id):
        return f"{int(s):,}".replace(",", "'")

    # --------------------------------------------------------------------------
    def _formatReal(self, s, src_id):
        return f"{float(s):,}".replace(",", "'")

    # --------------------------------------------------------------------------
    def _formatValueBar(self, mn, x, mx, width):
        try: coef = (x - mn) / (mx - mn)
        except ZeroDivisionError:
            coef = 0.0
        pos = coef * float(width)
        cnt = int(pos)

        i = 0
        result = "│"
        while i < cnt:
            result += "█"
            i += 1

        if i < width:
            parts = [" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"]
            result += parts[int(9 * (pos - float(cnt)))]
            i += 1

        while i < width:
            result += " "
            i += 1

        result += "│"

        return result

    # --------------------------------------------------------------------------
    def _formatProgress(self, src, inst, tag, mn, x, mx, width, progress):
        if src is not None and tag is not None:
            key = (src, inst, tag)
            try: progress_begin = self._progress_info[key]
            except KeyError:
                progress_begin = self._progress_info[key] = time.time()
            progress_seconds = time.time() - progress_begin

            if x >= mx:
                del self._progress_info[key]
        else:
            progress_seconds = None

        try: coef = (x - mn) / (mx - mn)
        except ZeroDivisionError:
            coef = 0.0
        pos = coef * float(width)
        cnt = int(pos)
        if progress_seconds is not None:
            if progress_seconds > 10.0 and coef > 0.0 and coef < 1.0:
                progress_eta = progress_seconds * (1.0 - coef) / coef
                if coef >= 0.5:
                    lbl = " (%s) %.1f%%" % (
                        formatRelTime(progress_eta),
                        100.0 * coef
                    )
                else:
                    lbl = " %.1f%% (%s)" % (
                        100.0 * coef,
                        formatRelTime(progress_eta)
                    )
            else:
                lbl = " %.1f%% (estimating)" % (100.0 * coef)
        else:
            lbl = " %.1f%%" % (100.0 * coef)

        i = 0
        result = "│"
        if progress:
            result += self._ttyInvert()

        if coef >= 0.5:
            while i + len(lbl) < cnt:
                result += "█"
                i += 1

            if i + len(lbl) <= cnt:
                if progress:
                    result += self._ttyReset()
                else:
                    result += self._ttyInvert()
                result += lbl
                i += len(lbl)
                if progress:
                    result += self._ttyInvert()
                else:
                    result += self._ttyReset()

        while i < cnt:
            result += "█"
            i += 1

        if i < width:
            parts = [" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"]
            result += parts[int(9 * (pos - float(cnt)))]
            i += 1

        if coef < 0.5:
            if i + len(lbl) < width:
                result += lbl
                i += len(lbl)

        while i < width:
            result += " "
            i += 1

        if progress:
            result += self._ttyReset()
        else:
            result += "│"

        return result

    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._start_time = time.time()
        self._src_times = dict()
        self._re_var = re.compile(".*(\${([A-Za-z][A-Za-z_0-9]*)}).*")
        self._lock = threading.Lock()
        self._out = options.log_output
        self._backend_count = 0
        self._progress_info = dict()

        self._translations = permanentTranslations()

        self._decorators = {
            "AppName": self._formatAppName,
            "FsPath": self._formatFsPath,
            "Hostname": self._formatHostname,
            "URL": self._formatURL,
            "Hash": self._formatHash,
            "GitBranch": self._formatGitBranch,
            "GitHash": self._formatHash,
            "Identifier": self._formatIdentifier,
            "ProgramArg": self._formatProgArg,
            "RFC2822": self._formatRFC2822,
            "integer": self._formatInteger,
            "int64": self._formatInteger,
            "int32": self._formatInteger,
            "int16": self._formatInteger,
            "uint64": self._formatInteger,
            "uint32": self._formatInteger,
            "uint16": self._formatInteger,
            "real": self._formatReal,
            "YesNo": self._formatYesNoMaybe,
            "YesNoMaybe": self._formatYesNoMaybe,
            "Ratio": lambda x, src_id: self._formatRatio(float(x), src_id),
            "duration": lambda x, src_id: self._formatDuration(float(x), src_id),
            "ByteSize": lambda x, src_id: self._formatByteSize(int(float(x)), src_id)
        }
        self._source_id = 0
        self._sources = []
        self._loggers = {}
        self._root_ids = {}
        self._work_dirs = {}
        self._prev_times = {}

        with self._lock:
            self._out.write("╮\n")
    # --------------------------------------------------------------------------
    def __del__(self):
        with self._lock:
            self._out.write("╯\n")
            self._out.close()

    # --------------------------------------------------------------------------
    def write(self, what):
        self._out.write(what)

    # --------------------------------------------------------------------------
    def beginLog(self, src_id, info):
        self._backend_count += 1
        self._src_times[src_id] = time.time()
        with self._lock:
            #
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write("   ╭────────────╮\n")
            #
            self.write("┝")
            for sid in self._sources:
                self.write("━━")
            self.write("━┯━┥")
            self.write(self._ttyGreen())
            self.write("starting log")
            self.write(self._ttyReset())
            self.write("│")
            self.write("\n")
            #
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" │ ╰────────────╯\n")
            self._sources.append(src_id)
            self._root_ids[src_id] = None
            self._work_dirs[src_id] = None
            self._prev_times[src_id] = None

    # --------------------------------------------------------------------------
    def finishLog(self, src_id, clean_shutdown):
        with self._lock:
            total_time = time.time() - self._src_times[src_id]
            del self._src_times[src_id]
            # L0
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ╭─────────┬──────────┬────────────┬─────────╮\n")
            # L1
            self.write("┊")
            conn = False
            for sid in self._sources:
                if sid == src_id:
                    conn = True
                    self.write(" ┕")
                elif conn:
                    self.write("━━")
                else:
                    self.write(" │")
            self.write("━┥")
            self.write("%9s│" % formatRelTime(float(total_time)))
            self.write("%10s│" % self._root_ids[src_id])
            self.write(self._ttyBlue())
            self.write(" closing log")
            self.write(self._ttyReset())
            self.write("│")
            if clean_shutdown:
                self.write(self._ttyGreen())
                self.write("  clean  ")
            else:
                self.write(self._ttyRed())
                self.write(" failed  ")
            self.write(self._ttyReset())
            self.write("│")
            self.write("\n")
            # L2
            self.write("┊")
            conn = False
            for sid in self._sources:
                if sid == src_id:
                    conn = True
                    self.write("  ")
                elif conn:
                    self.write("╭╯")
                else:
                    self.write(" │")
            self.write(" ╰─────────┴──────────┴────────────┴─────────╯\n")
            # L3
            self.write("┊")
            conn = False
            for sid in self._sources:
                if sid == src_id:
                    conn = True
                    self.write(" ")
                elif conn:
                    self.write("╭╯")
                else:
                    self.write(" │")
            self.write("\n")
            self._sources = [sid for sid in self._sources if sid != src_id]
            del self._root_ids[src_id]
            del self._prev_times[src_id]

        self._backend_count -= 1
        if self._backend_count < 1:
            if not self._options.keep_running:
                global keepRunning
                keepRunning = False

    # --------------------------------------------------------------------------
    def addInterval(self, src_id, interval, info):
        with self._lock:
            mn = interval.minDuration()
            av = interval.avgDuration()
            mx = interval.maxDuration()
            hc = interval.hitCount()

            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write("\n")

            self.write("┊")
            conn = False
            for sid in self._sources:
                if sid == src_id:
                    conn = True
                    self.write(" ┝")
                elif conn:
                    self.write("━━")
                else:
                    self.write(" │")
            self.write("━┑")
            self.write("%10s│" % self._root_ids[src_id])
            self.write("%10s│" % info.get("source", "N/A"))
            self.write("%10s│" % info["tag"])
            self.write("%12s│" % self.formatInstance(info["instance"]))
            self.write("\n")
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├──────────┴──────────┴──────────┴────────────╯")
            self.write("\n")
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├─min duration: %s\n" % formatRelTime(mn))
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├─avg duration: %s\n" % formatRelTime(av))
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├─max duration: %s\n" % formatRelTime(mx))
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├─linear distr:%s\n" % self._formatValueBar(
                mn, av, mx,
                70-len(self._sources)*2))
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ├─log 10 distr:%s\n" % self._formatValueBar(
                math.log10(mn),
                math.log10(av),
                math.log10(mx),
                70-len(self._sources)*2))
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write(" ╰╴hit count: %s\n" % self._formatInteger(hc, src_id))
            self._out.flush()

    # --------------------------------------------------------------------------
    def translateLevel(self, level):
        if level == "debug":
            return self._ttyBoldCyan()   + "  debug  " + self._ttyReset()
        if level == "info":
            return self._ttyBoldWhite()  + "  info   " + self._ttyReset()
        if level == "stat":
            return self._ttyBoldWhite()  + "  stat   " + self._ttyReset()
        if level == "change":
            return self._ttyBoldYellow() + " change  " + self._ttyReset()
        if level == "warning":
            return self._ttyYellow()     + " warning " + self._ttyReset()
        if level == "error":
            return self._ttyBoldRed()    + "  error  " + self._ttyReset()
        if level == "trace":
            return self._ttyCyan()       + "  trace  " + self._ttyReset()
        if level == "critical":
            return self._ttyRed()        + " critical" + self._ttyReset()
        if level == "backtrace":
            return self._ttyCyan()       + "backtrace" + self._ttyReset()
        return "%7s" % level

    # --------------------------------------------------------------------------
    def doTranslateArg(self, arg, info, src_id):
        if info.get("blob", False):
            return "BLOB"

        try:
            typ = info.get("type")
            value = info.get("value")
            try:
                trans = self._translations[typ]
                value = trans["opts"][trans["type"](value)]
            except: pass
            decorate = self._decorators.get(typ, lambda x, i: x)
            value = decorate(value, src_id)
        except KeyError:
            value = arg
        return value

    # --------------------------------------------------------------------------
    def alwaysTranslateAsList(self, values):
        for v in values:
            if v["type"] == "bitfield":
                return True
        return False

    # --------------------------------------------------------------------------
    def translateArg(self, arg, info, src_id):
        info["used"] = True
        values = info.get("values", [])
        if len(values) == 1 and not self.alwaysTranslateAsList(values):
            return self.doTranslateArg(arg, values[0], src_id)
        values = [self.doTranslateArg(arg, v, src_id) for v in values]
        return '[' + ", ".join(values) + ']'

    # --------------------------------------------------------------------------
    def formatInstance(self, instance):
        instance = hash(instance) % ((sys.maxsize + 1) * 2)
        instance = instance.to_bytes(8, byteorder='big')
        instance = base64.b64encode(instance, altchars=b"'-")
        instance = instance.decode("utf-8")
        return instance

    # --------------------------------------------------------------------------
    def isInList(self, msrc, mtag, lst):
        for lsrc, ltag in lst:
            src_match = lsrc is None or lsrc == msrc
            tag_match = ltag is None or ltag == mtag
            if src_match and tag_match:
                return True
        return False

    # --------------------------------------------------------------------------
    def isVisible(self, info):
        src = info.get("source", None)
        tag = info.get("tag", None)
        result = True
        if self._options.block_list:
            result = result and not self.isInList(src, tag, self._options.block_list)
        if self._options.allow_list:
            result = result and self.isInList(src, tag, self._options.allow_list)
        return result

    # --------------------------------------------------------------------------
    def previewMessage(self, src_id, info):
        if self._root_ids[src_id] is None:
            self._root_ids[src_id] = info["source"]
        if self._work_dirs[src_id] is None:
            try:
                info["tag"] == "pwd"
                self._work_dirs[src_id] = info["args"]["workDir"]["values"][0]["value"]
            except:
                pass
        return self.isVisible(info)

    # --------------------------------------------------------------------------
    def addMessage(self, src_id, info):
        args = info["args"]
        message = info["format"]
        tag = info.get("tag")
        source = info.get("source")

        curr_time = time.time()
        if self._prev_times[src_id] is None:
            time_diff = None
        else:
            time_diff = curr_time - self._prev_times[src_id]
        self._prev_times[src_id] = curr_time

        found = re.match(self._re_var, message)
        while found:
            prev = message[:found.start(1)]
            repl = self.translateArg(
                found.group(2),
                args.get(found.group(2), {}),
                src_id
            )
            folw = message[found.end(1):]
            message = prev + repl + folw
            found = re.match(self._re_var, message)

        with self._lock:
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            self.write("\n")

            self.write("┊")
            conn = False
            instance = info["instance"]
            for sid in self._sources:
                if sid == src_id:
                    conn = True
                    self.write(" ┝")
                elif conn:
                    self.write("━━")
                else:
                    self.write(" │")
            self.write("━┑")
            self.write("%9s│" % formatRelTime(float(info["timestamp"])))
            self.write("%9s│" % (formatRelTime(time_diff) if time_diff is not None else "   N/A   "))
            self.write("%s│" % self.translateLevel(info["level"]))
            self.write("%10s│" % self._root_ids[src_id])
            self.write("%10s│" % source)
            if tag is not None:
                self.write("%10s│" % tag)
            self.write("%12s│" % self.formatInstance(instance))
            self.write("\n")
            self.write("┊")
            for sid in self._sources:
                self.write(" │")
            if tag is not None:
                self.write(" ├─────────┴─────────┴─────────┴──────────┴──────────┴──────────┴────────────╯")
            else:
                self.write(" ├─────────┴─────────┴─────────┴──────────┴──────────┴────────────╯")
            self.write("\n")

            cols = 80 - (len(self._sources) * 2)
            lno = 0
            for line in textwrap.wrap(message, cols):
                self.write("┊")
                for sid in self._sources:
                    self.write(" │")
                if lno == 0:
                    self.write(" ╰╴")
                else:
                    self.write("   ")
                lno += 1
                self.write(line)
                self.write("\n")
            # BLOBs and progress
            for name, info in args.items():
                if not info["used"]:
                    for value in info["values"]:
                        self.write("┊")
                        for sid in self._sources:
                            self.write(" │")
                        self.write("   ")
                        self.write(name)
                        self.write(": ")
                        if value["min"] is not None and value["max"] is not None:
                            self.write(
                                self._formatProgress(
                                    source,
                                    instance,
                                    tag,
                                    float(value["min"]),
                                    float(value["value"]),
                                    float(value["max"]),
                                    cols - len(name) - 2,
                                    value["type"] in ["Progress", "MainPrgrss"]
                                )
                            )
                            self.write("\n")
                        else:
                            self.write(self.doTranslateArg(name, value, src_id))
                            self.write("\n")
                        if value["blob"]:
                            blob = value["value"]
                            if len(blob) % 4 != 0:
                                blob += '=' * (4 - len(blob) % 4)
                            blob = base64.standard_b64decode(blob)
                            while blob:
                                self.write("┊")
                                for sid in self._sources:
                                    self.write(" │")
                                self.write("  ")
                                for i in range(16):
                                    if i == 8:
                                        self.write(" ")
                                    try:
                                        self.write(" %02x" % blob[i])
                                    except IndexError:
                                        self.write(" ..")
                                self.write(" │ ")

                                for i in range(16):
                                    if i == 8:
                                        self.write(" ")
                                    try:
                                        c = bytes([blob[i]]).decode('ascii')
                                        assert c.isprintable()
                                        self.write(c)
                                    except:
                                        self.write(".")
                                self.write("\n")
                                blob = blob[16:]

            #
            self._out.flush()

    # --------------------------------------------------------------------------
    def addLoggerInfos(self, src_id, infos):
        self._loggers[src_id] = infos

    # --------------------------------------------------------------------------
    def makeProcessor(self):
        self._source_id += 1
        return XmlLogProcessor(self._source_id, self)

# ------------------------------------------------------------------------------
class TimeIntervalInfo:
    # --------------------------------------------------------------------------
    def __init__(self, options, time_ns):
        self._print_interval = options.interval_print_period
        self._print_time = time.time()
        self._count = 1
        self._time_min_ns = time_ns
        self._time_max_ns = time_ns
        self._time_sum_ns = time_ns

    # --------------------------------------------------------------------------
    def update(self, time_ns):
        self._count += 1
        self._time_min_ns = min(self._time_min_ns, time_ns)
        self._time_max_ns = max(self._time_max_ns, time_ns)
        self._time_sum_ns += time_ns
        return self

    # --------------------------------------------------------------------------
    def shouldPrint(self):
        return (self._print_time + self._print_interval) < time.time()

    # --------------------------------------------------------------------------
    def wasPrinted(self):
        self._print_time = time.time()

    # --------------------------------------------------------------------------
    def minDuration(self):
        return self._time_min_ns / 1000000000.0

    # --------------------------------------------------------------------------
    def maxDuration(self):
        return self._time_max_ns / 1000000000.0

    # --------------------------------------------------------------------------
    def avgDuration(self):
        return self._time_sum_ns / (self._count * 1000000000.0)

    # --------------------------------------------------------------------------
    def hitCount(self):
        return self._count

# ------------------------------------------------------------------------------
class XmlLogProcessor(xml.sax.ContentHandler):
    # --------------------------------------------------------------------------
    def __init__(self, src_id, formatter):
        self._src_id = src_id
        self._clean_shutdown = False
        self._ctag = None
        self._carg = None
        self._info = None
        self._loggers = {}
        self._intervals = {}
        self._source_map = {}
        self._start_time = time.time()
        self._formatter = formatter
        self._options = formatter._options
        self._parser = xml.sax.make_parser()
        self._parser.setContentHandler(self)

    # --------------------------------------------------------------------------
    def startElement(self, tag, attr):
        time_ofs = self._start_time - self._formatter._start_time
        self._ctag = tag
        if tag == "log":
            self._formatter.beginLog(self._src_id, attr)
        elif tag == "m":
            self._info = {
                r: attr.get(k, None) for k, r in [
                    ("lvl", "level"),
                    ("src", "source"),
                    ("tag", "tag"),
                    ("iid", "instance"),
                    ("ts",  "timestamp")
                ]
            }
            self._info["args"] = {}
            self._source_map[attr["iid"]] = attr["src"]
        elif tag == "a":
            self._carg = attr["n"]
            iarg = {
                "value": "''",
                "min": attr.get("min"),
                "max": attr.get("max"),
                "type": attr["t"],
                "blob": attr.get("blob", False)
            }
            try: self._info["args"][self._carg]["values"].append(iarg)
            except KeyError:
                self._info["args"][self._carg] = {
                    "used": False,
                    "values": [iarg]
                }
        elif tag == "i":
            key = (attr["iid"], attr["lbl"])
            try: interval = self._intervals[key].update(int(attr.get("tns")))
            except KeyError:
                interval = self._intervals[key] = TimeIntervalInfo(
                    self._options,
                    int(attr.get("tns")))
            if interval.shouldPrint():
                info = {
                    "source": self._source_map.get(attr.get("iid")),
                    "instance": attr.get("iid"),
                    "tag": attr.get("lbl")
                }
                if self._formatter.isVisible(info):
                    self._formatter.addInterval(self._src_id, interval, info)
                interval.wasPrinted()
        elif tag == "c":
            try: logger = self._loggers[attr["src"]]
            except KeyError:
                logger = self._loggers[attr["src"]] = {}
            try: inst = logger[attr["iid"]]
            except KeyError:
                inst = logger[attr["iid"]] = {}
            try: charts = inst["charts"]
            except KeyError:
                charts = inst["charts"] = {}
            try: series = charts[attr["ser"]]
            except KeyError:
                series = charts[attr["ser"]] = []
            series.append((time_ofs+float(attr["ts"]), float(attr["v"])))
        elif tag == "d":
            try: logger = self._loggers[attr["src"]]
            except KeyError:
                logger = self._loggers[attr["src"]] = {}
            try: inst = logger[attr["iid"]]
            except KeyError:
                inst = logger[attr["iid"]] = {}
            inst["display_name"] = attr["dn"]
            inst["description"] = attr["desc"]

    # --------------------------------------------------------------------------
    def endElement(self, tag):
        if tag == "log":
            self._clean_shutdown = True
        elif tag == "m":
            if self._formatter.previewMessage(self._src_id, self._info):
                self._formatter.addMessage(self._src_id, self._info)
            self._info = None

    # --------------------------------------------------------------------------
    def characters(self, content):
        if self._info:
            if self._ctag == "f":
                self._info["format"] = content
            elif self._ctag == "a":
                self._info["args"][self._carg]["values"][-1]["value"] = content

    # --------------------------------------------------------------------------
    def processLine(self, line):
        self._parser.feed(line)

    # --------------------------------------------------------------------------
    def processDisconnect(self):
        for key, interval in self._intervals.items():
            iarg = {
                "source": self._source_map.get(key[0]),
                "instance": key[0],
                "tag": key[1]
            }
            if self._formatter.isVisible(iarg):
                self._formatter.addInterval(self._src_id, interval, iarg)
        self._formatter.addLoggerInfos(self._src_id, self._loggers)
        self._formatter.finishLog(self._src_id, self._clean_shutdown)

# ------------------------------------------------------------------------------
class XmlLogClientHandler(xml.sax.ContentHandler):

    # --------------------------------------------------------------------------
    def __init__(self, processor, connection, selector):
        self._processor = processor
        self._connection = connection
        self._selector = selector
        self._buffer = bytes()

    # --------------------------------------------------------------------------
    def __del__(self):
        self._connection.close()

    # --------------------------------------------------------------------------
    def disconnect(self):
        self._processor.processLine(self._buffer)
        self._processor.processDisconnect()
        self._selector.unregister(self._connection)

    # --------------------------------------------------------------------------
    def handleRead(self):
        try:
            data = self._connection.recv(4096)
            if data:
                sep = bytes('\n','utf-8')
                self._buffer += data
                lines = self._buffer.split(sep)
                done = 0
                for line in lines[:-1]:
                    try:
                        self._processor.processLine(line.decode('utf-8'))
                        done += 1
                    except UnicodeDecodeError:
                        print(line)
                        break
                self._buffer = sep.join(lines[done:])
            else:
                self.disconnect()
        except socket.error:
            self.disconnect()

# ------------------------------------------------------------------------------
class LocalLogSocket(socket.socket):
    # --------------------------------------------------------------------------
    def __init__(self, socket_path):
        self._socket_path = socket_path
        try: os.unlink(self._socket_path)
        except OSError as os_error:
            if os_error.errno != errno.ENOENT:
                raise
        socket.socket.__init__(self, socket.AF_UNIX, socket.SOCK_STREAM)
        self.bind(self._socket_path)
        self.listen(50)
        self.setblocking(False)

    # --------------------------------------------------------------------------
    def __del__(self):
        try:
            self.close()
            os.unlink(self._socket_path)
        except: pass

# ------------------------------------------------------------------------------
class NetworkLogSocket(socket.socket):
    # --------------------------------------------------------------------------
    def __init__(self, socket_port):
        socket.socket.__init__(self, socket.AF_INET, socket.SOCK_STREAM)
        self.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bind(('', socket_port))
        self.listen(50)
        self.setblocking(False)

    # --------------------------------------------------------------------------
    def __del__(self):
        try:
            self.close()
        except: pass

# ------------------------------------------------------------------------------
def open_socket(options):
    if options.network_socket:
        return NetworkLogSocket(options.socket_port)
    return LocalLogSocket(options.local_socket)

# ------------------------------------------------------------------------------
def handle_connections(log_sock, formatter):
    global keepRunning
    with selectors.DefaultSelector() as selector:
        selector.register(
            log_sock,
            selectors.EVENT_READ,
            data = formatter
        )

        while keepRunning:
            events = selector.select(timeout=1.0)
            for key, mask in events:
                if type(key.data) is XmlLogFormatter:
                    connection, addr = log_sock.accept()
                    selector.register(
                        connection,
                        selectors.EVENT_READ,
                        data = XmlLogClientHandler(
                            key.data.makeProcessor(),
                            connection,
                            selector
                        )
                    )
                elif type(key.data) is XmlLogClientHandler:
                    if mask & selectors.EVENT_READ:
                        key.data.handleRead()

# ------------------------------------------------------------------------------
def handleInterrupt(sig, frame):
    global keepRunning
    keepRunning = False
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_xml_logs_cl",
            "eagine-xml-logs-cl",
            ["--print-bash-completion"],
            fd)
    if options.print_bash_completion == "-":
        _printIt(sys.stdout)
    else:
        with open(options.print_bash_completion, "wt") as fd:
            _printIt(fd)

# ------------------------------------------------------------------------------
def main():
    try:
        argparser = getArgumentParser()
        options = argparser.parseArgs()
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
        else:
            signal.signal(signal.SIGINT, handleInterrupt)
            signal.signal(signal.SIGTERM, handleInterrupt)
            formatter = XmlLogFormatter(options)
            handle_connections(open_socket(options), formatter)
    except KeyboardInterrupt:
        return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
        sys.exit(main())

