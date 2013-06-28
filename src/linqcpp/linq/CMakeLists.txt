project(linq CXX)

add_library(${PROJECT_NAME} ${LIB_TYPE}
	main.cpp
	
	make_unique.h
	optional.h
	range_traits.h
	
	interactive.h
	
	enumerable.h
	enumerator.h

	captured_enumerable.h
	captured_enumerator.h

	memoize_traits.h
	memoize_enumerable.h
	memoize_enumerator.h

	from_enumerable.h
	from_enumerator.h

	empty_enumerable.h
	empty_enumerator.h
	
	return_enumerable.h
	return_enumerator.h

	iota_enumerable.h
	iota_enumerator.h

	for_enumerable.h
	for_enumerator.h
	
	select_enumerable.h
	select_enumerator.h

	concat_traits.h
	concat_enumerable.h
	concat_enumerator.h

	merge_enumerable.h
	merge_enumerator.h

	where_enumerable.h
	where_enumerator.h

	take_while_enumerable.h
	take_while_enumerator.h

	skip_while_enumerable.h
	skip_while_enumerator.h

	counter_predicate.h
	negated_predicate.h

	static_cast_selector.h
	
	)
	
install_target(${PROJECT_NAME})
