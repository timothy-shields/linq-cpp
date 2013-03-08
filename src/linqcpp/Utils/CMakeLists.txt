project(Utils)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	dllapi.h
	ProgramUtils.h
	ProgramUtils.cpp
	)
	
install_target(${PROJECT_NAME})
