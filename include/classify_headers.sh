#!/usr/bin/env bash

wd="$(dirname ${0})"

find ${wd}/eagine -type f -name *.hpp -print |\
	sed -n "s|^${wd}/\(.*\)$|\1|p" > ${wd}/headers.txt

find ${wd}/eagine -type f -name *.inl -exec \
	grep -H -c -e EAGINE_LIB_FUNC {} \; |\
	sed -n "s|^${wd}/\(.*\):0$|\1|p" > ${wd}/pub_inl.txt

find $(dirname ${0})/eagine -type f -name *.inl -exec \
	grep -H -c -e EAGINE_LIB_FUNC {} \; |\
	sed -n "s|^${wd}/\(.*\):[1-9][0-9]*$|\1|p" > $(dirname ${0})/lib_inl.txt

