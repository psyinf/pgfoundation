if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	include(cmake/cmake-coverage.cmake)
else ()
	message("No code coverage for ${CMAKE_CXX_COMPILER_ID}")
	#add empty pseduo targets so we don't have to modify existing CMakeLists.txt files
	function (add_code_coverage)
	#noop
	endfunction ()

	function (target_code_coverage TARGET)
	#noop
	endfunction()
endif()
