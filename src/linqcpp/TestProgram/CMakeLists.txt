project(TestProgram)

add_executable(${PROJECT_NAME}
	main.cpp
	ProgramUtils.h
	ProgramUtils.cpp
	)

install_target(${PROJECT_NAME})
