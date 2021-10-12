# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt
#
add_executable(eagine-animated_value animated_value.cpp)
target_link_libraries(eagine-animated_value PUBLIC EAGine::Core)

add_executable(eagine-from_string from_string.cpp)
target_link_libraries(eagine-from_string PUBLIC EAGine::Core)

add_executable(eagine-sudoku_noise sudoku_noise.cpp)
target_link_libraries(eagine-sudoku_noise PUBLIC EAGine::Core)

add_executable(eagine-embed_self embed_self.cpp)
target_link_libraries(eagine-embed_self PUBLIC EAGine::Core)
eagine_embed_target_resources(TARGET eagine-embed_self)

