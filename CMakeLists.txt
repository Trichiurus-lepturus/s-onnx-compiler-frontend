cmake_minimum_required(VERSION 3.7)

project(sonnxc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(PROJECT_NAMESPACE antlr_sonnx)

include(ExternalAntlr4Cpp)

include_directories(${ANTLR4CPP_INCLUDE_DIRS})
link_directories(${ANTLR4CPP_LIBS})
message(
  STATUS
    "Found antlr4cpp libs: ${ANTLR4CPP_LIBS} and includes: ${ANTLR4CPP_INCLUDE_DIRS} "
)

antlr4cpp_process_grammar(${PROJECT_NAME} ${PROJECT_NAMESPACE}
                          ${CMAKE_CURRENT_SOURCE_DIR}/grammar/S_ONNX.g4)

include_directories(${antlr4cpp_include_dirs_${PROJECT_NAMESPACE}})

add_executable(sonnxc
               main.cpp
               ast/AST.cpp
               listener/ASTConstructionListener.cpp
               visitor/ASTOutputVisitor.cpp
               utils/Literal2Cpp.cpp
               ${antlr4cpp_src_files_${PROJECT_NAMESPACE}}
)
target_include_directories(sonnxc PRIVATE ${PROJECT_SOURCE_DIR})
add_dependencies(sonnxc antlr4cpp antlr4cpp_generation_${PROJECT_NAMESPACE})

target_link_libraries(sonnxc antlr4-runtime)
