#include "S_ONNXLexer.h"
#include "S_ONNXParser.h"
#include "ast/AST.hpp"
#include "error_listener/LexicalErrorListener.hpp"
#include "error_listener/ParserErrorListener.hpp"
#include "error_listener/ParserErrorStrategy.hpp"
#include "visitor/ASTConstructionVisitor.hpp"
#include "visitor/ASTOutputVisitor.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <visitor/ASTSemanticVisitor.hpp>

auto main(const int argc, char *argv[]) -> int
{
    if (argc != 2)
    {
        std::cerr << "Usage: sonnxc <path-to-model>" << '\n';
        return 1;
    }

    const auto file_stream = std::make_unique<antlr4::ANTLRFileStream>();
    file_stream->loadFromFile(argv[1]);
    auto lexer = std::make_unique<antlr_sonnx::S_ONNXLexer>(file_stream.get());
    lexer->removeErrorListeners(); // Remove default console error listener
    auto lexicalErrorListener = std::make_unique<sonnx::LexicalErrorListener>();
    lexer->addErrorListener(lexicalErrorListener.get());

    try
    {
        // Lexical Analysis
        auto tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
        tokens->fill();
        std::cout << "Lexical analysis completed successfully." << std::endl;
        // Parser Setup and Parsing
        auto parser = std::make_unique<antlr_sonnx::S_ONNXParser>(tokens.get());
        parser->setBuildParseTree(true);
        parser->removeErrorListeners();

        auto parserErrorListener = std::make_unique<sonnx::ParserErrorListener>();
        parser->addErrorListener(parserErrorListener.get());
        parser->setErrorHandler(std::make_unique<sonnx::ParserErrorStrategy>());

        std::cout << "Starting to parse..." << std::endl;
        auto parseTree = parser->model();
        std::cout << "Parsing completed successfully." << std::endl;

        // AST Construction with immediate error termination
        std::cout << "Building AST..." << std::endl;
        try
        {
            auto visitor = std::make_unique<sonnx::ASTConstructionVisitor>();
            visitor->visit(parseTree);
            auto model = visitor->getTop();

            if (!model)
            {
                std::cerr << "FATAL AST construction error: visitor returned null model" << std::endl;
                return 1;
            }

            std::cout << "AST construction completed successfully." << std::endl;

            auto *model_ptr = dynamic_cast<sonnx::ModelNode *>(model.get());
            if (model_ptr != nullptr)
            {
                // First, perform semantic analysis
                auto semantic_visitor = std::make_unique<sonnx::ASTSemanticVisitor>();
                semantic_visitor->visit(*model_ptr);
                // Check for semantic errors
                if (semantic_visitor->hasErrors())
                {
                    std::cerr << "Semantic analysis failed with errors:\n";
                    for (const auto &error : semantic_visitor->getErrors())
                    {
                        std::cerr << "- " << error << '\n';
                    }
                    return 1;
                }
                // If semantic analysis passed, then output the AST
                auto output_visitor = std::make_unique<sonnx::ASTOutputVisitor>();
                output_visitor->visit(*model_ptr);
                std::cout << output_visitor->getResult() << '\n';

                // Optionally, you can also access the built DAG and symbol table
                const auto &symbol_table = semantic_visitor->getSymbolTable();
                if (symbol_table.hasCycle())
                {
                    std::cerr << "Warning: Cycle detected in computation graph\n";
                }
            }
            else
            {
                std::cerr << "Error: Failed to construct valid model AST!" << '\n';
                return 1;
            }
        }
        catch (const antlr4::ParseCancellationException &e)
        {
            std::cerr << e.what() << std::endl; // Prints "FATAL AST construction error..."
            return 1;
        }
    }
    catch (const antlr4::ParseCancellationException &e)
    {
        std::cerr << e.what() << std::endl; // Lexical/Parser errors
        return 1;
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred!" << '\n';
        return 1;
    }

    return 0;
}
