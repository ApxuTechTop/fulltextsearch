set(target_name luafts)
project(${target_name})
add_library(${target_name} SHARED libfts.c)

target_include_directories(
	${target_name}
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror -pedantic)
set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(
	${target_name}
	PRIVATE
		lua::lib
        indexer_wrap
        searcher_wrap
        accessor_wrap
)
