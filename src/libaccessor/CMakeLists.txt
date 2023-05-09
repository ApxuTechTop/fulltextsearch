set(target_name text_accessor)
add_library(${target_name} STATIC
	text_accessor.cpp
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
		
)

set(target_name binary_accessor)
add_library(${target_name} STATIC
	binary_accessor.cpp
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
		
)

add_library(accessor INTERFACE)
target_link_libraries(accessor INTERFACE text_accessor binary_accessor)
