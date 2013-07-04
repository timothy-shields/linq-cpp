cmake_minimum_required(VERSION 2.8.7)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

macro(add_preprocessor_definition PREPROCESSOR_DEFINITION)
	add_definitions(-D${PREPROCESSOR_DEFINITION})
endmacro()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	)
	
add_preprocessor_definition(NOMINMAX)
add_preprocessor_definition(WIN32_LEAN_AND_MEAN)

set(LIB_TYPE STATIC)

macro(install_target_configuration TARGET_NAME CONFIGURATION_NAME)
	INSTALL(TARGETS ${TARGET_NAME}
		CONFIGURATIONS ${CONFIGURATION_NAME}
		RUNTIME DESTINATION ${CONFIGURATION_NAME}/bin
		LIBRARY DESTINATION ${CONFIGURATION_NAME}/lib
		ARCHIVE DESTINATION ${CONFIGURATION_NAME}/lib
		)
endmacro()

macro(install_target TARGET_NAME)
	install_target_configuration(${TARGET_NAME} Debug)
	install_target_configuration(${TARGET_NAME} Release)
	install_target_configuration(${TARGET_NAME} RelWithDebInfo)
	install_target_configuration(${TARGET_NAME} MinSizeRel)
endmacro()

add_subdirectory(linqcpp)
add_subdirectory(Optional)