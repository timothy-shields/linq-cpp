project(linq)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	Comparer.h
	Comparer.cpp
	
	interactive.h
	
	enumerable.h
	enumerator.h

	captured_enumerable.h

	empty_enumerable.h
	empty_enumerator.h
	
	return_enumerable.h
	return_enumerator.h

	for_enumerable.h
	for_enumerator.h

	generate_enumerable.h
	generate_enumerator.h
	
	select_enumerable.h
	select_enumerator.h

	concat.h

	where_enumerable.h
	where_enumerator.h
	
	)
	
install_target(${PROJECT_NAME})
