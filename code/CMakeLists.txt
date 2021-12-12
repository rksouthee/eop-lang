
add_library(libeopc
	eopc.natvis
	token_iterator.cpp
	token_iterator.h
	parser.cpp
	parser.h
	symbol.cpp
	symbol.h
	)
target_compile_features(libeopc PRIVATE cxx_std_17)

add_executable(eopc main.cpp)
target_link_libraries(eopc PRIVATE libeopc)

# TODO only do this if testing is enabled
# TODO do we want to use Catch2?
add_executable(tests
	token_iterator.test.cpp
	parser.test.cpp
	)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain libeopc)
catch_discover_tests(tests)
