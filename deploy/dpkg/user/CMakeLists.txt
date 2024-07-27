# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
cpack_add_component(
	user-account
	DESCRIPTION "Meta-package maintaining the EAGine user account.")

install(
	FILES
		_config/eagine/systemd/environment
		_config/eagine/systemd/log_environment
	COMPONENT user-account
	PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	DESTINATION /var/lib/eagine/.config)

install(
	DIRECTORY etc
	COMPONENT user-account
	DESTINATION /)

install(
	DIRECTORY usr
	COMPONENT user-account
	DESTINATION /)

eagine_add_package_conffiles(user-account etc)

eagine_add_license(user-account)
eagine_add_debian_changelog(user-account)

eagine_add_package_property(
	user-account
	GENERATOR DEBIAN
	VARIABLE PACKAGE_SUGGESTS
	VALUE "wmdocker")

eagine_add_package_property(
	user-account
	GENERATOR DEBIAN
	VARIABLE DESCRIPTION
	VALUE "Meta-package managing EAGine user account.")

eagine_add_package_property(
	user-account
	GENERATOR DEBIAN
	VARIABLE PACKAGE_CONTROL_EXTRA 
	VALUE "preinst;postinst;postrm")

eagine_add_package_property(
	user-account
	GENERATOR DEBIAN
	VARIABLE PACKAGE_ARCHITECTURE
	VALUE "all")
