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
