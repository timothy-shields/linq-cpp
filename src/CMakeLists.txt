cmake_minimum_required(VERSION 2.8.9)

macro(add_preprocessor_definition PREPROCESSOR_DEFINITION)
	add_definitions(-D${PREPROCESSOR_DEFINITION})
endmacro()

set(PACKAGE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../pkg)
set(OPENCV_ROOT ${PACKAGE_ROOT}/opencv)
set(TBB_ROOT ${PACKAGE_ROOT}/tbb40_20120613oss)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${OPENCV_ROOT}/build/include
	${TBB_ROOT}/include
	)

link_directories(
	${TBB_ROOT}/lib/intel64/vc10
	)
	
set(OPENCV_LIB_DIR ${OPENCV_ROOT}/build/x64/vc10/lib)
set(OPENCV_DLL_DIR ${OPENCV_ROOT}/build/x64/vc10/bin)
set(OPENCV_VERSION 243)
file(GLOB OPENCV_RELEASE_LIBS ${OPENCV_LIB_DIR}/*${OPENCV_VERSION}.lib)
file(GLOB OPENCV_DEBUG_LIBS ${OPENCV_LIB_DIR}/*${OPENCV_VERSION}d.lib)
file(GLOB OPENCV_RELEASE_DLLS ${OPENCV_DLL_DIR}/*${OPENCV_VERSION}.dll)
file(GLOB OPENCV_DEBUG_DLLS ${OPENCV_DLL_DIR}/*${OPENCV_VERSION}d.dll)

file(GLOB TBB_DLLS ${TBB_ROOT}/bin/intel64/vc10/*.dll)

macro(target_link_libraries_configuration TARGET_NAME CONFIGURATION_NAME LIB_NAMES)
	foreach(LIB_NAME ${LIB_NAMES})
		target_link_libraries(${TARGET_NAME} ${CONFIGURATION_NAME} ${LIB_NAME})
	endforeach()
endmacro()

macro(target_link_libraries_optimized TARGET_NAME LIB_NAMES)
	target_link_libraries_configuration(${TARGET_NAME} optimized "${LIB_NAMES}")
endmacro()

macro(target_link_libraries_debug TARGET_NAME LIB_NAMES)
	target_link_libraries_configuration(${TARGET_NAME} debug "${LIB_NAMES}")
endmacro()

macro(target_link_libraries_debug_optimized TARGET_NAME LIB_NAMES)
	target_link_libraries_debug(${TARGET_NAME} "${LIB_NAMES}")
	target_link_libraries_optimized(${TARGET_NAME} "${LIB_NAMES}")
endmacro()

macro(target_link_libraries_opencv TARGET_NAME)
	target_link_libraries_debug(${TARGET_NAME} "${OPENCV_DEBUG_LIBS}")
	target_link_libraries_optimized(${TARGET_NAME} "${OPENCV_RELEASE_LIBS}")
endmacro()
	
	
	
add_preprocessor_definition(NOMINMAX)
add_preprocessor_definition(WIN32_LEAN_AND_MEAN)

set(LIB_TYPE SHARED)

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

macro(install_files_configuration FILE_NAMES CONFIGURATION_NAME)
	install(
		FILES ${FILE_NAMES}
		CONFIGURATIONS ${CONFIGURATION_NAME}
		DESTINATION ${CONFIGURATION_NAME}/bin
		)
endmacro()

macro(install_files_debug FILE_NAMES)
	install_files_configuration("${FILE_NAMES}" Debug)
endmacro()

macro(install_files_optimized FILE_NAMES)
	install_files_configuration("${FILE_NAMES}" Release)
	install_files_configuration("${FILE_NAMES}" MinSizeRel)
	install_files_configuration("${FILE_NAMES}" RelWithDebInfo)
endmacro()

macro(install_files_debug_optimized FILE_NAMES)
	install_files_debug("${FILE_NAMES}")
	install_files_optimized("${FILE_NAMES}")
endmacro()

install_files_debug("${OPENCV_DEBUG_DLLS}")
install_files_optimized("${OPENCV_RELEASE_DLLS}")
install_files_debug_optimized("${TBB_DLLS}")