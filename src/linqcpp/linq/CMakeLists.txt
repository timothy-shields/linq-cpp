project(linq)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	Comparer.h
	Comparer.cpp
	Enumerable.h
	Enumerable.cpp
	
	interactive.h
	
	empty_enumerable.h
	empty_enumerator.h
	
	return_enumerable.h
	return_enumerator.h

	for_enumerable.h
	for_enumerator.h
	
	select_enumerable.h
	select_enumerator.h

	select_many_enumerable.h
	select_many_enumerator.h

	where_enumerable.h
	where_enumerator.h
	
	)
	
install_target(${PROJECT_NAME})
