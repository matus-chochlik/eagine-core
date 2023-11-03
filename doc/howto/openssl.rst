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


Usage with the Soft HSM module
==============================

Installing the required packages
--------------------------------

The required packages can be installed with the following command:

::

  sudo apt install opensc libengine-pkcs11-openssl1.1 softhsm

Configuration of Soft HSM
-------------------------

The Soft HSM (version 2) OpenSSL module can be configured by doing the following:

::

  dd if=/dev/urandom of=~/.rnd bs=256 count=1
  mkdir ~/.config/softhsm2
  echo "directories.tokendir = /path/to/token/dir" > ~/.config/softhsm2/softhsm2.conf


Working with the security tokens
--------------------------------

A new software security token called `eagine` in slot number zero
can be initialized by:

::

  softhsm2-util --init-token --label eagine --slot 0

Another software security token called `user` in slot number one can be created
by this command:

::

  softhsm2-util --init-token --label user --slot 1

The available tokens can be listed with the following command:

::

  softhsm2-util --show-slots


The available PKCS11 modules can be listed with:

::

  pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so -L


Creating a certificate authority
--------------------------------

A new key pair for a certificate authority (CA) stored in the software security
token `eagine` with the label `ca` can be done with the following command:

::

    pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token eagine --login --keypairgen --key-type rsa:8192 --label ca --usage-sign

Objects in the security token `eagine` can be listed with:

::

  pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token eagine --login --list-objects

A self-signed certificate for the CA, using the key stored in the security module
`eagine` can be done with the following command:

::

  openssl req -new -x509 -days 365 -subj '/CN=EAGine certificate authority' -sha512 -engine pkcs11 -keyform engine -key "pkcs11:token=eagine;object=ca" -out ca.crt

The information on the certificate can be printed by doing:

::

  openssl x509 -in oglplus-ca.crt -text

Generating a user private key with signed certificate
-----------------------------------------------------

Key pair for the user stored in the token `user` with the label `user` can
be done with:

::

  pkcs11-tool --module /usr/lib/softhsm/libsofthsm2.so --token user --login --keypairgen --key-type rsa:2048 --label user --usage-sign

The certificate signing request (CSR) for the user certificate for the private
key in the token `user` can be created with:

::

  openssl req -new -engine pkcs11 -keyform engine -key "pkcs11:token=user;object=user" -out user.csr

The certificate with the serial number 1, can be issued by the certificate
authority from the CSR by doing:

::

  openssl x509 -req -CAkeyform engine -engine pkcs11 -in user.csr -CA ca.crt -CAkey "pkcs11:token=eagine;object=ca" -set_serial 1 -sha256 -out user.crt

The user certificate can be verified against the certificate of the CA that
issued it by the following command:

::

  openssl verify -CAfile ca.crt user.crt 

Usage with the ZymKey HSM module
================================

Installing the required driver
------------------------------

The driver for the ZymKey module can be installed with the following command:

::

  curl -G https://s3.amazonaws.com/zk-sw-repo/install_zk_sw.sh | sudo bash

This command be used to confirm if the ZymKey engine works properly:

::

  openssl engine -t -tt -vvvv zymkey_ssl

The hardware security token with the label `ZymKey` in slot zero can be
initialized with:

::

  sudo -g zk_pkcs11 zk_pkcs11-util --init-token --slot 0 --label Zymkey

Creating a certificate authority
--------------------------------

A new private key stored in the ZymKey HSM for the CA can be generated with:

::

  sudo -g zk_pkcs11 zk_pkcs11-util  --use-zkslot 0 --slot 1595944162 --label cakey --id 0001

The objects (like keys) stored in the ZymKey HSM can be listed with:

::

  pkcs11-tool --module /usr/lib/libzk_pkcs11.so -l --token Zymkey --list-objects

A self-signed certificate for the CA, using the key stored in the ZymKey security
module can be done with the following command:

::

  openssl req -new -x509 -days 365 -subj '/CN=EAGine CA key' -sha256 -engine zymkey_ssl -keyform engine -key cakey -out ca.crt

Generating a user private key with signed certificate
-----------------------------------------------------

As before, a new user private key can be created with:

::

  openssl genrsa -out user.key 2048

The certificate signing request for the user private key is created with this
command:

::

  openssl req -new -key user.key -out user.csr

The user certificate can be issued by the CA, the private key of which is
stored in the ZymKey security module by doing:

::

  openssl x509 -req -CAkeyform engine -engine zymkey_ssl -in user.csr -CA ca.crt -CAkey cakey -set_serial 1 -sha256 -out user.crt

The information on the issued certificate can be printed with the following
command:

::

  openssl x509 -in user.crt -text

The user certificate can be verified against the certificate of the CA
that issued the user certificate by doing this:

::

  openssl verify -CAfile ca.crt user.crt 
