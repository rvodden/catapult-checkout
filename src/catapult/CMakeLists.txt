standard_build()

add_custom_command(
    TARGET ${ExecutableName} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/data/seed.csv
            ${CMAKE_CURRENT_BINARY_DIR}/seed.csv)
