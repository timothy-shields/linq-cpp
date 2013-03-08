project(linq)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	dllapi.h
	Enumerable.h
	Enumerable.cpp
	PairingHeap.h
	PairingHeap.cpp
	)
	
install_target(${PROJECT_NAME})
