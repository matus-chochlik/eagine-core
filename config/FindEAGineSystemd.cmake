#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
eagine_common_import_lib(
	 PREFIX SYSTEMD
	 PKGCONFIG libsystemd
	 HEADER systemd/sd-daemon.h
	 LIBRARY systemd
)
