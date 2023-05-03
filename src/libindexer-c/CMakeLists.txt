set(target_name indexer_wrap)
add_library(${target_name} STATIC
	indexer_wrap.cpp
)
include(CompileOptions)
set_compile_options(${target_name})
set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(
	${target_name}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
	${target_name}
	PRIVATE
		indexer
)