set(target_name math_test)
enable_testing()
add_executable(${target_name})

include(GoogleTest)
include(CompileOptions)
set_compile_options(${target_name})

target_sources(
	${target_name}
	PRIVATE
		math.test.cpp
)
target_include_directories(
	${target_name}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${target_name}
	PRIVATE
		math
        GTest::gtest_main
)

gtest_discover_tests(${target_name})

set(parser_target parser_test)
add_executable(${parser_target})

set_compile_options(${parser_target})

target_sources(
	${parser_target}
	PRIVATE
		parser.test.cpp
)
target_include_directories(
	${parser_target}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${parser_target}
	PRIVATE
		parser
        GTest::gtest_main
)

gtest_discover_tests(${parser_target})


set(indexer_target indexer_test)
add_executable(${indexer_target})

set_compile_options(${indexer_target})

target_sources(
	${indexer_target}
	PRIVATE
		indexer.test.cpp
)
target_include_directories(
	${indexer_target}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
	${indexer_target}
	PRIVATE
		indexer
        GTest::gtest_main
)

gtest_discover_tests(${indexer_target})
