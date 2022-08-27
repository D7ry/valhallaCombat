set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(SOURCE_DIR "${ROOT_DIR}/src")
set(SOURCE_FILES
	"${SOURCE_DIR}/bin/main.cpp"
	"${SOURCE_DIR}/bin/AI.cpp"
	"${SOURCE_DIR}/bin/attackHandler.cpp"
	"${SOURCE_DIR}/bin/balanceHandler.cpp"
	"${SOURCE_DIR}/bin/blockHandler.cpp"
	"${SOURCE_DIR}/bin/data.cpp"
	"${SOURCE_DIR}/bin/debuffHandler.cpp"
	"${SOURCE_DIR}/bin/executionHandler.cpp"
	"${SOURCE_DIR}/bin/hitProcessor.cpp"
	"${SOURCE_DIR}/bin/Hooks.cpp"
	"${SOURCE_DIR}/bin/main.cpp"
	"${SOURCE_DIR}/bin/Papyrus.cpp"
	"${SOURCE_DIR}/bin/reactionHandler.cpp"
	"${SOURCE_DIR}/bin/settings.cpp"
	"${SOURCE_DIR}/bin/staminaHandler.cpp"
	"${SOURCE_DIR}/bin/stunHandler.cpp"
	"${SOURCE_DIR}/bin/ValhallaCombat.hpp"
	"${SOURCE_DIR}/bin/events/animEventHandler.cpp"
	"${SOURCE_DIR}/bin/events/cellLoadEventHandler.cpp"
	"${SOURCE_DIR}/bin/events/inputEventHandler.cpp"
	"${SOURCE_DIR}/bin/ModAPI.cpp"
	"${SOURCE_DIR}/include/ModAPI.h"
	"${SOURCE_DIR}/include/actorManager.h"
	"${SOURCE_DIR}/include/actorManager.h"
	"${SOURCE_DIR}/include/AI.h"
	"${SOURCE_DIR}/include/attackHandler.h"
	"${SOURCE_DIR}/include/balanceHandler.h"
	"${SOURCE_DIR}/include/blockHandler.h"
	"${SOURCE_DIR}/include/data.h"
	"${SOURCE_DIR}/include/debuffHandler.h"
	"${SOURCE_DIR}/include/events.h"
	"${SOURCE_DIR}/include/executionHandler.h"
	"${SOURCE_DIR}/include/hitProcessor.h"
	"${SOURCE_DIR}/include/Hooks.h"
	"${SOURCE_DIR}/include/offsets.h"
	"${SOURCE_DIR}/include/Papyrus.h"
	"${SOURCE_DIR}/include/PCH.h"
	"${SOURCE_DIR}/include/settings.h"
	"${SOURCE_DIR}/include/staminaHandler.h"
	"${SOURCE_DIR}/include/stunHandler.h"
	"${SOURCE_DIR}/include/Utils.h"
	"${SOURCE_DIR}/include/lib/BlockSpark.h"
	"${SOURCE_DIR}/include/lib/robin_hood.h"
	"${SOURCE_DIR}/include/lib/SimpleIni.h"
	"${SOURCE_DIR}/include/lib/TrueHUDAPI.h"
	"${SOURCE_DIR}/include/lib/ValhallaCombatAPI.h"
	"${SOURCE_DIR}/include/lib/PrecisionAPI.h"

	"${SOURCE_DIR}/include/perilousAttack.h"
	"${SOURCE_DIR}/bin/perilousAttack.cpp"
)

source_group(TREE "${ROOT_DIR}" FILES ${SOURCE_FILES})

set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"${ROOT_DIR}/cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)

source_group("src" FILES "${VERSION_HEADER}")

configure_file(
	"${ROOT_DIR}/cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCE_FILES}
	"${VERSION_HEADER}"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	"${ROOT_DIR}/.clang-format"
	"${ROOT_DIR}/.editorconfig"
)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_20
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/await"

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/src"
		"${SOURCE_DIR}"
)

set(SKSE_SUPPORT_XBYAK ON)

add_subdirectory("$ENV{CommonLibSSEPath_NG}" CommonLibSSE EXCLUDE_FROM_ALL)

find_package(xbyak REQUIRED CONFIG)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		CommonLibSSE::CommonLibSSE
		xbyak::xbyak
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		"${SOURCE_DIR}/PCH.h"
)

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT "main"
)

install(
	FILES
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "/"
	COMPONENT "pdbs"
)

if("${COPY_OUTPUT}")
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		VERBATIM
	)
endif()
