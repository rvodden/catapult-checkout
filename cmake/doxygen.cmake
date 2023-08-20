find_package(Doxygen
             REQUIRED dot
             OPTIONAL_COMPONENTS mscgen dia)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
set(DOXYGEN_CALLER_GRAPH YES)
set(DOXYGEN_CALL_GRAPH YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_GENERATE_TREEVIEW YES)
# svg files are much smaller than jpeg and png, and yet they have higher quality
set(DOXYGEN_DOT_IMAGE_FORMAT svg)
set(DOXYGEN_DOT_TRANSPARENT YES)
set(DOXYGEN_THEME "awesome-sidebar")
set(DOXYGEN_PROJECT_LOGO "${CMAKE_SOURCE_DIR}/assets/logo.png")
set(DOXYGEN_SHOW_NAMESPACES NO)
set(DOXYGEN_EXCLUDE_SYMBOLS "catapult::testing")

include(FetchContent)
FetchContent_Declare(_doxygen_theme
                         URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/tags/v1.6.1.zip)
FetchContent_MakeAvailable(_doxygen_theme)
set(DOXYGEN_HTML_EXTRA_STYLESHEET ${DOXYGEN_HTML_EXTRA_STYLESHEET} "${_doxygen_theme_SOURCE_DIR}/doxygen-awesome-sidebar-only.css")

doxygen_add_docs(doxygen-docs ALL "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/README.md"
                   COMMENT "Generating documentation - entry file: ${CMAKE_CURRENT_BINARY_DIR}/html/index.html")
