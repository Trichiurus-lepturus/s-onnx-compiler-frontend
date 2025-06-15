#include "S_ONNXLexer.h"
#include "S_ONNXParser.h"
#include "ast/AST.hpp"
#include "error_listener/LexicalErrorListener.hpp"
#include "error_listener/ParserErrorListener.hpp"
#include "error_listener/ParserErrorStrategy.hpp"
#include "visitor/ASTConstructionVisitor.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <visitor/ASTSemanticVisitor.hpp>

// #define OUTPUT_AST

#ifdef OUTPUT_AST
#include "visitor/ASTOutputVisitor.hpp"
#endif

auto main(const int argc, char *argv[]) -> int
{
    if (argc != 2)
    {
        std::cerr << "Usage: sonnxc <path-to-model>" << '\n';
        return 1;
    }

    try
    {
        const auto file_stream = std::make_unique<antlr4::ANTLRFileStream>();
        file_stream->loadFromFile(argv[1]);
        auto lexer = std::make_unique<antlr_sonnx::S_ONNXLexer>(file_stream.get());
        lexer->removeErrorListeners();
        auto lexicalErrorListener = std::make_unique<sonnx::LexicalErrorListener>();
        lexer->addErrorListener(lexicalErrorListener.get());
        auto token_stream = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
        token_stream->fill();

        auto parser = std::make_unique<antlr_sonnx::S_ONNXParser>(token_stream.get());
        parser->removeErrorListeners();
        auto parserErrorListener = std::make_unique<sonnx::ParserErrorListener>();
        auto errorStrategy = std::make_unique<sonnx::ParserErrorStrategy>();
        parser->addErrorListener(parserErrorListener.get());
        parser->setErrorHandler(std::move(errorStrategy));

        auto parseTree = parser->model();
        auto visitor = std::make_unique<sonnx::ASTConstructionVisitor>();
        visitor->visit(parseTree);
        auto model = visitor->getTop();

        if (!model)
        {
            std::cerr << "FATAL AST construction error: visitor returned null model" << std::endl;
            return 1;
        }

        auto *model_ptr = dynamic_cast<sonnx::ModelNode *>(model.get());
        if (!model_ptr)
        {
            std::cerr << "Error: Failed to cast to ModelNode!" << '\n';
            return 1;
        }

#ifdef OUTPUT_AST
        auto ast_output_visitor = std::make_unique<sonnx::ASTOutputVisitor>();
        ast_output_visitor->visit(*model_ptr);
        std::cout << ast_output_visitor->getResult() << std::endl;
#endif

        auto semantic_visitor = std::make_unique<sonnx::ASTSemanticVisitor>();
        semantic_visitor->visit(*model_ptr);

        if (semantic_visitor->hasErrors())
        {
            std::cerr << "Semantic analysis failed with errors:\n";
            for (const auto &error : semantic_visitor->getErrors())
            {
                std::cerr << "- " << error << '\n';
            }
            return 1;
        }

        const auto &symbol_table = semantic_visitor->getSymbolTable();
        if (symbol_table.hasCycle())
        {
            std::cerr << "Warning: Cycle detected in computation graph\n";
        }

        std::cout << symbol_table.generateTACode() << std::endl;
        return 0;
    }
    catch (const antlr4::ParseCancellationException &e)
    {
        std::cerr << "Compilation aborted: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
