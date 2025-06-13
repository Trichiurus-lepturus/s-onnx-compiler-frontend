#include "ASTConstructionListener.hpp"
#include "ast/AST.hpp"
#include "utils/Literal2Cpp.hpp"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>
#include <variant>

namespace sonnx
{

template <typename CtxType> void ASTConstructionListener::processU32U64(CtxType ctx)
{
    auto *terminal_node = ctx->INTEGER_LITERAL();
    if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    try
    {
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
    catch (const std::out_of_range &)
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
}

template <typename CtxType> void ASTConstructionListener::processString(CtxType ctx)
{
    auto *terminal_node = ctx->STRING_LITERAL();
    if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
    stack_.push(std::make_unique<StrLiteralNode>(string));
}

template <typename CtxType> void ASTConstructionListener::processTypeEnum(CtxType ctx)
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
        stack_.push(std::make_unique<ErrorNode>());
        return;
    }
    stack_.push(std::make_unique<TypeEnumNode>(type));
}

void ASTConstructionListener::visitErrorNode(antlr4::tree::ErrorNode *node)
{
}

void ASTConstructionListener::exitModel(antlr_sonnx::S_ONNXParser::ModelContext *ctx)
{
    auto opset_version = std::move(stack_.top());
    stack_.pop();
    auto opset_domain = std::move(stack_.top());
    stack_.pop();
    std::unique_ptr<ASTNode> initializer_list = nullptr;
    if (has_attribute_list_)
    {
        initializer_list = std::move(stack_.top());
        stack_.pop();
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

void ASTConstructionListener::exitIr_version_def(antlr_sonnx::S_ONNXParser::Ir_version_defContext *ctx)
{
    processU32U64(ctx);
}

void ASTConstructionListener::exitProducer_name_def(antlr_sonnx::S_ONNXParser::Producer_name_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitProducer_version_def(antlr_sonnx::S_ONNXParser::Producer_version_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitDomain_def(antlr_sonnx::S_ONNXParser::Domain_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitModel_version_def(antlr_sonnx::S_ONNXParser::Model_version_defContext *ctx)
{
    processU32U64(ctx);
}

void ASTConstructionListener::exitDoc_string_def(antlr_sonnx::S_ONNXParser::Doc_string_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitName_def(antlr_sonnx::S_ONNXParser::Name_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitNode_list(antlr_sonnx::S_ONNXParser::Node_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> nodes{};
    nodes.reserve(ctx->NODE().size());
    for (size_t i = 0; i < ctx->NODE().size(); ++i)
    {
        nodes.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto node_list = std::make_unique<NodeListNode>(std::move(nodes));
    stack_.push(std::move(node_list));
}

void ASTConstructionListener::exitInput_list(antlr_sonnx::S_ONNXParser::Input_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> inputs{};
    inputs.reserve(ctx->INPUT().size());
    for (size_t i = 0; i < ctx->INPUT().size(); ++i)
    {
        inputs.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto input_list = std::make_unique<InputListNode>(std::move(inputs));
    stack_.push(std::move(input_list));
}

void ASTConstructionListener::exitOutput_list(antlr_sonnx::S_ONNXParser::Output_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> outputs{};
    outputs.reserve(ctx->OUTPUT().size());
    for (size_t i = 0; i < ctx->OUTPUT().size(); ++i)
    {
        outputs.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto output_list = std::make_unique<OutputListNode>(std::move(outputs));
    stack_.push(std::move(output_list));
}

void ASTConstructionListener::exitInitializer_list(antlr_sonnx::S_ONNXParser::Initializer_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> tensors{};
    tensors.reserve(ctx->INITIALIZER().size());
    for (size_t i = 0; i < ctx->INITIALIZER().size(); ++i)
    {
        tensors.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto initializer_list = std::make_unique<InitializerListNode>(std::move(tensors));
    stack_.push(std::move(initializer_list));
}

void ASTConstructionListener::exitNode_def(antlr_sonnx::S_ONNXParser::Node_defContext *ctx)
{
    std::unique_ptr<ASTNode> attribute_list = nullptr;
    if (ctx->attribute_list() != nullptr)
    {
        attribute_list = std::move(stack_.top());
        stack_.pop();
    }
    auto output_list_or_array = std::move(stack_.top());
    stack_.pop();
    auto input_list_or_array = std::move(stack_.top());
    stack_.pop();
    auto name = std::move(stack_.top());
    stack_.pop();
    auto op_type = std::move(stack_.top());
    stack_.pop();

    auto node = std::make_unique<NodeNode>(std::move(op_type), std::move(name), std::move(input_list_or_array),
                                           std::move(output_list_or_array), std::move(attribute_list));
    stack_.push(std::move(node));
}

void ASTConstructionListener::exitOp_type_def(antlr_sonnx::S_ONNXParser::Op_type_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitInput_arr(antlr_sonnx::S_ONNXParser::Input_arrContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> input_elements{};
    input_elements.reserve(ctx->STRING_LITERAL().size());
    for (auto *elem : ctx->STRING_LITERAL())
    {
        if (elem->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            input_elements.push_back(std::make_unique<ErrorNode>());
        }
        auto *token = elem->getSymbol();
        auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
        input_elements.push_back(std::make_unique<StrLiteralNode>(string));
    }
    auto input_array = std::make_unique<InputArrNode>(std::move(input_elements));
    stack_.push(std::move(input_array));
}

void ASTConstructionListener::exitOutput_arr(antlr_sonnx::S_ONNXParser::Output_arrContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> output_elements{};
    output_elements.reserve(ctx->STRING_LITERAL().size());
    for (auto *elem : ctx->STRING_LITERAL())
    {
        if (elem->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            output_elements.push_back(std::make_unique<ErrorNode>());
        }
        auto *token = elem->getSymbol();
        auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
        output_elements.push_back(std::make_unique<StrLiteralNode>(string));
    }
    auto input_array = std::make_unique<OutputArrNode>(std::move(output_elements));
    stack_.push(std::move(input_array));
}

void ASTConstructionListener::exitAttribute_list(antlr_sonnx::S_ONNXParser::Attribute_listContext *ctx)
{
    has_attribute_list_ = true;
    std::vector<std::unique_ptr<ASTNode>> attributes{};
    attributes.reserve(ctx->ATTRIBUTE().size());
    for (size_t i = 0; i < ctx->ATTRIBUTE().size(); ++i)
    {
        attributes.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto attribute_list = std::make_unique<AttributeListNode>(std::move(attributes));
    stack_.push(std::move(attribute_list));
}

void ASTConstructionListener::exitAttribute_def(antlr_sonnx::S_ONNXParser::Attribute_defContext * /*ctx*/)
{
    auto value = std::move(stack_.top());
    stack_.pop();
    auto name = std::move(stack_.top());
    stack_.pop();

    auto attribute = std::make_unique<AttributeNode>(std::move(name), std::move(value));
    stack_.push(std::move(attribute));
}

void ASTConstructionListener::exitValue_def(antlr_sonnx::S_ONNXParser::Value_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitValue_info_def(antlr_sonnx::S_ONNXParser::Value_info_defContext * /*ctx*/)
{
    auto io_shape = std::move(stack_.top());
    stack_.pop();
    auto type = std::move(stack_.top());
    stack_.pop();
    auto name = std::move(stack_.top());
    stack_.pop();

    auto io_tensor = std::make_unique<IOTensorNode>(std::move(name), std::move(type), std::move(io_shape));
    stack_.push(std::move(io_tensor));
}

void ASTConstructionListener::exitElem_type_def(antlr_sonnx::S_ONNXParser::Elem_type_defContext *ctx)
{
    processTypeEnum(ctx);
}

void ASTConstructionListener::exitDim_list(antlr_sonnx::S_ONNXParser::Dim_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> io_dims{};
    io_dims.reserve(ctx->DIM().size());
    for (size_t i = 0; i < ctx->DIM().size(); ++i)
    {
        io_dims.push_back(std::move(stack_.top()));
        stack_.pop();
    }
    auto input_list = std::make_unique<IOShapeNode>(std::move(io_dims));
    stack_.push(std::move(input_list));
}

void ASTConstructionListener::exitDim_def(antlr_sonnx::S_ONNXParser::Dim_defContext *ctx)
{
    if (ctx->INTEGER_LITERAL() != nullptr)
    {
        processU32U64(ctx);
    }
    else if (ctx->STRING_LITERAL() != nullptr)
    {
        processString(ctx);
    }
    else
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
}

void ASTConstructionListener::exitTensor_def(antlr_sonnx::S_ONNXParser::Tensor_defContext * /*ctx*/)
{
    auto raw_data = std::move(stack_.top());
    stack_.pop();
    auto shape = std::move(stack_.top());
    stack_.pop();
    auto data_type = std::move(stack_.top());
    stack_.pop();
    auto name = std::move(stack_.top());
    stack_.pop();

    auto init_tensor =
        std::make_unique<InitTensorNode>(std::move(name), std::move(data_type), std::move(shape), std::move(raw_data));
    stack_.push(std::move(init_tensor));
}

void ASTConstructionListener::exitData_type_def(antlr_sonnx::S_ONNXParser::Data_type_defContext *ctx)
{
    processTypeEnum(ctx);
}

void ASTConstructionListener::exitDims_def(antlr_sonnx::S_ONNXParser::Dims_defContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> dim_values{};
    dim_values.reserve(ctx->INTEGER_LITERAL().size());
    for (auto *dim : ctx->INTEGER_LITERAL())
    {
        if (dim->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            dim_values.push_back(std::make_unique<ErrorNode>());
        }
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

void ASTConstructionListener::exitRaw_data_def(antlr_sonnx::S_ONNXParser::Raw_data_defContext *ctx)
{
    auto *terminal_node = ctx->BYTES_LITERAL();
    if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    try
    {
        auto bytes = Literal2Cpp::bytesLiteral2CppBytes(token->getText());
        stack_.push(std::make_unique<BytesLiteralNode>(std::move(bytes)));
    }
    catch (const std::invalid_argument &)
    {
        stack_.push(std::make_unique<ErrorNode>());
    }
}

void ASTConstructionListener::exitVersion_def(antlr_sonnx::S_ONNXParser::Version_defContext *ctx)
{
    processU32U64(ctx);
}

} // namespace sonnx
