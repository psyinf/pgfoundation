project(pgf LANGUAGES CXX)
include(GNUInstallDirs)
add_code_coverage()

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        "src/taskengine/TaskEngine.cpp"
        "src/strings/StringTools.cpp"
    PUBLIC
        "include/pgf/taskengine/TaskEngine.hpp"
        "include/pgf/serialization/Yaml2Json.hpp"
        "include/pgf/console/miniAnsi.hpp"
        "include/pgf/strings/StringTools.hpp"
        "include/pgf/strings/FixedLengthString.hpp"
        "include/pgf/filesystem/directory.hpp"
        "include/pgf/caching/GenericFactory.hpp"
        "include/pgf/caching/ResourceCache.hpp"
        "include/pgf/caching/ResourceLocator.hpp"
        "include/pgf/caching/ResourceManager.hpp"
  #  PRIVATE
)
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/include)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    nlohmann_json::nlohmann_json
    yaml-cpp::yaml-cpp
    PRIVATE
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    LINKER_LANGUAGE CXX
    CXX_STANDARD 23
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX "d"
)
add_compile_definitions(${PROJECT_NAME} USE_SPDLOG ON)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_code_coverage(${PROJECT_NAME})

