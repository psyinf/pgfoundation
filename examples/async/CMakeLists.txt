project(async)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		pgf::pgf
        fmt::fmt
)
target_code_coverage(${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/${PROJECT_NAME})

