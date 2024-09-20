message(STATUS "Checking conan executable and dependencies")
find_program(conanexecutable "conan")
set(CMAKE_CXX_STANDARD 20)
if(NOT conanexecutable)
    message(WARNING "Tool conan is not installed. Check README.md for build instructions without conan.")
else()
    file(COPY conanfile.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                      "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Finished downloading conan.cmake")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" ${CMAKE_MODULE_PATH})
    set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" ${CMAKE_PREFIX_PATH})
endif()
