# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
origin=${EAGINE_ORIGIN}
sub_module=${EAGINE_SUB_MODULE}
branch=${EAGINE_BRANCH}
build_type=${EAGINE_BUILD_TYPE}
cpu_count=${EAGINE_CPU_COUNT}

while [[ "${1}" != "" ]]
do
	case "${1}" in
		"--origin")
			origin="${2}"
			shift
			;;
		"--sub-module")
			sub_module="${2}"
			shift
			;;
		"--branch")
			branch="${2}"
			shift
			;;
		"--build-type")
			build_type="${2}"
			shift
			;;
		"--cpu-count")
			cpu_count="${2}"
			shift
			;;
		*)
			break
			;;
	esac
	shift
done

