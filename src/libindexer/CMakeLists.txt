set(target_name indexer)
add_library(${target_name} STATIC
	indexer.cpp
)
include(CompileOptions)
set_compile_options(${target_name})
target_include_directories(
	${target_name}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)
set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(
	${target_name}
	PRIVATE
		parser
)