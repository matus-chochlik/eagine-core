#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
file(WRITE "${PROJECT_BINARY_DIR}/SOURCE_DIR" "${PROJECT_SOURCE_DIR}")
file(WRITE "${PROJECT_SOURCE_DIR}/BINARY_DIR" "${PROJECT_BINARY_DIR}")
file(WRITE "${PROJECT_SOURCE_DIR}/INSTALL_PREFIX" "${CMAKE_INSTALL_PREFIX}")

