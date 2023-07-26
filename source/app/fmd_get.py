#!/usr/bin/env python3
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt

import os
import re
import sys
import json
import base64
import logging
import argparse
import tempfile

# ------------------------------------------------------------------------------
# Digital signatures
# ------------------------------------------------------------------------------
def getSignedAttributes(verified, header):
    signed = {}
    for attrib in verified.get("attributes", []):
        attrib = attrib.split(".")
        src = header
        dst = signed
        for key in attrib[:-1]:
            src = src.setdefault(key, {})
            dst = dst.setdefault(key, {})
        dst[attrib[-1]] = src[attrib[-1]]
    return json.dumps(
        signed,
        sort_keys=True,
        ensure_ascii=True,
        separators=(',',':'))
# ------------------------------------------------------------------------------
#  OpenSSL.crypto
# ------------------------------------------------------------------------------
class OpenSSLDataVerifier(object):
    # -------------------------------------------------------------------------
    def __init__(self, arg_parser, crypto):
        self._crypto = crypto
        self._cacert = None

        arg_parser.add_argument(
            "--ca-certificate",
            metavar="FILE",
            type=os.path.realpath,
            default=None
        )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, arg_parser, options):
        self._options = options
        try:
            with open(options.ca_certificate, "r") as certfd:
                self._cacert = self._crypto.load_certificate(
                    self._crypto.FILETYPE_PEM,
                    certfd.read());
        except:
            arg_parser.error("failed to load CA certificate from `%s'" % options.ca_certificate)

    # -------------------------------------------------------------------------
    def getX509NameValue(self, name, attrib):
        if len(attrib) == 1:
            for key, value in name.get_components():
                if key.decode("utf-8") == attrib[0]:
                    return value.decode("utf-8")
        return None

    # -------------------------------------------------------------------------
    def getCertAttribValue(self, cert, attrib):
        if attrib == ["serial_number"]:
            return cert.get_serial_number()
        if attrib == ["version"]:
            return cert.get_version()
        if attrib[0] == "subject":
            return self.getX509NameValue(cert.get_subject(), attrib[1:])
        if attrib[0] == "issuer":
            return self.getX509NameValue(cert.get_issuer(), attrib[1:])
        return None;

    # -------------------------------------------------------------------------
    def doGetCertificateAttribute(self, signatures, attrib):
        for signed in signatures:
            sign_cert = signed.get("certificate")
            if sign_cert is not None:
                sign_cert = self._crypto.load_certificate(
                    self._crypto.FILETYPE_PEM,
                    sign_cert)
                yield self.getCertAttribValue(sign_cert, attrib)
            else:
                yield None

    # -------------------------------------------------------------------------
    def getCertificateAttribute(self, signatures, attrib):
        return [v for v in self.doGetCertificateAttribute(signatures, attrib)]

    # -------------------------------------------------------------------------
    def beginVerify(self, header):
        self._data = bytes()

    # -------------------------------------------------------------------------
    def addData(self, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        assert isinstance(data, bytes)
        self._data += data

    # -------------------------------------------------------------------------
    def verifyCertificate(self, sign_cert):
        store = self._crypto.X509Store()
        store.add_cert(self._cacert)

        ctx = self._crypto.X509StoreContext(store, sign_cert)
        try:
            ctx.verify_certificate()
            return True
        except self._crypto.Error as ce:
            self._options.error(
                "failed to verify certificate: %s",
                str(ce))
            return False

    # -------------------------------------------------------------------------
    def verifySignature(self, header, verified):
        algorithm = verified.get("algorithm", "sha256")
        signature = verified.get("signature")
        sign_cert = verified.get("certificate")

        if sign_cert is not None:
            sign_cert = self._crypto.load_certificate(
                self._crypto.FILETYPE_PEM,
                sign_cert)
            if not self.verifyCertificate(sign_cert):
                return False
            if signature is not None:
                try:
                    date = verified.get("date", "")
                    attribs = getSignedAttributes(verified, header)
                    self._crypto.verify(
                        sign_cert,
                        base64.b64decode(signature),
                        date.encode("utf-8") + attribs.encode("utf-8") + self._data,
                        algorithm)
                except self._crypto.Error as ce:
                    self._options.error(
                        "failed to verify signature: %s",
                        str(ce))
                    return False
        return True

    # -------------------------------------------------------------------------
    def finishVerify(self, header):
        try:
            for verified in header.get("signed", []):
                if not self.verifySignature(header, verified):
                    return False
        finally:
            self._data = None
        return True

# ------------------------------------------------------------------------------
def makeVerifier(arg_parser):
    # TODO pyca/cryptography
    try:
        from OpenSSL import crypto
        return OpenSSLDataVerifier(arg_parser, crypto)
    except ImportError:
        pass
    return None
# ------------------------------------------------------------------------------
# Argument parsing
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # --------------------------------------------------------------------------
    def __init__(self, **kw):
        self._path_re = re.compile("^[A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*$")
        self._qry_re = re.compile("^([A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*)(==|!=|<=|>=|<|>|\))(.*)$")

        # ----------------------------------------------------------------------
        def _valid_input_path(x):
            try:
                p = os.path.realpath(x)
                assert os.path.isfile(p)
                assert os.access(p, os.R_OK)
                return p
            except:
                self.error("`%s' is not an existing readable file" % str(x))
        # ----------------------------------------------------------------------
        def _valid_attribute(x):
            try:
                assert self._path_re.match(x)
                return x
            except:
                self.error("invalid attribute specifier '%s'" % x)
        # ----------------------------------------------------------------------
        def _valid_query(x):
            def _value(v):
                try:
                    return int(v)
                except ValueError:
                    try:
                        return float(v)
                    except ValueError:
                        if v in ["null", "Null", "NULL"]:
                            return None
                        if v in ["true", "True", "TRUE"]:
                            return True
                        if v in ["false", "False", "FALSE"]:
                            return True
                return v

            def _operation(o):
                if o == "==":
                    return lambda x,y: x == y
                if o == "!=":
                    return lambda x,y: x != y
                if o == "<=":
                    return lambda x,y: x <= y
                if o == ">=":
                    return lambda x,y: x >= y
                if o == "<":
                    return lambda x,y: x < y
                if o == ">":
                    return lambda x,y: x > y
                if o == ")":
                    return lambda x,y: y in x

            try:
                found = self._qry_re.match(x)
                assert found
                attrib = found.group(1)
                operation = _operation(found.group(3))
                value = _value(found.group(4))
                return (attrib, operation, value)
            except Exception as e:
                self.error("invalid query specifier '%s'" % x)
        # ----------------------------------------------------------------------
        argparse.ArgumentParser.__init__(self, **kw)

        self.add_argument(
            "--print-bash-completion",
            metavar='FILE|-',
            dest='print_bash_completion',
            default=None
        )

        self.add_argument(
            "--debug",
            action="store_true",
            default=False,
            help="""Starts in debug mode."""
        )

        self.add_argument(
            "--value", "-v",
            metavar='ATTRIBUTE',
            dest='queried_values',
            nargs='?',
            type=_valid_attribute,
            action="append",
            default=[]
        )

        self.add_argument(
            "--query", "-q",
            metavar='ATTRIBUTE',
            dest='queries',
            nargs='?',
            type=_valid_query,
            action="append",
            default=[]
        )

        self.add_argument(
            "--input", "-i",
            metavar='INPUT-FILE',
            dest='input_paths',
            nargs='?',
            type=_valid_input_path,
            action="append",
            default=[]
        )

        self.add_argument(
            "_input_paths",
            metavar='INPUT-FILE',
            nargs=argparse.REMAINDER,
            type=_valid_input_path
        )

        self._verifier = makeVerifier(self)
        if self._verifier is not None:
            self.add_argument(
                "--verify-signatures",
                dest='verify_signatures',
                action='store_true',
                default=False
            )

    # -------------------------------------------------------------------------
    def processParsedOptions(self, options):
        options.input_paths = set(options.input_paths)
        options.input_paths.update(set(options._input_paths))
        del options._input_paths
        options.input_paths = list(options.input_paths)

        def _processKey(k):
            try:
                assert isinstance(k, str)
                return k.split(".")
            except:
                self.error("invalid attribute specifier '%s'" % k)

        options.queries = [(_processKey(k), o, v) for k,o,v in options.queries]
        options.queried_values = [_processKey(k) for k in options.queried_values]

        options.verifier = self._verifier
        if options.verifier is not None:
            options.verifier.processParsedOptions(self, options)

        logging.basicConfig(
            encoding="utf-8",
            format='[%(levelname)s: %(message)s',
            level=logging.DEBUG if options.debug else logging.INFO)
        options._log = logging.getLogger("fmd-get")

        return options
    # -------------------------------------------------------------------------
    def parseArgs(self):
        # ----------------------------------------------------------------------
        class _Options(object):
            # ------------------------------------------------------------------
            def __init__(self, base):
                self.__dict__.update(base.__dict__)

            # ------------------------------------------------------------------
            def output(self, what):
                sys.stdout.write(what)

            # ------------------------------------------------------------------
            def error(self, *args, **kwargs):
                self._log.error(*args, **kwargs)

        # ----------------------------------------------------------------------
        return _Options(self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)
        ))
# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Queries metadata in EAGine files with JSON header.
        """
    )
# ------------------------------------------------------------------------------
def extractHeaderAndData(options, path):
    with open(path, "rb") as inputfd:
        decoder = json.JSONDecoder()
        head_bytes = bytes()
        while True:
            chunk = inputfd.read(1024)
            if not chunk:
                break
            head_bytes += chunk
            try:
                data = bytes()
                head_str = head_bytes.decode("utf-8")
            except UnicodeDecodeError as ude:
                data = head_bytes[ude.start:]
                head_bytes = head_bytes[:ude.start]
                head_str = head_bytes.decode("utf-8")
            try:
                header, pos = decoder.raw_decode(head_str)
                data = head_str[pos:].encode("utf-8") + data
                yield header
                break
            except json.JSONDecodeError as jde:
                pass
        yield data
        while True:
            chunk = inputfd.read(4096)
            if not chunk:
                break
            yield chunk
        return
    yield {}

# ------------------------------------------------------------------------------
def getAttribValue(options, header, attrib):
    node = header
    try:
        for key in attrib[:-1]:
            node = node[key]
        return node[attrib[-1]]
    except TypeError:
        pass
    except KeyError:
        pass

    if attrib[0] == "signed" and attrib[1] == "certificate":
        if "signed" in header:
            if options.verifier is not None:
                return options.verifier.getCertificateAttribute(
                    header["signed"],
                    attrib[2:])

    return None

# ------------------------------------------------------------------------------
def writeAttributeValue(options, value):
    if isinstance(value, list):
        options.output('[')
        first = True
        for e in value:
            if first:
                first = False
            else:
                options.output(',')
            writeAttributeValue(options, e)
        options.output(']')
    else:
        options.output(str(value))
# ------------------------------------------------------------------------------
def processFile(options, path):
    first = True
    verify_first = True
    for item in extractHeaderAndData(options, path):
        if first:
            first = False
            header = item
        elif options.verify_signatures:
                if verify_first:
                    verify_first = False
                    options.verifier.beginVerify(header)
                options.verifier.addData(item)
        else:
            break

    if options.verify_signatures:
        if not options.verifier.finishVerify(header):
            # TODO print info
            return False

    first = True
    if len(options.queried_values) > 0:
        for attrib in options.queried_values:
            if first:
                first = False
            else:
                options.output('\t')
            writeAttributeValue(options, getAttribValue(options, header, attrib))
        options.output('\n')

    for attrib, operation, value in options.queries:
        if not operation(getAttribValue(options, header, attrib), value):
            return False
    return True
# ------------------------------------------------------------------------------
def processFiles(options):
    for path in options.input_paths:
        if not processFile(options, path):
            return 1
    return 0
# ------------------------------------------------------------------------------
#  bash completion
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_fmd_get",
            "eagine-fmd-get",
            ["--print-bash-completion"],
            fd)
    if options.print_bash_completion == "-":
        _printIt(sys.stdout)
    else:
        with open(options.print_bash_completion, "wt") as fd:
            _printIt(fd)

# ------------------------------------------------------------------------------
#  Main function
# ------------------------------------------------------------------------------
def main():
    try:
        argparser = getArgumentParser()
        options = argparser.parseArgs()
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
            return 0
        else:
            return processFiles(options)
    except Exception as error:
        print(type(error), error)
        raise
    return 2

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    sys.exit(main())
