project(linq)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	Comparer.h
	Comparer.cpp
	Enumerable.h
	Enumerable.cpp
	Functional.h
	Functional.cpp
	PairingHeap.h
	PairingHeap.cpp
	)
	
install_target(${PROJECT_NAME})
