cmake_minimum_required(VERSION 3.7)

project(sonnxc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(PROJECT_NAMESPACE antlr_sonnx)

include(ExternalAntlr4Cpp)

include_directories(${ANTLR4CPP_INCLUDE_DIRS})
link_directories(${ANTLR4CPP_LIBS})
message(STATUS "Found antlr4cpp libs: ${ANTLR4CPP_LIBS}")
message(STATUS "Found antlr4cpp includes: ${ANTLR4CPP_INCLUDE_DIRS}")

antlr4cpp_process_grammar(
        ${PROJECT_NAME}
        ${PROJECT_NAMESPACE}
        ${CMAKE_CURRENT_SOURCE_DIR}/grammar/S_ONNX.g4
)
message(STATUS "Generated source files: ${generated_source_files}")
message(STATUS "Generated include dirs: ${generated_include_dirs}")

add_executable(sonnxc
        main.cpp
        ${generated_source_files}
        ast/AST.cpp
        error_listener/LexicalErrorListener.cpp
        error_listener/ParserErrorListener.cpp
        error_listener/ParserErrorStrategy.cpp
        visitor/ASTConstructionVisitor.cpp
        utils/Literal2Cpp.cpp
        visitor/ASTBaseVisitor.cpp
        visitor/ASTOutputVisitor.cpp
        visitor/ASTSemanticVisitor.cpp
        utils/SymbolTable.cpp
        utils/SymbolTable.hpp)
add_dependencies(sonnxc
        antlr4cpp
        antlr4cpp_generation_${PROJECT_NAMESPACE})
target_include_directories(sonnxc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${generated_include_dirs})
target_link_libraries(sonnxc antlr4-runtime)
