# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
install(
	PROGRAMS
		bin/eagilog-server-start
		bin/eagilog-server-stop
	COMPONENT core-docker
	DESTINATION bin)

install(
	DIRECTORY etc
	COMPONENT core-docker
	DESTINATION /)

eagine_add_package_property(
	core-docker
	GENERATOR DEBIAN
	VARIABLE PACKAGE_DEPENDS
	VALUE "docker")

eagine_add_package_property(
	core-docker
	GENERATOR DEBIAN
	VARIABLE PACKAGE_SUGGESTS
	VALUE "docker-compose")

eagine_add_package_property(
	core-docker
	GENERATOR DEBIAN
	VARIABLE DESCRIPTION
	VALUE "Collection of containerized utilities for EAGine.")


