#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#

if(WITH_CLANG_TIDY)
	if(DEFINED ENV{EAGINE_CLANG_TIDY})
		find_program(
			CLANG_TIDY_PROG
			NAMES "$ENV{EAGINE_CLANG_TIDY}"
			DOC "Path to clang-tidy executable"
		)
	else()
		find_program(
			CLANG_TIDY_PROG
			NAMES "clang-tidy"
			DOC "Path to clang-tidy executable"
		)
	endif()

	if(CLANG_TIDY_PROG)
		set(INVOKE_CLANG_TIDY "${CLANG_TIDY_PROG}")
	endif()
endif()

macro(eagine_add_exe_analysis TARGETNAME)
	target_compile_options(
		${TARGETNAME}
		PRIVATE $<$<BOOL:${EAGINE_GNUCXX_COMPILER}>:-pedantic;-Wall;-Werror>
	)

	target_compile_options(
		${TARGETNAME}
		PRIVATE $<$<BOOL:${EAGINE_CLANGXX_COMPILER}>:-Weverything;-Wno-sign-conversion;-Wno-old-style-cast;-Wno-c++98-compat;-Wno-c++98-compat-pedantic;-Wno-c++20-compat;-Wno-undef;-Wno-double-promotion;-Wno-global-constructors;-Wno-exit-time-destructors;-Wno-date-time;-Wno-weak-vtables;-Wno-padded;-Wno-missing-prototypes;-Wno-undefined-inline;-Wno-documentation-unknown-command;-Wno-switch-enum;-Wno-ctad-maybe-unsupported;-Wno-used-but-marked-unused;-Wno-c++1z-extensions>
	)
	if(WITH_REFLECTION)
		target_compile_options(
			${TARGETNAME}
			PRIVATE $<$<BOOL:${EAGINE_CLANGXX_COMPILER}>:-Wno-language-extension-token;-Wno-c++20-compat;-Wno-c++2a-extensions>
		)
	endif()

	target_compile_options(
		${TARGETNAME}
		PRIVATE $<$<BOOL:${EAGINE_GXX_COMPILER}>:-Wextra;-Wshadow;-Wno-noexcept-type;-Wno-attributes;-Wno-psabi;-Wno-unknown-warning-option>
	)

	target_compile_options(
		${TARGETNAME}
		PRIVATE $<$<BOOL:(${EAGINE_GXX_COMPILER} OR ${EAGINE_CLANGXX_COMPILER}) AND ${EAGINE_DEBUG}>:-fstack-protector-all>
	)
	
	if(INVOKE_CLANG_TIDY)
		set_target_properties(
			${TARGETNAME}
			PROPERTIES CXX_CLANG_TIDY "${INVOKE_CLANG_TIDY}"
		)
	endif()
endmacro()

macro(eagine_add_lib_analysis TARGETNAME)
	eagine_add_exe_analysis(${TARGETNAME})
endmacro()

