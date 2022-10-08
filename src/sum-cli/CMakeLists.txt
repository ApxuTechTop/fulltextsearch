set(target_name sum)

add_executable(${target_name})

include(CompileOptions)
set_compile_options(${target_name})

target_sources(
	${target_name}
	PRIVATE
		main.cpp
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
		cxxopts
)