=================
OpenSSL workflows
=================

This document describes some of the common OpenSSL workflows related to the use
of EAGine and it's tools.

Basic usage
===========

Creating a certificate authority
--------------------------------

In order to generate the private key for the certificate authority (CA) do
the following:

::

  openssl genrsa -out ca.key 4096

Generating a certificate signing request (CSR) for the CA certificate with
validity of 3650 days (~10 years) to be signed by some other CA:

::

  openssl req -new -days 3650 -key ca.key -sha256 -out ca.csr

Generating the self-signed CA certificate:

::

  openssl req -new -x509 -days 3650 -key ca.key -sha256 -out ca.crt


Generating a user private key with signed certificate
-----------------------------------------------------

In order to generate the private key for a user do the following:

::

  openssl genrsa -out user.key 4096

Generating a certificate signing request (CSR) for the certificate (including
the public key) for the user's private key:

::

  openssl req -new -key user.key -out user.csr

In order to issue a user certificate signed by the CA, from the certificate
signing request do:

::

 openssl x509 -req -in user.csr -CA ca.crt -CAkey ca.key -set_serial 1 -sha256 -out user.crt

The information on the issued certificate can be printed with:

::

  openssl x509 -in user.crt -text

The user certificate can be verified against the certificate of the CA that
issued it by the following command:

::

  openssl verify -CAfile ca.crt user.crt 
