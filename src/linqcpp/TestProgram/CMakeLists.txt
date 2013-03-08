project(TestProgram)

add_executable(${PROJECT_NAME}
	main.cpp
	)

target_link_libraries(${PROJECT_NAME}
	Utils
	)

install_target(${PROJECT_NAME})
