#!/usr/bin/env python3
# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt

import os
import re
import sys
import json
import base64
import shutil
import logging
import datetime
import argparse
import tempfile

# ------------------------------------------------------------------------------
#  Helpers
# ------------------------------------------------------------------------------
def exceptionMessage(e):
    return repr(e)
# ------------------------------------------------------------------------------
# Digital signatures
# ------------------------------------------------------------------------------
def listSignedAttributes(options, header):
    if options.sign_attributes == [["*"]]:
        def _get(header):
            for key in header:
                if key != "signed":
                    yield key
        return [[key] for key in _get(header)]
    else:
        return options.sign_attributes
# ------------------------------------------------------------------------------
def getSignedAttributes(options, header):
    signed = {}
    if options.sign_attributes == [["*"]]:
        signed = header.copy()
        try: del signed["signed"]
        except KeyError: pass
    else:
        for attrib in options.sign_attributes:
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
#  PyCaCrypto
# ------------------------------------------------------------------------------
class PyCaCryptoDataSigner(object):
    # -------------------------------------------------------------------------
    def __init__(self, arg_parser, hashes, padding, serialization, x509):
        self._hashes = hashes 
        self._padding = padding 
        self._serialization = serialization
        self._x509 = x509 

        arg_parser.add_argument(
            "--sign-certificate",
            metavar="FILE",
            type=os.path.realpath,
            default=None
        )
        arg_parser.add_argument(
            "--sign-key",
            metavar="FILE",
            type=os.path.realpath,
            default=None
        )
        arg_parser.add_argument(
            "--sign-hash",
            metavar="HASH-ID",
            choices=["sha256", "sha384", "sha512"],
            default="sha256"
        )
    # -------------------------------------------------------------------------
    def processParsedOptions(self, arg_parser, options):
        self._options = options

    # -------------------------------------------------------------------------
    def beginSign(self, header):
        with open(self._options.sign_certificate, "rb") as certfd:
            self._cert = self._x509.load_pem_x509_certificate(certfd.read());
        with open(self._options.sign_key, "rb") as pkeyfd:
            self._pkey = self._serialization.load_pem_private_key(
                pkeyfd.read(),
                None);
        self._data = bytes()

    # -------------------------------------------------------------------------
    def addData(self, header, data):
        if isinstance(data, str):
            data = data.encode("utf-8")
        assert isinstance(data, bytes)
        self._data += data

    # -------------------------------------------------------------------------
    def getMDType(self, name):
        if name == "sha384":
            return self._hashes.SHA384()
        if name == "sha512":
            return self._hashes.SHA512()
        return self._hashes.SHA256()

    # -------------------------------------------------------------------------
    def signData(self, options):
        md_type = self.getMDType(self._options.sign_hash)
        signature = self._pkey.sign(
            self._data,
            self._padding.PSS(
                mgf=self._padding.MGF1(md_type),
                salt_length=self._padding.PSS.MAX_LENGTH),
            md_type)

        signature = base64.b64encode(signature)
        signature = signature.decode("ascii")
        return signature

    # -------------------------------------------------------------------------
    def verifySignature(self, options, signature):
        sig = signature.encode("ascii")
        sig = base64.b64decode(sig)

        pubkey = self._cert.public_key()
        if signature is not None:
            if not pubkey:
                self._options.error("failed to get key from certificate")
                return False
            try:
                md_type = self.getMDType(self._options.sign_hash)
                pubkey.verify(
                    base64.b64decode(signature),
                    self._data,
                    self._padding.PSS(
                        mgf=self._padding.MGF1(md_type),
                        salt_length=self._padding.PSS.MAX_LENGTH),
                    md_type)
            except Exception as err:
                self._options.error(
                    "failed to verify signature: %s",
                    exceptionMessage(err))
                return False
        return True

    # -------------------------------------------------------------------------
    def finishSign(self, options):
        sig = {}
        signature = self.signData(options)
        if self.verifySignature(options, signature):
            sig["algorithm"] = options.sign_hash
            sig["padding"] = "PSS"
            with open(options.sign_certificate, "rt") as certfd:
                sig["certificate"] = certfd.read();
            sig["signature"] = signature
        self._data = None
        self._cert = None
        self._pkey = None
        return sig

# ------------------------------------------------------------------------------
def makeSigner(arg_parser):
    try:
        from cryptography.hazmat.primitives import hashes
        from cryptography.hazmat.primitives.asymmetric import padding
        from cryptography.hazmat.primitives import serialization
        from cryptography import x509
        return PyCaCryptoDataSigner(
            arg_parser,
            hashes, padding, serialization, x509)
    except ImportError:
        pass

    return None
# ------------------------------------------------------------------------------
# Argument parsing
# ------------------------------------------------------------------------------
class ArgumentParser(argparse.ArgumentParser):
    # -------------------------------------------------------------------------
    def __init__(self, **kw):
        self._path_re = re.compile("^[A-Za-z_][A-Za-z0-9_]*(\.[A-Za-z_][A-Za-z0-9_]*)*$")

        def _valid_input_path(x):
            try:
                p = os.path.realpath(x)
                assert os.path.isfile(p)
                assert os.access(p, os.R_OK)
                return p
            except:
                self.error("`%s' is not an existing readable file" % str(x))

        def _valid_addition(x):
            if not self._path_re.match(x):
                try:
                    return int(x)
                except ValueError:
                    try:
                        return float(x)
                    except ValueError:
                        if x in ["null", "Null", "NULL"]:
                            return None
                        if x in ["true", "True", "TRUE"]:
                            return True
                        if x in ["false", "False", "FALSE"]:
                            return True
            return x

        def _valid_attribute(x):
            try:
                assert x == "*" or self._path_re.match(x)
                return x
            except:
                self.error("invalid attribute specifier '%s'" % s)

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
            "--add", "-a",
            metavar=('ATTRIBUTE','VALUE'),
            dest='additions',
            nargs=2,
            type=_valid_addition,
            action="append",
            default=[]
        )

        self.add_argument(
            "--set", "-s",
            metavar=('ATTRIBUTE','VALUE'),
            dest='updates',
            nargs=2,
            type=_valid_addition,
            action="append",
            default=[]
        )

        self.add_argument(
            "--delete", "-d",
            metavar='ATTRIBUTE',
            dest='deletions',
            nargs='?',
            type=_valid_attribute,
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

        self._signer = makeSigner(self)
        if self._signer is not None:
            self.add_argument(
                "--sign-attribute",
                metavar='ATTRIBUTE',
                dest='sign_attributes',
                nargs='?',
                type=_valid_attribute,
                action="append",
                default=[]
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

        options.additions = [(_processKey(k), v) for k, v in options.additions]
        options.updates = [(_processKey(k), v) for k, v in options.updates]
        options.deletions = [_processKey(k) for k in options.deletions]

        options.signer = self._signer
        if options.signer is not None:
            options.sign_attributes = [_processKey(k) for k in options.sign_attributes]

        logging.basicConfig(
            encoding="utf-8",
            format='[%(levelname)s: %(message)s',
            level=logging.DEBUG if options.debug else logging.INFO)
        options._log = logging.getLogger("fmd-modify")

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
        options = _Options(self.processParsedOptions(
            argparse.ArgumentParser.parse_args(self)
        ))

        if options.signer is not None:
            options.signer.processParsedOptions(self, options)

        return options
# ------------------------------------------------------------------------------
def getArgumentParser():
    return ArgumentParser(
        prog=os.path.basename(__file__),
        description="""
            Modifies metadata in EAGine files with JSON header.
        """
    )
# ------------------------------------------------------------------------------
def updateHeader(options, header):
    for attrib in options.deletions:
        node = header
        try:
            for key in attrib[:-1]:
                node = node[key]
            del node[attrib[-1]]
        except KeyError:
            pass

    for attrib, value in options.updates:
        node = header
        for key in attrib[:-1]:
            node = node.setdefault(key, {})
        node[attrib[-1]] = value

    for attrib, value in options.additions:
        node = header
        for key in attrib[:-1]:
            node = node.setdefault(key, {})

        key = attrib[-1]
        try:
            node[key].append(value)
        except KeyError:
            node[key] = [value]
        except AttributeError:
            node[key] = [node[key], value]

    return header
# ------------------------------------------------------------------------------
def formatHeader(options, header, ofd):
    def _write(s):
        ofd.write(s.encode("utf-8"))
    first = True
    for key, value in header.items():
        if first:
            _write("{")
            first = False
        else:
            _write(",")
        _write(json.dumps(key))
        _write(':')
        _write(json.dumps(value, separators=(',',':')))
        _write('\n')
    _write("}")
# ------------------------------------------------------------------------------
def addSignature(signer, options, header, datafd):
    signer.beginSign(header)
    while True:
        chunk = datafd.read(4096)
        if not chunk:
            break
        signer.addData(header, chunk)
    signer.addData(header, getSignedAttributes(options, header))
    date_str = datetime.datetime.now(datetime.timezone.utc).isoformat()
    signer.addData(header, date_str)
    sig = signer.finishSign(options)
    if sig is not None:
        sig["date"] = date_str
        sig["attributes"] = ['.'.join(a) for a in listSignedAttributes(options, header)]
        header.setdefault("signed", []).append(sig)
# ------------------------------------------------------------------------------
def processFile(options, path):
    with open(path, "rb") as inputfd:
        decoder = json.JSONDecoder()
        head_bytes = bytes()
        with tempfile.NamedTemporaryFile(mode='wb', delete=False) as datafd:
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
                    break
                except json.JSONDecodeError as jde:
                    pass
            datafd.write(data)
            while True:
                chunk = inputfd.read(4096)
                if not chunk:
                    break
                datafd.write(chunk)

        header = updateHeader(options, header)
        if options.signer is not None:
            with open(datafd.name, "rb") as datafd:
                addSignature(options.signer, options, header, datafd)

        with open(datafd.name, "rb") as datafb:
            with tempfile.NamedTemporaryFile(mode='wb', delete=False) as outputfd:
                formatHeader(options, header, outputfd)
                while True:
                    chunk = datafb.read(4096)
                    if not chunk:
                        break
                    outputfd.write(chunk)
            shutil.move(outputfd.name, path)
        os.unlink(datafd.name)

# ------------------------------------------------------------------------------
def processFiles(options):
    for path in options.input_paths:
        processFile(options, path)
# ------------------------------------------------------------------------------
def printBashCompletion(argparser, options):
    from eagine.argparseUtil import printBashComplete
    def _printIt(fd):
        printBashComplete(
            argparser,
            "_eagine_fmd_modify",
            "eagine-fmd-modify",
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
    options = None
    try:
        argparser = getArgumentParser()
        options = argparser.parseArgs()
        if options.print_bash_completion:
            printBashCompletion(argparser, options)
        else:
            processFiles(options)
        return 0
    except Exception as error:
        print(exceptionMessage(error))
        if options is None or options.debug:
            raise
        return 1

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    sys.exit(main())
