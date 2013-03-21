project(Utils)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	ProgramUtils.h
	ProgramUtils.cpp
	)
	
install_target(${PROJECT_NAME})
