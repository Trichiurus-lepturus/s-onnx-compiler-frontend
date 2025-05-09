#include "S_ONNXLexer.h"
#include "S_ONNXParser.h"
#include "ast/AST.hpp"
#include "listener/ASTConstructionListener.hpp"
#include "visitor/ASTOutputVisitor.hpp"
#include <iostream>
#include <memory>

void findErrorNodes(antlr4::tree::ParseTree* tree) {
    // Check if this node is an error node
    if (dynamic_cast<antlr4::tree::ErrorNode*>(tree) != nullptr) {
        std::cout << "Error node found!" << std::endl;
    }

    // Check children
    for (size_t i = 0; i < tree->children.size(); i++) {
        findErrorNodes(tree->children[i]);
    }
}

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
    auto tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
    auto parser = std::make_unique<antlr_sonnx::S_ONNXParser>(tokens.get());
    parser->setBuildParseTree(false);
    parser->setErrorHandler(std::make_unique<antlr4::DefaultErrorStrategy>());
    auto listener = std::make_unique<sonnx::ASTConstructionListener>();
    parser->addParseListener(listener.get());
    auto model_from_parser = parser->model();
    // // Then after parsing:
    // findErrorNodes(model_from_parser);
    auto model = listener->getTop();
    auto *model_ptr = dynamic_cast<sonnx::ModelNode *>(model.get());
    if (model_ptr != nullptr)
    {
        auto visitor = std::make_unique<sonnx::ASTOutputVisitor>();
        visitor->visit(*model_ptr);
        std::cout << visitor->getResult() << '\n';
    }
    else
    {
        std::cerr << "This is not a model!" << '\n';
    }
    return 0;
}
