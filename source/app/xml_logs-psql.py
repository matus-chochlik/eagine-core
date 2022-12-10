#!/usr/bin/env python3
# coding=utf-8
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
import os
import sys
import json
import errno
import socket
import signal
import getpass
import xml.sax
import argparse
import psycopg2

try:
    import selectors
except ImportError:
    import selectors2 as selectors

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
            "--port", "-p",
            dest='socket_port',
            type=self._positive_int,
            action="store",
            default=34917
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
            default=None
        )

        self.add_argument(
            "--db-user", "-u",
            dest='db_user',
            action="store",
            default=None
        )

        self.add_argument(
            "--db-password", "-w",
            dest='db_password',
            action="store",
            default=None
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

        if options.db_name is None:
            options.db_name = os.getlogin()

        if options.db_user is None:
            options.db_user = os.getlogin()

        if options.db_password is None:
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
            Process storing the XML log output from one or several
            EAGine logger backends into a PostgreSQL database.
        """
    )
# ------------------------------------------------------------------------------
keepRunning = True
# ------------------------------------------------------------------------------
class XmlLogDbWriter(object):
    # --------------------------------------------------------------------------
    def __init__(self, options):
        self._options = options
        self._source_id = 0

    # --------------------------------------------------------------------------
    def startStream(self, pg_conn):
        with pg_conn:
            with pg_conn.cursor() as cursor:
                cursor.execute("SELECT eagilog.start_stream()")
                data = cursor.fetchone()
                return int(data[0])

    # --------------------------------------------------------------------------
    def finishStream(self, pg_conn, streamid, clean_shutdown):
        with pg_conn:
            with pg_conn.cursor() as cursor:
                cursor.execute("SELECT eagilog.finish_stream(%s)", (streamid,))

    # --------------------------------------------------------------------------
    def makeProcessor(self):
        self._source_id += 1
        return XmlLogProcessor(self._source_id, self, self._options)

# ------------------------------------------------------------------------------
class XmlLogProcessor(xml.sax.ContentHandler):
    # --------------------------------------------------------------------------
    def __init__(self, srcid, db_writer, options):
        self._srcid = srcid
        self._streamid = None
        self._clean_shutdown = False
        self._db_writer = db_writer
        self._pg_conn = None
        self._pg_conn = psycopg2.connect(
            user=options.db_user,
            password=options.db_password,
            host=options.db_host,
            port=options.db_port
        )
        self._parser = xml.sax.make_parser()
        self._parser.setContentHandler(self)

    # --------------------------------------------------------------------------
    def __del__(self):
        if self._pg_conn:
            self._pg_conn.close()

    # --------------------------------------------------------------------------
    def startElement(self, tag, attr):
        if tag == "log":
            if self._pg_conn is not None:
                self._streamid = self._db_writer.startStream(self._pg_conn)

    # --------------------------------------------------------------------------
    def endElement(self, tag):
        if tag == "log":
            self._clean_shutdown = True

    # --------------------------------------------------------------------------
    def characters(self, content):
        pass

    # --------------------------------------------------------------------------
    def processLine(self, line):
        self._parser.feed(line)

    # --------------------------------------------------------------------------
    def processDisconnect(self):
        if self._pg_conn is not None and self._streamid is not None:
            self._db_writer.finishStream(
                self._pg_conn, 
                self._streamid,
                self._clean_shutdown)

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
    return NetworkLogSocket(options.socket_port)

# ------------------------------------------------------------------------------
def handle_connections(log_sock, db_writer):
    global keepRunning
    with selectors.DefaultSelector() as selector:
        selector.register(
            log_sock,
            selectors.EVENT_READ,
            data = db_writer
        )

        while keepRunning:
            events = selector.select(timeout=1.0)
            for key, mask in events:
                if type(key.data) is XmlLogDbWriter:
                    connection, addr = log_sock.accept()
                    connection.setblocking(False)
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
def main():
    try:
        options = getArgumentParser().parse_args()
        signal.signal(signal.SIGINT, handleInterrupt)
        signal.signal(signal.SIGTERM, handleInterrupt)
        db_writer = XmlLogDbWriter(options)
        handle_connections(open_socket(options), db_writer)
    except KeyboardInterrupt:
        return 0
# ------------------------------------------------------------------------------
if __name__ == "__main__":
        sys.exit(main())

