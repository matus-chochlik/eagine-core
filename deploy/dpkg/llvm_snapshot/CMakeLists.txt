# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
set(PKG_VERSION 0.1.0)

configure_file(llvm-snapshot.gpg llvm-snapshot.gpg COPYONLY)

foreach(PKG_DEB_CODENAME jammy)
	set(PKG_NAME "eagine-llvm-snapshot-source-${PKG_DEB_CODENAME}_${PKG_VERSION}")
	set(PKG_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PKG_NAME}")
	set(PKG_PREFIX "${PKG_DIR}/usr")
	configure_file(build-pkg.sh.in build-pkg-${PKG_DEB_CODENAME}.sh @ONLY)
	configure_file(81_llvm.list.in 81_llvm_${PKG_DEB_CODENAME}.list @ONLY)

	add_custom_command(
		OUTPUT "${PKG_DIR}/DEBIAN/control"
		COMMAND ${BASH_COMMAND}
		ARGS "${CMAKE_CURRENT_BINARY_DIR}/build-pkg-${PKG_DEB_CODENAME}.sh"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Copying files for ${PKG_NAME}.deb package"
	)

	add_custom_command(
		OUTPUT ${PKG_NAME}.deb
		COMMAND ${DPKG_DEB_COMMAND}
		ARGS --build --root-owner-group ${PKG_NAME}
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		DEPENDS "${PKG_DIR}/DEBIAN/control"
		COMMENT "Building ${PKG_NAME}.deb package"
	)

	add_custom_target(
		eagine-llvm-snapshot-source-${PKG_DEB_CODENAME}-deb
		DEPENDS ${PKG_NAME}.deb
	)

	add_dependencies(
		eagine-core-deb
		eagine-llvm-snapshot-source-${PKG_DEB_CODENAME}-deb
	)
endforeach()

