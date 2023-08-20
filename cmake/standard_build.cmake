include(cmake/doxygen.cmake)
include(cmake/name_generation.cmake)

function(get_sources Path Glob Role)
    if( EXISTS ${Path} )
        cmake_path(SET SearchPattern ${Path})
        cmake_path(APPEND SearchPattern ${Glob})
        file( GLOB_RECURSE Files CONFIGURE_DEPENDS ${SearchPattern})
    endif()

    if(Files)
        foreach( File ${Files} )
            message( "   Found ${Role} file: ${File}" )
        endforeach()
    endif()
    return(PROPAGATE Files)
endfunction()

function(getLibrarySources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp source )
    set(LibraryFiles ${Files})
    return(PROPAGATE LibraryFiles)
endfunction()

function (getHeaderSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/include *.h source )
    set(HeaderFiles ${Files})
    return(PROPAGATE HeaderFiles)
endfunction()

function (getTestSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/test *.cpp source )
    set(TestFiles ${Files})
    return(PROPAGATE TestFiles)
endfunction()

function (getExecutableSources)
    get_sources(${CMAKE_CURRENT_SOURCE_DIR}/exe *.cpp source )
    set(ExecutableFiles ${Files})
    return(PROPAGATE ExecutableFiles)
endfunction()

function (addIncludeToTargetIncludePath Target Scope)
    target_include_directories(
        ${Target}
        ${Scope}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

function (addSrcToTargetIncludePath Target Scope)
    target_include_directories(
        ${Target}
        ${Scope}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

macro(addStaticAndSharedLibraryTargets)
    add_library( ${ObjectName} OBJECT ${LibraryFiles} )
    add_library( ${LibraryName} SHARED $<TARGET_OBJECTS:${ObjectName}> )
    add_library( ${StaticName} STATIC $<TARGET_OBJECTS:${ObjectName}> )
    set_target_properties(${StaticName} PROPERTIES OUTPUT_NAME ${LibraryName})

    addSrcToTargetIncludePath(${ObjectName} PRIVATE)

    addIncludeToTargetIncludePath(${ObjectName} PUBLIC)
    addIncludeToTargetIncludePath(${LibraryName} PUBLIC)
    addIncludeToTargetIncludePath(${StaticName} PUBLIC)

    set(LibraryName ${LibraryName} PARENT_SCOPE)
    set(ObjectName ${ObjectName} PARENT_SCOPE)
    set(StaticName ${StaticName} PARENT_SCOPE)


    # target_compile_options(${LibraryName} PRIVATE "$<$<CONFIG:DEBUG>:${HA_TARGET_DEBUG_OPTIONS}>")
    # target_compile_options(${ObjectName} PRIVATE "$<$<CONFIG:DEBUG>:${HA_TARGET_DEBUG_OPTIONS}>")
    # target_compile_options(${StaticName} PRIVATE "$<$<CONFIG:DEBUG>:${HA_TARGET_DEBUG_OPTIONS}>")

    set_property(TARGET ${LibraryName} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${ObjectName} PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET ${StaticName} PROPERTY POSITION_INDEPENDENT_CODE ON)
endmacro()

macro(addHeaderOnlyLibrary)
    add_library(${LibraryName} INTERFACE)
    addIncludeToTargetIncludePath(${LibraryName} INTERFACE)
    set(LibraryName ${LibraryName} PARENT_SCOPE)
endmacro()

macro(addLibraryTarget)
    message( CHECK_START "Loading ${LibraryName}..." )
    if( HeaderFiles AND NOT LibraryFiles )
        message("   Detected that ${LibraryName} is a header only library.")
        addHeaderOnlyLibrary()
    elseif( LibraryFiles )
        message("   ${LibraryName} has a library component.")
        addStaticAndSharedLibraryTargets()
    endif()
    message( CHECK_PASS "done." )
endmacro()

macro(addExecutableTarget)
    if( ExecutableFiles )
        add_executable( ${ExecutableName} ${ExecutableFiles} )
        target_link_libraries( ${ExecutableName} PUBLIC ${LibraryName} )

        set(ExecutableName ${ExecutableName} PARENT_SCOPE)
        set_target_properties(${ExecutableName} PROPERTIES OUTPUT_NAME ${LibraryName})
    endif()
endmacro()

function(addLintTargetForTarget Target TargetSources)
    add_custom_command(
        TARGET ${Target} PRE_BUILD
        COMMAND /opt/clang-tools/bin/clang-tidy
        ${TargetSources}
        --config-file='${CMAKE_SOURCE_DIR}/.clang-tidy'
        --header-filter=.*
        --quiet
        -p ${CMAKE_BINARY_DIR}
    )
endfunction()

find_program(CLANG_FORMAT_EXE NAMES clang-format)

function(addFormatTargetForTarget Target TargetSources)
    add_custom_command(
        TARGET ${Target} PRE_BUILD
        COMMAND ${CLANG_FORMAT_EXE}
        -i
        -style=file:${CMAKE_SOURCE_DIR}/.clang-format
        --dry-run
        ${TargetSources}
    )
endfunction()

macro(addLintTarget)
    if(LibraryFiles)
        addLintTargetForTarget(${LibraryName} ${LibraryFiles})
        addFormatTargetForTarget(${LibraryName} ${LibraryFiles})
    endif()
    if(ExecutableFiles)
        addLintTargetForTarget(${ExecutableName} ${ExecutableFiles})
        addFormatTargetForTarget(${ExecutableName} ${ExecutableFiles})
    endif()
endmacro()

macro(addTestTarget)
    if( TestFiles )
        add_executable ( ${TestName} ${TestFiles} )
        target_link_libraries( ${TestName} PRIVATE ${LibraryName} )
        target_link_libraries( ${TestName} PUBLIC gtest )
        target_link_libraries( ${TestName} PUBLIC gmock )
        target_link_libraries( ${TestName} PUBLIC gtest_main )

        addSrcToTargetIncludePath(${TestName} PRIVATE)
        gtest_discover_tests( ${TestName} XML_OUTPUT_DIR report EXTRA_ARGS --gtest_catch_exceptions=0 )
    endif()
endmacro()

macro(standard_build)
    # set(HA_TARGET_DEBUG_OPTIONS -fsanitize=address -static-libasan)
    add_compile_options(-fdiagnostics-color=always)
    generateLibraryName(${CMAKE_CURRENT_SOURCE_DIR})
    generateObjectName(${CMAKE_CURRENT_SOURCE_DIR})
    generateStaticName(${CMAKE_CURRENT_SOURCE_DIR})
    generateTestName(${CMAKE_CURRENT_SOURCE_DIR})
    generateExecutableName(${CMAKE_CURRENT_SOURCE_DIR})

    getHeaderSources()
    getLibrarySources()
    getExecutableSources()
    getTestSources()

    addLibraryTarget()
    addExecutableTarget()
    addLintTarget()
    addTestTarget()
endmacro()

function(add_dependency Target Scope Dependency)
    get_target_property(type ${Target} TYPE)
    if (${type} STREQUAL "SHARED_LIBRARY")
        generateStaticNameFromLibraryName(${Target})
        generateObjectNameFromLibraryName(${Target})
        target_link_libraries(${Target} ${Scope} ${Dependency})
        target_link_libraries(${StaticName} ${Scope} ${Dependency})
        target_link_libraries(${ObjectName} ${Scope} ${Dependency})
    else()
        target_link_libraries(${Target} ${Scope} ${Dependency})
    endif()
endfunction()
