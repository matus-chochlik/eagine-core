#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
import os
import re
import sys
import math
import numpy
import getpass
import argparse
import colorsys
import psycopg2
import matplotlib.pyplot as plt
import matplotlib.ticker as pltckr

# ------------------------------------------------------------------------------
# arguments
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # -------------------------------------------------------------------------
    def _valid_msg_id(self, x):
        try:
            found = self._msg_re1.match(x)
            if found:
                return (None, None, found.group(1), x)
            found = self._msg_re2.match(x)
            if found:
                return (None, found.group(1), None, x)
            found = self._msg_re3.match(x)
            if found:
                return (None, found.group(1), found.group(2), x)
            found = self._msg_re4.match(x)
            if found:
                return (found.group(1), found.group(2), found.group(3), x)
            assert False
        except:
            self.error("`%s' is not a valid message identifier" % str(x))

    # -------------------------------------------------------------------------
    def _positive_int(self, x):
        try:
            assert(int(x) > 0)
            return int(x)
        except:
            self.error("`%s' is not a positive integer value" % str(x))

    # -------------------------------------------------------------------------
    def __init__(self, **kw):
        self._msg_re1 = re.compile(r"^([A-Za-z0-9_]{1,10})$")
        self._msg_re2 = re.compile(r"^([A-Za-z0-9_]{1,10})\.$")
        self._msg_re3 = re.compile(r"^([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10})$")
        self._msg_re4 = re.compile(r"^([A-Za-z0-9_]{1,10})\.([A-Za-z0-9_]{1,10}).([A-Za-z0-9_]{1,10})$")

        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None
        )

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
            default='eagilog'
        )

        self.add_argument(
            "--db-user", "-u",
            dest='db_user',
            action="store",
            default='eagilog'
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
            "--list", "-l",
            dest='list_name',
            action="store",
            choices=[
                "numeric_streams",
                "stream_profiles"],
            default=None
        )

        self.add_argument(
            "--plot", "-p",
            dest='plot_name',
            action="store",
            choices=[
                "numeric_streams",
                "stream_profiles"],
            default="numeric_stream"
        )

        self.add_argument(
            "--stream-id", "-S",
            metavar='INTEGER',
            dest='stream_ids',
            nargs='+',
            action="append",
            type=self._positive_int,
            default=[]
        )

        self.add_argument(
            "--identifier", "-I",
            metavar='ID.ID.ID',
            dest='identifiers',
            nargs='+',
            action="append",
            type=self._valid_msg_id,
            default=[]
        )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, options):
        if options.db_user is None:
            options.db_user = os.getlogin()

        if options.db_password is None and not options.print_bash_completion:
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

        options.stream_ids = [i for sl in options.stream_ids for i in sl]
        options.identifiers = [i for sl in options.identifiers for i in sl]

        return options

    # -------------------------------------------------------------------------
    def parseArgs(self):
        # ----------------------------------------------------------------------
        class _Options(object):
            # ------------------------------------------------------------------
            def __init__(self, base):
                self.__dict__.update(base.__dict__)
            # ------------------------------------------------------------------
            def query_data(self, query_format, query_params):
                with psycopg2.connect(
                    host=self.db_host,
                    database=self.db_name,
                    user=self.db_user,
                    password=self.db_password) as pg_conn:
                    with pg_conn.cursor() as cursor:
                        cursor.execute(query_format, query_params)
                        while True:
                            row = cursor.fetchone()
                            if row is None:
                                break
                            yield row
            # ------------------------------------------------------------------
            def plot_color(self, i, n):
                try:
                    b = float(i) / float(n - 1)
                except ZeroDivisionError:
                    b = 1.0
                a = 1.0 - b
                h1,s1,v1 = colorsys.rgb_to_hsv(2/255, 179/255, 249/255)
                h2,s2,v2 = colorsys.rgb_to_hsv(249/255, 179/255, 2/255)
                return colorsys.hsv_to_rgb(h1*a+h2*b, s1*a+s2*b, v1*a+v2*b)

            # ------------------------------------------------------------------
            def brighter_color(self, rgb):
                r, g, b = rgb
                h, s, v = colorsys.rgb_to_hsv(r, g, b)
                return colorsys.hsv_to_rgb(h, s, min(v*1.212, 1.0))

            # ------------------------------------------------------------------
            def darker_color(self, rgb):
                r, g, b = rgb
                h, s, v = colorsys.rgb_to_hsv(r, g, b)
                return colorsys.hsv_to_rgb(h, s, min(v*0.707, 1.0))

            # ------------------------------------------------------------------
            def init_plot(self, nrows, ncols):
                mult = 1.0
                plt.style.use('dark_background')
                fig, spl = plt.subplots(nrows, ncols)
                fig.set_size_inches(16.0*mult, 9.0*mult)
                return fig, spl
            # ------------------------------------------------------------------
            def finish_plot(self, fig):
                plt.show()
            # ------------------------------------------------------------------

        return _Options(self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)
        ))

# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Process storing the XML log output from one or several
            EAGine logger backends into a PostgreSQL database.
        """
    )
# ------------------------------------------------------------------------------
# plot functions
# ------------------------------------------------------------------------------
def formatRelTime(s, pos=None):
    if s >= 60480000:
        return "%dw" % (int(s) / 604800)
    if s >= 604800:
        m = int(s) % 604800
        if m == 0:
            return "%2dw" % (int(s) / 604800)
        else:
            return "%2dw %2dd" % (int(s) / 604800, m / 86400)
    if s >= 86400:
        m = int(s) % 86400
        if m == 0:
            return "%2dd" % (int(s) / 86400)
        else:
            return "%2dd %2dh" % (int(s) / 86400, m / 3600)
    if s >= 3600:
        m = int(s) % 3600
        if m == 0:
            return "%2dh" % (int(s) / 3600)
        else:
            return "%2dh %02dm" % (int(s) / 3600, m / 60)
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
def plotInitInterval():
    return (None, None)
# ------------------------------------------------------------------------------
def plotUpdateInterval(interval, ts):
    min_t, max_t = interval
    min_t = min(ts) if min_t is None else min(min_t, min(ts))
    max_t = max(ts) if max_t is None else max(max_t, max(ts))
    return (min_t, max_t)
# ------------------------------------------------------------------------------
def plotLocatorReltime(interval):
    min_t, max_t = interval
    if min_t is None: min_t = 0.0
    if max_t is None: max_t = 1.0
    interval = max_t - min_t
    tick_opts = [1,5,10,15,30,60,300,900,1800,3600,7200,10800,21600,43200,86400,7*86400]
    tick_maj = tick_opts[0]

    def _found():
        return interval / tick_maj < 12

    for t in tick_opts:
        tick_maj = t*60
        if _found():
            break
    while not _found():
        tick_maj *= 2

    return tick_maj
# ------------------------------------------------------------------------------
# numeric stream
# ------------------------------------------------------------------------------
def list_numeric_streams(options):
    query = """
        SELECT DISTINCT value_id
        FROM eagilog.numeric_streams
        ORDER BY value_id
    """
    for row in options.query_data(query, ()):
        print(row[0])
# ------------------------------------------------------------------------------
def numeric_stream_streams(options):
    stream_ids = {}
    plotnum = 0

    for app_id, source_id, tag, full_id in options.identifiers:
        if len(options.stream_ids) == 0:
            stream_id_query = """
                SELECT DISTINCT stream_id
                FROM eagilog.numeric_streams
                WHERE value_id = %s
                ORDER BY stream_id
            """
            stream_ids[full_id] =\
                [row[0] for row in options.query_data(stream_id_query, full_id)]
            plotnum += len(stream_ids[full_id])
        else:
            stream_ids[full_id] = options.stream_ids
            plotnum += len(options.stream_ids)

    return stream_ids, plotnum
# ------------------------------------------------------------------------------
def numeric_stream_data(options, value_id, stream_id):
    query = """
        SELECT
            stream_id,
            entry_time,
            value
        FROM eagilog.numeric_streams
        WHERE stream_id = %s
        AND value_id = %s
        ORDER BY value_time
    """

    return numpy.fromiter((
        [row[1].total_seconds(), row[2]]
            for row in options.query_data(query, (stream_id, value_id))),
                numpy.dtype((float, 2))).transpose()

# ------------------------------------------------------------------------------
def plot_numeric_streams(options):
    fig, spl = options.init_plot(1, 1)

    spl.set_xlabel("time")
    spl.set_ylabel("value")
    spl.grid(axis="y", alpha=0.25)

    stream_ids, plotnum = numeric_stream_streams(options)
    plotidx = 0
    interval = plotInitInterval()

    for app_id, source_id, tag, full_id in options.identifiers:
        for stream_id in stream_ids[full_id]:
            t, val = numeric_stream_data(options, full_id, stream_id)
            interval = plotUpdateInterval(interval, t)
            rgb = options.plot_color(plotidx, plotnum)
            plotidx += 1

            val_lbl = "%s/%d" % (full_id, stream_id)
            spl.plot(t, val, label=val_lbl, color=rgb)

    spl.xaxis.set_major_locator(pltckr.MultipleLocator(plotLocatorReltime(interval)))
    spl.xaxis.set_major_formatter(pltckr.FuncFormatter(formatRelTime))
    spl.legend()
    fig.tight_layout()
    options.finish_plot(fig)

# ------------------------------------------------------------------------------
# stream profile
# ------------------------------------------------------------------------------
def list_stream_profiles(options):
    query = """
        SELECT DISTINCT source_id || '.' || tag
        FROM eagilog.profile_interval
        ORDER BY source_id || '.' || tag
    """
    for row in options.query_data(query, ()):
        print(row[0])
# ------------------------------------------------------------------------------
def stream_profile_streams(options):
    stream_ids = {}
    plotnum = 0

    for app_id, source_id, tag, full_id in options.identifiers:
        interval_id = (source_id, tag)
        if len(options.stream_ids) == 0:
            stream_id_query = """
                SELECT DISTINCT stream_id
                FROM eagilog.profile_interval
                WHERE source_id = coalesce(%s, source_id)
                AND tag = %s
                ORDER BY stream_id
            """
            stream_ids[interval_id] =\
                [row[0] for row in options.query_data(stream_id_query, interval_id)]
            plotnum += len(stream_ids[interval_id])
        else:
            stream_ids[interval_id] = options.stream_ids
            plotnum += len(options.stream_ids)

    return stream_ids, plotnum

# ------------------------------------------------------------------------------
def stream_profile_data(options, source_id, tag, stream_id):
    query = """
        SELECT
            stream_id,
            source_id,
            tag,
            entry_time,
            min_duration_ms,
            avg_duration_ms,
            max_duration_ms
        FROM eagilog.profile_interval
        WHERE source_id = coalesce(%s, source_id)
        AND tag = %s
        AND stream_id = %s
        ORDER BY entry_time
    """

    return numpy.fromiter((
        [row[3].total_seconds(), row[4], row[5], row[6]]
            for row in options.query_data(query, (source_id, tag, stream_id))),
                numpy.dtype((float, 4))).transpose()

# ------------------------------------------------------------------------------
def plot_stream_profiles(options):
    fig, spl = options.init_plot(2, 1)

    spl[0].set_xlabel("execution time [s]")
    spl[0].set_ylabel("average\ninterval [ms]")
    spl[0].grid(axis="y", alpha=0.25)
    spl[1].set_xlabel("execution time [s]")
    spl[1].set_ylabel("min/avg/max\nintervals [ms]")
    spl[1].grid(axis="y", alpha=0.25)

    interval_count = len(options.identifiers)

    stream_ids, plotnum = stream_profile_streams(options)
    plotidx = 0

    interval = plotInitInterval()

    for app_id, source_id, tag, full_id in options.identifiers:
        interval_id = (source_id, tag)
        for stream_id in stream_ids[interval_id]:
            t, min_ms, avg_ms, max_ms = \
                stream_profile_data(options, source_id, tag, stream_id)
            interval = plotUpdateInterval(interval, t)
            rgb = options.plot_color(plotidx, plotnum)
            plotidx += 1

            avg_lbl = "%s/%d" % (full_id, stream_id)

            spl[0].plot(t, avg_ms, label=avg_lbl, color=rgb)

            spl[1].plot(t, min_ms, color=options.darker_color(rgb))
            spl[1].plot(t, max_ms, color=rgb)
            spl[1].plot(t, avg_ms, color=options.brighter_color(rgb))

    spl[0].xaxis.set_major_locator(pltckr.MultipleLocator(plotLocatorReltime(interval)))
    spl[0].xaxis.set_major_formatter(pltckr.FuncFormatter(formatRelTime))
    spl[0].legend()
    spl[1].xaxis.set_major_locator(pltckr.MultipleLocator(plotLocatorReltime(interval)))
    spl[1].xaxis.set_major_formatter(pltckr.FuncFormatter(formatRelTime))
    fig.tight_layout()
    options.finish_plot(fig)

# ------------------------------------------------------------------------------
# list dispatch
# ------------------------------------------------------------------------------
def print_list(options):
    if options.list_name == "numeric_streams":
        list_numeric_streams(options)
    elif options.list_name == "stream_profiles":
        list_stream_profiles(options)
# ------------------------------------------------------------------------------
# plot dispatch
# ------------------------------------------------------------------------------
def draw_plot(options):
    if options.plot_name == "numeric_streams":
        plot_numeric_streams(options)
    elif options.plot_name == "stream_profiles":
        plot_stream_profiles(options)
# ------------------------------------------------------------------------------
#  bash completion
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagilog_plot",
            "eagilog-plot",
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
    try:
        argparser = getArgumentParser()
        options = argparser.parseArgs()
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
            return 0
        else:
            if options.list_name is not None:
                print_list(options)
            else:
                draw_plot(options)
    except KeyboardInterrupt:
        pass
    return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
        sys.exit(main())
# ------------------------------------------------------------------------------
exit

