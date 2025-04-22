cmake_minimum_required(VERSION 3.7)
project(antlr4cpp_fetcher CXX)
include(ExternalProject)

find_package(Git REQUIRED)
find_package(Java COMPONENTS Runtime REQUIRED)

set(ANTLR4CPP_EXTERNAL_ROOT ${CMAKE_BINARY_DIR}/externals/antlr4cpp)
set(ANTLR4CPP_LOCAL_ROOT ${CMAKE_BINARY_DIR}/locals/antlr4cpp)

set(ANTLR4CPP_JAR_LOCATION
    ${PROJECT_SOURCE_DIR}/third_party/antlr-4.13.2-complete.jar)
set(ANTLR4CPP_LOCAL_REPO
    ${PROJECT_SOURCE_DIR}/third_party/antlr4-4.13.2.zip)

if (NOT EXISTS "${ANTLR4CPP_JAR_LOCATION}")
    message(
            FATAL_ERROR
            "Unable to find antlr tool. ANTLR4CPP_JAR_LOCATION:${ANTLR4CPP_JAR_LOCATION}"
    )
endif ()

if (NOT ANTLR4CPP_GENERATED_SRC_DIR)
    set(ANTLR4CPP_GENERATED_SRC_DIR ${CMAKE_BINARY_DIR}/antlr4cpp_generated_src)
endif ()

ExternalProject_Add(
        antlr4cpp
        PREFIX ${ANTLR4CPP_LOCAL_ROOT}
        URL ${ANTLR4CPP_LOCAL_REPO}
        TIMEOUT 10
        LOG_DOWNLOAD ON
        CONFIGURE_COMMAND
        ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release
        -DANTLR4CPP_JAR_LOCATION=${ANTLR4CPP_JAR_LOCATION} -DBUILD_SHARED_LIBS=ON
        -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_SOURCE_DIR:PATH=<SOURCE_DIR>/runtime/Cpp <SOURCE_DIR>/runtime/Cpp
        LOG_CONFIGURE ON
        LOG_BUILD ON
)

ExternalProject_Get_Property(antlr4cpp INSTALL_DIR)

list(APPEND ANTLR4CPP_INCLUDE_DIRS ${INSTALL_DIR}/include/antlr4-runtime)
foreach (src_path misc atn dfa tree support)
    list(APPEND ANTLR4CPP_INCLUDE_DIRS
            ${INSTALL_DIR}/include/antlr4-runtime/${src_path})
endforeach (src_path)

set(ANTLR4CPP_LIBS "${INSTALL_DIR}/lib64")

macro(antlr4cpp_process_grammar antlr4cpp_project antlr4cpp_project_namespace
        antlr4cpp_grammar)

    if (EXISTS "${ANTLR4CPP_JAR_LOCATION}")
        message(STATUS "Found antlr tool: ${ANTLR4CPP_JAR_LOCATION}")
    else ()
        message(
                FATAL_ERROR
                "Unable to find antlr tool. ANTLR4CPP_JAR_LOCATION:${ANTLR4CPP_JAR_LOCATION}"
        )
    endif ()

    add_custom_target(
            "antlr4cpp_generation_${antlr4cpp_project_namespace}"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${ANTLR4CPP_GENERATED_SRC_DIR}
            COMMAND
            "${Java_JAVA_EXECUTABLE}" -jar "${ANTLR4CPP_JAR_LOCATION}" -Werror
            -Dlanguage=Cpp -listener -visitor -o
            "${ANTLR4CPP_GENERATED_SRC_DIR}/${antlr4cpp_project_namespace}" -package
            ${antlr4cpp_project_namespace} "${antlr4cpp_grammar}"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            DEPENDS "${antlr4cpp_grammar}")

    file(GLOB generated_files
            ${ANTLR4CPP_GENERATED_SRC_DIR}/${antlr4cpp_project_namespace}/*.cpp)

    foreach (generated_file ${generated_files})
        list(APPEND antlr4cpp_src_files_${antlr4cpp_project_namespace}
                ${generated_file})
        set_source_files_properties(
                ${generated_file} PROPERTIES COMPILE_FLAGS -Wno-overloaded-virtual)
    endforeach (generated_file)
    message(
            STATUS
            "Antlr4Cpp  ${antlr4cpp_project_namespace} Generated: ${generated_files}")

    set(antlr4cpp_include_dirs_${antlr4cpp_project_namespace}
            ${ANTLR4CPP_GENERATED_SRC_DIR}/${antlr4cpp_project_namespace})
    message(
            STATUS
            "Antlr4Cpp ${antlr4cpp_project_namespace} include: ${ANTLR4CPP_GENERATED_SRC_DIR}/${antlr4cpp_project_namespace}"
    )

endmacro()
