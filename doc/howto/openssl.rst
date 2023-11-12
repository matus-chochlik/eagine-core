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

  openssl genrsa -out ca.pkey 4096

Generating a certificate signing request (CSR) for the CA certificate
to be signed by some other CA:

::

  openssl req -new -key ca.pkey -sha256 -out ca.csr

Generating the self-signed CA certificate with
validity of 3650 days (~10 years):

::

  openssl req -new -x509 -days 3650 -key ca.pkey -sha256 -out ca.crt


Generating a user private key with signed certificate
-----------------------------------------------------

In order to generate the private key for a user do the following:

::

  openssl genrsa -out user.pkey 4096

The information about the generated user key can be printed with:

::

  openssl pkey -in user.pkey -text

Generating a certificate signing request (CSR) for the certificate (including
the public key) for the user's private key:

::

  openssl req -new -key user.pkey -out user.csr

The information in the CSR can be printed with the following command:

::

  openssl req -in user.csr -text

In order to issue a user certificate signed by the CA, from the certificate
signing request do:

::

  openssl x509 -req -in user.csr -CA ca.crt -CAkey ca.pkey -set_serial 1 -sha256 -out user.crt

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
`eagine` can be done with the following command (omitting the `-subj` parameter
will let you specify the certificate subject info on the standard input, as
with previous commands):

::

  openssl req -new -x509 -days 3650 -subj '/CN=EAGine certificate authority' -sha512 -engine pkcs11 -keyform engine -key "pkcs11:token=eagine;object=ca" -out ca.crt

The information on the certificate can be printed by doing:

::

  openssl x509 -in ca.crt -text

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

Usage with the hardware TPM
===========================

Overview of the TPM module usage and related Linux tools
--------------------------------------------------------

The TPM is a cryptographic chip which also provides some limited secure storage capabilities.
Unlike the removable USB cryptographic devices, the TPM chip is integrated into
the main board.  The TPM also implements PCRs (Platform Configuration Registers) -- slots
(typically 24 or more of them) containing cryptographic hashes of device hardware
ant firmware-related information like the BIOS state, the content of the MBR,
etc. Combinations of these hashes can be used to uniquely identify
various aspects of hardware devices and their firmware and operating systems.
Changes in the values of the configuration registers indicate possible tampering
with the device.  The values can be included in the process of encrypting private
data, and so tying the encrypted data to that particular device and possibly
specific firmware/software configurations. Several hashing algorithms are typically
supported.

The PCR registers are indexed from zero. Some of them are vendor-specific while
others have pre-defined meaning, for example:

- 0:  the BIOS structure
- 1:  the BIOS configuration (setting values)
- 2:  the Option ROMs structure
- 3:  the Option ROMs configuration (setting values)
- 4:  the content of the Master Boot Record
- 5:  the configuration of the Master Boot Record
- 7:  manufacturer-specific platform measurement
- 16: debugging support 

Installing the required packages
--------------------------------

The Debian packages that allow to examine and manipulate the status and data
stored in the TPM can be installed with the following command:

::

  apt install tpm2-tools tpm2-openssl


Listing the TPM capabilities
----------------------------

The following command can be used to print information about the supported
cryptographic algorithms:

::

  tpm2_getcap algorithms

The following command can be used to list the active PCRs alongside with the
supported hash algorithms:

::

  tpm2_getcap pcrs

Various fixed and variable property settings of the TPM can be listed with:

::

  tpm2_getcap properties-fixed
  tpm2_getcap properties-variable

Listing the contents of the PCRs
--------------------------------

The following command can be used to list the hash values in the platform
configuration registers:

::

  tpm2_pcrread

The following command can be used to list the SHA256 hash values only
in the specified PCRs:

::

  tpm2_pcrread sha256:0,2,4,7

Other hash algorithms might be supported and can be combined in a single command:

::

  tpm2_pcrread sha384:0+sha256:2,4

Sealing and unsealing of a key in the TPM
-----------------------------------------

The PCR values to be used in key sealing can be selected and stored by
the following commands into a file (`pcr_values`).
For example we can tie the key to the values of BIOS and other
option ROM structure, the content of the MBR and to the vendor-specific
system HW measurement (these are unlikely to change without tampering
with the hardware or OS installation)

::

  echo -n "sha256:0,2,4,7" > pcrs
  tpm2_pcrread -o "pcr_values" "$(<pcrs)"

An authorization policy digest file (`policy_digest`) that will be needed
in the subsequent commands can be created with:

::

  tpm2_createpolicy --policy-pcr -l "$(<pcrs)" -f "pcr_values" -L "policy_digest"

The private key file can be created by doing:

::

  dd if=/dev/random of=user.pkey bs=256 count=1

A new NV RAM space for storing of the key can be defined and the index of that
space which is used to access it can be stored into a file with this command:

::

  tpm2_nvdefine -L "policy_digest" -s "$(stat -c %s private.keyfile)" -a "policyread|policywrite" | sed -n 's/nv-index: \(0x[0-9A-Za-z]\+\).*$/\1/p' | tr -d '\r\n' > nvram_index

The private key generated in previous step can be stored into the created
NV RAM slot by doing:

::

  tpm2_nvwrite -P "pcr:$(<pcrs)" -i "private.keyfile" "$(<nvram_index)"

The key data can be read back from the NV RAM examined and compared with the
original key file by running the following commands:

::

  tpm2_nvread -P "pcr:$(<pcrs)" "$(<nvram_index)" | hexdump -C
  hexdump -C private.keyfile


The unneeded files can now be removed. Keep the content of the `pcrs` file
(it should be something like "sha256:0,2,4,7") and `nvram_index` (the index of
the TPM NV RAM slot where the key is sealed)

::

  rm pcr_values policy_digest private.keyfile

Manipulating objects in TPM
---------------------------

An endorsement key (EK) can be created and context information stored in a file
by doing:

::

  tpm2_createek -G rsa -c ek_rsa.ctx

An attestation key (AK) within the endorsement hierarchy specified by the data
in the `ek_rsa.ctx` file can be created and the AK context data can be stored
in a new file with:

::

  tpm2_createak -C ek_rsa.ctx -G rsa -g sha512 -s rsassa -c ak_rsa.ctx

The attestation key can be loaded into the TPM under a persistent handle
(note the handle value printed by this command):

::

  tpm_evictcontrol -c ak_rsa.ctx

The handles to various transient, persistent and permanent objects can be listed
by the following commands:

::

  tpm2_getcap handles-transient
  tpm2_getcap handles-persistent
  tpm2_getcap handles-permanent



Creating a certificate authority
--------------------------------

If the new object handle is for example `0x81800001` then the corresponding
public key can be obtained by doing:

::

  openssl pkey -provider tpm2 -in handle:0x81800001 -pubout -out ca.pub

Information about the private key can be printed with:

::

  openssl pkey -provider tpm2 -in handle:0x81800001 -text

A certificate signing request (CSR) to be signed by another certificate authority
can be generated with:

::

  openssl req -provider tpm2 -provider default -new -key handle:0x81800001 -sha512 -out ca.csr

Generating a self-signed CA certificate can be done with:

::

  openssl req -provider tpm2 -provider default -new -x509 -days 3650 -key handle:0x81800001 -sha512 -out ca.crt
  
Generating a user private key with signed certificate
-----------------------------------------------------

In order to generate the private key for a user do the following:

::

  openssl genrsa -out user.pkey 4096

Generating a certificate signing request (CSR) for the certificate (including
the public key) for the user's private key:

::

  openssl req -new -key user.pkey -out user.csr

In order to issue a user certificate signed by the CA, from the certificate
signing request do:

::

  openssl x509 -provider tmp2 -provider default -req -in user.csr -CA ca.crt -CAkey handle:0x81800001 -set_serial 1 -sha512 -out user.crt

The information on the issued certificate can be printed with:

::

  openssl x509 -in user.crt -text

The user certificate can be verified against the certificate of the CA that
issued it by the following command:

::

  openssl verify -CAfile ca.crt user.crt 


Usage with the RPi ZymKey HSM module
====================================

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

  openssl genrsa -out user.pkey 2048

The certificate signing request for the user private key is created with this
command:

::

  openssl req -new -key user.pkey -out user.csr

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
