#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
configure_file(
	${PROJECT_SOURCE_DIR}/config/cpp/test_compiler.cpp
	${PROJECT_BINARY_DIR}/cpp/test_compiler.cpp
)
try_compile(
	SUPPORTED_COMPILER
	${PROJECT_BINARY_DIR}/cpp
	${PROJECT_BINARY_DIR}/cpp/test_compiler.cpp
)
if(NOT ${SUPPORTED_COMPILER})
	message(FATAL_ERROR
	"Your compiler does not support all required C++ features."
	)
endif()
