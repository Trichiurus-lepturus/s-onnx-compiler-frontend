#include "ASTConstructionVisitor.hpp"
#include "ast/AST.hpp"
#include "utils/Literal2Cpp.hpp"
#include <memory>
#include <utility>

namespace sonnx
{

#ifdef DEBUG_AST_CONSTRUCTION
std::string nodeToString(const ASTNode *node)
{
    auto type = node->getASTNodeType();
    std::string type_string{}, value_string{};
    switch (type)
    {
    case NodeType::MODEL: {
        type_string = "MODEL";
        break;
    }
    case NodeType::NODE_LIST: {
        type_string = "NODE_LIST";
        break;
    }
    case NodeType::INPUT_LIST: {
        type_string = "INPUT_LIST";
        break;
    }
    case NodeType::OUTPUT_LIST: {
        type_string = "OUTPUT_LIST";
        break;
    }
    case NodeType::INITIALIZER_LIST: {
        type_string = "INITIALIZER_LIST";
        break;
    }
    case NodeType::NODE: {
        type_string = "NODE";
        break;
    }
    case NodeType::INPUT_ARR: {
        type_string = "INPUT_ARR";
        break;
    }
    case NodeType::OUTPUT_ARR: {
        type_string = "OUTPUT_ARR";
        break;
    }
    case NodeType::ATTRIBUTE_LIST: {
        type_string = "ATTRIBUTE_LIST";
        break;
    }
    case NodeType::ATTRIBUTE: {
        type_string = "ATTRIBUTE";
        break;
    }
    case NodeType::IO_TENSOR: {
        type_string = "IO_TENSOR";
        break;
    }
    case NodeType::IO_SHAPE: {
        type_string = "IO_SHAPE";
        break;
    }
    case NodeType::IO_DIM: {
        type_string = "IO_DIM";
        break;
    }
    case NodeType::INIT_TENSOR: {
        type_string = "INIT_TENSOR";
        break;
    }
    case NodeType::INIT_SHAPE: {
        type_string = "INIT_SHAPE";
        break;
    }
    case NodeType::U32_LITERAL: {
        type_string = "U32_LITERAL";
        value_string = " = " + std::to_string(dynamic_cast<const U32LiteralNode *>(node)->getValue());
        break;
    }
    case NodeType::U64_LITERAL: {
        type_string = "U64_LITERAL";
        value_string = " = " + std::to_string(dynamic_cast<const U64LiteralNode *>(node)->getValue());
        break;
    }
    case NodeType::STR_LITERAL: {
        type_string = "STR_LITERAL";
        value_string = " = \"" + dynamic_cast<const StrLiteralNode *>(node)->getValue() + "\"";
        break;
    }
    case NodeType::BYTES_LITERAL: {
        type_string = "BYTES_LITERAL";
        break;
    }
    case NodeType::TYPE_ENUM: {
        type_string = "TYPE_ENUM";
        break;
    }
    case NodeType::ERROR_NODE: {
        type_string = "ERROR_NODE";
        break;
    }
    default: {
        type_string = "UNKNOWN_NODE_TYPE";
        break;
    }
    }
    return type_string + value_string;
}

void ASTConstructionVisitor::printStack() const
{
    auto stackOperation = [this]() {
        auto &mutableStack = const_cast<std::stack<std::unique_ptr<ASTNode>> &>(this->stack_);
        std::vector<std::unique_ptr<ASTNode>> temp;
        temp.reserve(mutableStack.size());

        while (!mutableStack.empty())
        {
            auto top = std::move(mutableStack.top());
            mutableStack.pop();
            temp.emplace_back(std::move(top));
        }

        return temp;
    };

    std::cout << "───── Stack Trace (size=" << stack_.size() << ") ─────\n";
    if (stack_.empty())
    {
        std::cout << "  ╰─ [Empty]\n";
        return;
    }

    auto temp = stackOperation();

    constexpr bool kShowMemoryAddress = false;
    for (auto it = temp.rbegin(); it != temp.rend(); ++it)
    {
        std::cout << "  ╰─ " << nodeToString(it->get());
        if constexpr (kShowMemoryAddress)
        {
            std::cout << " [" << it->get() << "]";
        }
        std::cout << '\n';
    }

    auto restoreStack = [this, temp = std::move(temp)]() mutable {
        auto &mutableStack = const_cast<std::stack<std::unique_ptr<ASTNode>> &>(this->stack_);
        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
        {
            mutableStack.push(std::move(*it));
        }
    };
    restoreStack();

    std::cout << "─────────────────────────────\n";
}
#endif

void ASTConstructionVisitor::reportError(const antlr4::ParserRuleContext *ctx, const std::string &message)
{
    if (ctx != nullptr && ctx->getStart() != nullptr)
    {
        auto token = ctx->getStart();
        std::ostringstream errorMsg;
        errorMsg << "FATAL AST construction error at line " << token->getLine() << ", column "
                 << (token->getCharPositionInLine() + 1) << ": " << message;
        throw antlr4::ParseCancellationException(errorMsg.str());
    }
    else
    {
        throw antlr4::ParseCancellationException("AST construction error: " + message);
    }
}

template <typename CtxType> void ASTConstructionVisitor::processU32U64(CtxType ctx)
{
    try
    {
        auto *terminal_node = ctx->INTEGER_LITERAL();
        if (terminal_node == nullptr)
        {
            reportError(ctx, "Missing integer literal");
            return;
        }

        if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            reportError(ctx, "Invalid integer literal");
            return;
        }

        auto *token = terminal_node->getSymbol();
        auto integer = Literal2Cpp::integerLiteral2CppInteger(token->getText());
        if (std::holds_alternative<uint32_t>(integer))
        {
            stack_.push(std::make_unique<U32LiteralNode>(std::get<uint32_t>(integer)));
        }
        else
        {
            stack_.push(std::make_unique<U64LiteralNode>(std::get<uint64_t>(integer)));
        }
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to parse integer value: ") + e.what());
    }
}

template <typename CtxType> void ASTConstructionVisitor::processString(CtxType ctx)
{
    try
    {
        auto *terminal_node = ctx->STRING_LITERAL();
        if (terminal_node == nullptr)
        {
            reportError(ctx, "Missing string literal");
            return;
        }

        if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            reportError(ctx, "Invalid string literal");
            return;
        }

        auto *token = terminal_node->getSymbol();
        auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
        stack_.push(std::make_unique<StrLiteralNode>(string));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to parse string value: ") + e.what());
    }
}

template <typename CtxType> void ASTConstructionVisitor::processTypeEnum(CtxType ctx)
{
    try
    {
        DataType type{};
        if (ctx->INT() != nullptr)
        {
            type = DataType::INT;
        }
        else if (ctx->FLOAT() != nullptr)
        {
            type = DataType::FLOAT;
        }
        else if (ctx->STRING() != nullptr)
        {
            type = DataType::STRING;
        }
        else if (ctx->BOOL() != nullptr)
        {
            type = DataType::BOOL;
        }
        else
        {
            reportError(ctx, "Unknown or invalid data type");
            return;
        }
        stack_.push(std::make_unique<TypeEnumNode>(type));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to process type enum: ") + e.what());
    }
}

std::any ASTConstructionVisitor::visitModel(antlr_sonnx::S_ONNXParser::ModelContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Model" << std::endl;
    printStack();
#endif
    try
    {
        if (stack_.size() < (has_initializer_list_ ? 13 : 12))
        {
            reportError(ctx, "Insufficient elements on stack for model construction");
            return nullptr;
        }

        auto opset_version = std::move(stack_.top());
        stack_.pop();
        auto opset_domain = std::move(stack_.top());
        stack_.pop();
        auto initializer_list = has_initializer_list_ ? std::move(stack_.top()) : nullptr;
        if (has_initializer_list_)
        {
            stack_.pop();
            has_initializer_list_ = false;
        }
        auto output_list = std::move(stack_.top());
        stack_.pop();
        auto input_list = std::move(stack_.top());
        stack_.pop();
        auto node_list = std::move(stack_.top());
        stack_.pop();
        auto graph_name = std::move(stack_.top());
        stack_.pop();
        auto doc_string = std::move(stack_.top());
        stack_.pop();
        auto model_version = std::move(stack_.top());
        stack_.pop();
        auto model_domain = std::move(stack_.top());
        stack_.pop();
        auto producer_version = std::move(stack_.top());
        stack_.pop();
        auto producer_name = std::move(stack_.top());
        stack_.pop();
        auto ir_version = std::move(stack_.top());
        stack_.pop();

        auto model = std::make_unique<ModelNode>(
            std::move(ir_version), std::move(producer_name), std::move(producer_version), std::move(model_domain),
            std::move(model_version), std::move(doc_string), std::move(graph_name), std::move(node_list),
            std::move(input_list), std::move(output_list), std::move(initializer_list), std::move(opset_domain),
            std::move(opset_version));
        stack_.push(std::move(model));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct model: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitModel_body_def(antlr_sonnx::S_ONNXParser::Model_body_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitIr_version_def(antlr_sonnx::S_ONNXParser::Ir_version_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Ir_version_def" << std::endl;
    printStack();
#endif
    processU32U64(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitProducer_name_def(antlr_sonnx::S_ONNXParser::Producer_name_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Producer_name_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitProducer_version_def(antlr_sonnx::S_ONNXParser::Producer_version_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Producer_version_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitDomain_def(antlr_sonnx::S_ONNXParser::Domain_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Domain_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitModel_version_def(antlr_sonnx::S_ONNXParser::Model_version_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Model_version_def" << std::endl;
    printStack();
#endif
    processU32U64(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitDoc_string_def(antlr_sonnx::S_ONNXParser::Doc_string_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Doc_string_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitGraph_def(antlr_sonnx::S_ONNXParser::Graph_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitGraph_body_def(antlr_sonnx::S_ONNXParser::Graph_body_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitName_def(antlr_sonnx::S_ONNXParser::Name_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Name_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitNode_list(antlr_sonnx::S_ONNXParser::Node_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Node_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> nodes{};
        nodes.reserve(ctx->NODE().size());

        if (stack_.size() < ctx->NODE().size())
        {
            reportError(ctx, "Insufficient nodes on stack for node list construction");
            return nullptr;
        }

        for (size_t i = 0; i < ctx->NODE().size(); ++i)
        {
            nodes.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        // Reverse to maintain correct order
        std::reverse(nodes.begin(), nodes.end());

        auto node_list = std::make_unique<NodeListNode>(std::move(nodes));
        stack_.push(std::move(node_list));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct node list: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitInput_list(antlr_sonnx::S_ONNXParser::Input_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Input_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> inputs{};
        inputs.reserve(ctx->INPUT().size());

        if (stack_.size() < ctx->INPUT().size())
        {
            reportError(ctx, "Insufficient elements on stack for input list construction");
            return nullptr;
        }

        for (size_t i = 0; i < ctx->INPUT().size(); ++i)
        {
            inputs.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        std::reverse(inputs.begin(), inputs.end());

        auto input_list = std::make_unique<InputListNode>(std::move(inputs));
        stack_.push(std::move(input_list));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct input list: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitOutput_list(antlr_sonnx::S_ONNXParser::Output_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Output_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> outputs{};
        outputs.reserve(ctx->OUTPUT().size());

        if (stack_.size() < ctx->OUTPUT().size())
        {
            reportError(ctx, "Insufficient elements on stack for output list construction");
            return nullptr;
        }

        for (size_t i = 0; i < ctx->OUTPUT().size(); ++i)
        {
            outputs.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        std::reverse(outputs.begin(), outputs.end());

        auto output_list = std::make_unique<OutputListNode>(std::move(outputs));
        stack_.push(std::move(output_list));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct output list: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitInitializer_list(antlr_sonnx::S_ONNXParser::Initializer_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Initializer_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> initializers{};
        initializers.reserve(ctx->INITIALIZER().size());

        if (stack_.size() < ctx->INITIALIZER().size())
        {
            reportError(ctx, "Insufficient elements on stack for initializer list construction");
            return nullptr;
        }

        for (size_t i = 0; i < ctx->INITIALIZER().size(); ++i)
        {
            initializers.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        std::reverse(initializers.begin(), initializers.end());

        auto initializer_list = std::make_unique<InitializerListNode>(std::move(initializers));
        stack_.push(std::move(initializer_list));
        has_initializer_list_ = true;
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct initializer list: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitNode_def(antlr_sonnx::S_ONNXParser::Node_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Node_def" << std::endl;
    printStack();
#endif
    try
    {
        if (stack_.size() < (has_attribute_list_ ? 5 : 4))
        {
            reportError(ctx, "Insufficient elements on stack for node construction");
            return nullptr;
        }

        auto attribute_list = has_attribute_list_ ? std::move(stack_.top()) : nullptr;
        if (has_attribute_list_)
        {
            stack_.pop();
            has_attribute_list_ = false;
        }
        auto output = std::move(stack_.top());
        stack_.pop();
        auto input = std::move(stack_.top());
        stack_.pop();
        auto name = std::move(stack_.top());
        stack_.pop();
        auto op_type = std::move(stack_.top());
        stack_.pop();

        auto node = std::make_unique<NodeNode>(std::move(op_type), std::move(name), std::move(input), std::move(output),
                                               std::move(attribute_list));
        stack_.push(std::move(node));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct node: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitOp_type_def(antlr_sonnx::S_ONNXParser::Op_type_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Op_type_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitInput_arr(antlr_sonnx::S_ONNXParser::Input_arrContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Input_arr" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> input_elements{};
        input_elements.reserve(ctx->STRING_LITERAL().size());
        for (auto *elem : ctx->STRING_LITERAL())
        {
            auto *token = elem->getSymbol();
            auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
            input_elements.push_back(std::make_unique<StrLiteralNode>(string));
        }
        auto input_array = std::make_unique<InputArrNode>(std::move(input_elements));
        stack_.push(std::move(input_array));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct input array: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitOutput_arr(antlr_sonnx::S_ONNXParser::Output_arrContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Output_arr" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> output_elements{};
        output_elements.reserve(ctx->STRING_LITERAL().size());
        for (auto *elem : ctx->STRING_LITERAL())
        {
            auto *token = elem->getSymbol();
            auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
            output_elements.push_back(std::make_unique<StrLiteralNode>(string));
        }
        auto input_array = std::make_unique<OutputArrNode>(std::move(output_elements));
        stack_.push(std::move(input_array));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct output array: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitAttribute_list(antlr_sonnx::S_ONNXParser::Attribute_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Attribute_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> attributes{};
        attributes.reserve(ctx->ATTRIBUTE().size());

        if (stack_.size() < ctx->ATTRIBUTE().size())
        {
            reportError(ctx, "Insufficient elements on stack for attribute list construction");
            return nullptr;
        }

        for (size_t i = 0; i < ctx->ATTRIBUTE().size(); ++i)
        {
            attributes.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        std::reverse(attributes.begin(), attributes.end());

        auto attribute_list = std::make_unique<AttributeListNode>(std::move(attributes));
        stack_.push(std::move(attribute_list));
        has_attribute_list_ = true;
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct attribute list: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitAttribute_def(antlr_sonnx::S_ONNXParser::Attribute_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Attribute_def" << std::endl;
    printStack();
#endif
    try
    {
        if (stack_.size() < 2)
        {
            reportError(ctx, "Insufficient elements on stack for attribute construction");
            return nullptr;
        }

        auto value = std::move(stack_.top());
        stack_.pop();
        auto name = std::move(stack_.top());
        stack_.pop();

        auto attribute = std::make_unique<AttributeNode>(std::move(name), std::move(value));
        stack_.push(std::move(attribute));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct attribute: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitValue_def(antlr_sonnx::S_ONNXParser::Value_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Value_def" << std::endl;
    printStack();
#endif
    processString(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitValue_info_def(antlr_sonnx::S_ONNXParser::Value_info_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Value_info_def" << std::endl;
    printStack();
#endif
    try
    {
        if (stack_.size() < 3)
        {
            reportError(ctx, "Insufficient elements on stack for value info construction");
            return nullptr;
        }

        auto shape = std::move(stack_.top());
        stack_.pop();
        auto type = std::move(stack_.top());
        stack_.pop();
        auto name = std::move(stack_.top());
        stack_.pop();

        auto io_tensor = std::make_unique<IOTensorNode>(std::move(name), std::move(type), std::move(shape));
        stack_.push(std::move(io_tensor));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct value info: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitType_def(antlr_sonnx::S_ONNXParser::Type_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitTensor_type_def(antlr_sonnx::S_ONNXParser::Tensor_type_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitElem_type_def(antlr_sonnx::S_ONNXParser::Elem_type_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Elem_type_def" << std::endl;
    printStack();
#endif
    processTypeEnum(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitShape_def(antlr_sonnx::S_ONNXParser::Shape_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitDim_list(antlr_sonnx::S_ONNXParser::Dim_listContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Dim_list" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> io_dims{};
        io_dims.reserve(ctx->DIM().size());
        for (size_t i = 0; i < ctx->DIM().size(); ++i)
        {
            io_dims.push_back(std::move(stack_.top()));
            stack_.pop();
        }
        std::reverse(io_dims.begin(), io_dims.end());
        auto input_list = std::make_unique<IOShapeNode>(std::move(io_dims));
        stack_.push(std::move(input_list));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct shape: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitDim_def(antlr_sonnx::S_ONNXParser::Dim_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Dim_def" << std::endl;
    printStack();
#endif
    try
    {
        if (ctx->INTEGER_LITERAL() != nullptr)
        {
            processU32U64(ctx);
        }
        else if (ctx->STRING_LITERAL() != nullptr)
        {
            processString(ctx);
        }
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct dimension: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitTensor_def(antlr_sonnx::S_ONNXParser::Tensor_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }

#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Tensor_def" << std::endl;
    printStack();
#endif
    try
    {
        if (stack_.size() < 4)
        {
            reportError(ctx, "Insufficient elements on stack for tensor construction");
            return nullptr;
        }

        auto raw_data = std::move(stack_.top());
        stack_.pop();
        auto shape = std::move(stack_.top());
        stack_.pop();
        auto type = std::move(stack_.top());
        stack_.pop();
        auto name = std::move(stack_.top());
        stack_.pop();

        auto tensor =
            std::make_unique<InitTensorNode>(std::move(name), std::move(type), std::move(shape), std::move(raw_data));
        stack_.push(std::move(tensor));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct tensor: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitData_type_def(antlr_sonnx::S_ONNXParser::Data_type_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Data_type_def" << std::endl;
    printStack();
#endif
    processTypeEnum(ctx);
    return nullptr;
}

std::any ASTConstructionVisitor::visitDims_def(antlr_sonnx::S_ONNXParser::Dims_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Dims_def" << std::endl;
    printStack();
#endif
    try
    {
        std::vector<std::unique_ptr<ASTNode>> dim_values{};
        dim_values.reserve(ctx->INTEGER_LITERAL().size());
        for (auto *dim : ctx->INTEGER_LITERAL())
        {
            auto *token = dim->getSymbol();
            try
            {
                auto integer = Literal2Cpp::integerLiteral2CppInteger(token->getText());
                if (std::holds_alternative<uint32_t>(integer))
                {
                    dim_values.push_back(std::make_unique<U32LiteralNode>(std::get<uint32_t>(integer)));
                }
                else
                {
                    dim_values.push_back(std::make_unique<U64LiteralNode>(std::get<uint64_t>(integer)));
                }
            }
            catch (const std::out_of_range &)
            {
                dim_values.push_back(std::make_unique<ErrorNode>());
            }
        }
        auto init_shape = std::make_unique<InitShapeNode>(std::move(dim_values));
        stack_.push(std::move(init_shape));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to construct dims: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitRaw_data_def(antlr_sonnx::S_ONNXParser::Raw_data_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Raw_data_def" << std::endl;
    printStack();
#endif
    try
    {
        auto *terminal_node = ctx->BYTES_LITERAL();
        if (terminal_node == nullptr)
        {
            reportError(ctx, "Missing bytes literal");
            return nullptr;
        }

        if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            reportError(ctx, "Invalid bytes literal");
            return nullptr;
        }

        auto *token = terminal_node->getSymbol();
        auto bytes = Literal2Cpp::bytesLiteral2CppBytes(token->getText());
        stack_.push(std::make_unique<BytesLiteralNode>(std::move(bytes)));
    }
    catch (const std::exception &e)
    {
        reportError(ctx, std::string("Failed to parse bytes literal: ") + e.what());
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitOpset_import_def(antlr_sonnx::S_ONNXParser::Opset_import_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
    return nullptr;
}

std::any ASTConstructionVisitor::visitVersion_def(antlr_sonnx::S_ONNXParser::Version_defContext *ctx)
{
    for (auto child : ctx->children)
    {
        visit(child);
    }
#ifdef DEBUG_AST_CONSTRUCTION
    std::cout << "Visiting Version_def" << std::endl;
    printStack();
#endif
    processU32U64(ctx);
    return nullptr;
}

} // namespace sonnx
