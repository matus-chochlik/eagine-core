#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
import os
import sys
import base64
import argparse
import datetime
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
    # --------------------------------------------------------------------------
    def _ttyEsc(self, escseq):
        if self._out.isatty():
            return escseq
        return ""

    # -------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._out = sys.stdout

    # -------------------------------------------------------------------------
    def columns(self):
        return 80

    # -------------------------------------------------------------------------
    def write(self, s):
        self._out.write(s)

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
    def formatDuration(self, s, w = 0):
        c = lambda t: self.centerText(t, w) if w > 0 else lambda t: t
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

    # --------------------------------------------------------------------------
    def formatLogSeverity(self, level):
        if level == "debug":
            return self.ttyBoldCyan()   + "  debug  " + self.ttyReset()
        if level == "info":
            return self.ttyBoldWhite()  + "  info   " + self.ttyReset()
        if level == "stat":
            return self.ttyBoldWhite()  + "statistic" + self.ttyReset()
        if level == "change":
            return self.ttyBoldYellow() + " change  " + self.ttyReset()
        if level == "warning":
            return self.ttyYellow()     + " warning " + self.ttyReset()
        if level == "error":
            return self.ttyBoldRed()    + "  error  " + self.ttyReset()
        if level == "trace":
            return self.ttyCyan()       + "  trace  " + self.ttyReset()
        if level == "critical":
            return self.ttyRed()        + " critical" + self.ttyReset()
        if level == "backtrace":
            return self.ttyCyan()       + "backtrace" + self.ttyReset()
        return "%7s" % level

    # --------------------------------------------------------------------------
    def formatInstance(self, instance):
        instance = hash(instance) % ((sys.maxsize + 1) * 2)
        instance = instance.to_bytes(8, byteorder='big')
        instance = base64.b64encode(instance, altchars=b"'-")
        instance = instance.decode("utf-8")
        return instance

# ------------------------------------------------------------------------------
# Log entry formatter
# ------------------------------------------------------------------------------
class LogFormatter(object):
    # -------------------------------------------------------------------------
    def __init__(self, options):
        self._utils = LogFormattingUtils(options)

    # -------------------------------------------------------------------------
    def write(self, s):
        self._utils.write(s)

    # -------------------------------------------------------------------------
    def formatMessage(self, data):
        return data["format"]

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
    def renderEntryConnectorsSwitch(self, data):
        pass

    # -------------------------------------------------------------------------
    def renderEntry(self, data):
        ls = len(data["streams"])
        tag = data["tag"]
        time_diff = None

        self.renderEntryConnectorsHead(data)
        self.write("%s│" % self._utils.formatDuration(data["time_since_start"], 9))
        self.write("%s│" % self._utils.formatDuration(time_diff, 9))
        self.write("%s│" % self._utils.formatLogSeverity(data["severity"]))
        self.write("%10s│" % data["application_id"])
        self.write("%10s│" % data["source_id"])
        if tag is not None:
            self.write("%10s│" % tag)
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
        self.write("  starting  ")
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
        self.write("  finished  ")
        self.write("│")
        self.write("% 10s" % data["application_id"])
        self.write("│\n")
        self.renderEntryConnectorsPass(ls-1)
        self.write("   ╰────────────┴──────────╯\n")

    # -------------------------------------------------------------------------
    def processBegin(self):
        self._prev_streams = []

    # -------------------------------------------------------------------------
    def processEntry(self, data):
        self.renderEntryConnectorsSwitch(data)

        if data["is_first"]:
            self.renderStreamHead(data)

        self.renderEntry(data)

        if data["is_last"]:
            self.renderStreamTail(data)

        self._prev_streams = data["streams"]

    # -------------------------------------------------------------------------
    def processEnd(self):
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
    def processEntry(self, processor, data):
        return processor.processEntry(data)

    # -------------------------------------------------------------------------
    def processEntries(self, processor, cursor):
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
        while True:
            data = cursor.fetchone()
            if data is None:
                break
            data = {k:v for k,v in zip(columns, data)}
            self.processEntry(processor, data)
        processor.processEnd()

    # -------------------------------------------------------------------------
    def read(self, processor):
        pg_conn = self._pg_conn
        with pg_conn:
            with pg_conn.cursor() as cursor:
                cursor.execute("""
                    SELECT eagilog.contemporary_streams(entry_time), *
                    FROM eagilog.stream_entry
                    ORDER BY entry_time
                """)
                self.processEntries(processor, cursor)
# ------------------------------------------------------------------------------
def bla(options):
    reader = DbReader(options)
    reader.read(LogFormatter(options))
# ------------------------------------------------------------------------------
def main():
    try:
        options = getArgumentParser().parse_args()
        bla(options)
    except KeyboardInterrupt:
        return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
        sys.exit(main())


