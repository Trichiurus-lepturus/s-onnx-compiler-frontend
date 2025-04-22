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
        m_stack.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    try
    {
        auto integer = Literal2Cpp::integerLiteral2CppInteger(token->getText());
        if (std::holds_alternative<uint32_t>(integer))
        {
            m_stack.push(std::make_unique<U32LiteralNode>(std::get<uint32_t>(integer)));
        }
        else
        {
            m_stack.push(std::make_unique<U64LiteralNode>(std::get<uint64_t>(integer)));
        }
    }
    catch (const std::out_of_range &)
    {
        m_stack.push(std::make_unique<ErrorNode>());
    }
}

template <typename CtxType> void ASTConstructionListener::processString(CtxType ctx)
{
    auto *terminal_node = ctx->STRING_LITERAL();
    if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
    {
        m_stack.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
    m_stack.push(std::make_unique<StrLiteralNode>(string));
}

template <typename CtxType> void ASTConstructionListener::processTypeEnum(CtxType ctx)
{
    ValueOrDataType type{};
    if (ctx->INT() != nullptr)
    {
        type = ValueOrDataType::INT;
    }
    else if (ctx->FLOAT() != nullptr)
    {
        type = ValueOrDataType::FLOAT;
    }
    else if (ctx->STRING() != nullptr)
    {
        type = ValueOrDataType::STRING;
    }
    else if (ctx->BOOL() != nullptr)
    {
        type = ValueOrDataType::BOOL;
    }
    else
    {
        m_stack.push(std::make_unique<ErrorNode>());
        return;
    }
    m_stack.push(std::make_unique<TypeEnumNode>(type));
}

void ASTConstructionListener::visitErrorNode(antlr4::tree::ErrorNode *node)
{

}

void ASTConstructionListener::exitModel_body_def(antlr_sonnx::S_ONNXParser::Model_body_defContext * /*ctx*/)
{
    auto opset_import = std::move(m_stack.top());
    m_stack.pop();
    auto graph = std::move(m_stack.top());
    m_stack.pop();
    auto doc_string = std::move(m_stack.top());
    m_stack.pop();
    auto model_version = std::move(m_stack.top());
    m_stack.pop();
    auto domain = std::move(m_stack.top());
    m_stack.pop();
    auto producer_version = std::move(m_stack.top());
    m_stack.pop();
    auto producer_name = std::move(m_stack.top());
    m_stack.pop();
    auto ir_version = std::move(m_stack.top());
    m_stack.pop();

    auto model = std::make_unique<ModelNode>(std::move(ir_version), std::move(producer_name),
                                             std::move(producer_version), std::move(domain), std::move(model_version),
                                             std::move(doc_string), std::move(graph), std::move(opset_import));
    m_stack.push(std::move(model));
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

void ASTConstructionListener::exitGraph_body_def(antlr_sonnx::S_ONNXParser::Graph_body_defContext *ctx)
{
    std::unique_ptr<ASTNode> initializer_list = nullptr;
    if (ctx->initializer_list() != nullptr)
    {
        initializer_list = std::move(m_stack.top());
        m_stack.pop();
    }
    auto output_list = std::move(m_stack.top());
    m_stack.pop();
    auto input_list = std::move(m_stack.top());
    m_stack.pop();
    auto node_list = std::move(m_stack.top());
    m_stack.pop();
    auto name = std::move(m_stack.top());
    m_stack.pop();

    auto graph = std::make_unique<GraphNode>(std::move(name), std::move(node_list), std::move(input_list),
                                             std::move(output_list), std::move(initializer_list));
    m_stack.push(std::move(graph));
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
        nodes.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto node_list = std::make_unique<NodeListNode>(std::move(nodes));
    m_stack.push(std::move(node_list));
}

void ASTConstructionListener::exitInput_list(antlr_sonnx::S_ONNXParser::Input_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> inputs{};
    inputs.reserve(ctx->INPUT().size());
    for (size_t i = 0; i < ctx->INPUT().size(); ++i)
    {
        inputs.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto input_list = std::make_unique<InputListNode>(std::move(inputs));
    m_stack.push(std::move(input_list));
}

void ASTConstructionListener::exitOutput_list(antlr_sonnx::S_ONNXParser::Output_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> outputs{};
    outputs.reserve(ctx->OUTPUT().size());
    for (size_t i = 0; i < ctx->OUTPUT().size(); ++i)
    {
        outputs.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto output_list = std::make_unique<OutputListNode>(std::move(outputs));
    m_stack.push(std::move(output_list));
}

void ASTConstructionListener::exitInitializer_list(antlr_sonnx::S_ONNXParser::Initializer_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> tensors{};
    tensors.reserve(ctx->INITIALIZER().size());
    for (size_t i = 0; i < ctx->INITIALIZER().size(); ++i)
    {
        tensors.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto initializer_list = std::make_unique<InitializerListNode>(std::move(tensors));
    m_stack.push(std::move(initializer_list));
}

void ASTConstructionListener::exitNode_def(antlr_sonnx::S_ONNXParser::Node_defContext *ctx)
{
    std::unique_ptr<ASTNode> attribute_list = nullptr;
    if (ctx->attribute_list() != nullptr)
    {
        attribute_list = std::move(m_stack.top());
        m_stack.pop();
    }
    auto output_list_or_array = std::move(m_stack.top());
    m_stack.pop();
    auto inputs_list_or_array = std::move(m_stack.top());
    m_stack.pop();
    auto name = std::move(m_stack.top());
    m_stack.pop();
    auto op_type = std::move(m_stack.top());
    m_stack.pop();

    auto node = std::make_unique<NodeNode>(std::move(op_type), std::move(name), std::move(inputs_list_or_array),
                                           std::move(output_list_or_array), std::move(attribute_list));
    m_stack.push(std::move(node));
}

void ASTConstructionListener::exitOp_type_def(antlr_sonnx::S_ONNXParser::Op_type_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitInput_arr(antlr_sonnx::S_ONNXParser::Input_arrContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> inputs{};
    inputs.reserve(ctx->STRING_LITERAL().size());
    for (auto *input : ctx->STRING_LITERAL())
    {
        if (input->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            inputs.push_back(std::make_unique<ErrorNode>());
        }
        auto *token = input->getSymbol();
        auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
        inputs.push_back(std::make_unique<StrLiteralNode>(string));
    }
    auto input_array = std::make_unique<InputArrNode>(std::move(inputs));
    m_stack.push(std::move(input_array));
}

void ASTConstructionListener::exitOutput_arr(antlr_sonnx::S_ONNXParser::Output_arrContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> outputs{};
    outputs.reserve(ctx->STRING_LITERAL().size());
    for (auto *output : ctx->STRING_LITERAL())
    {
        if (output->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            outputs.push_back(std::make_unique<ErrorNode>());
        }
        auto *token = output->getSymbol();
        auto string = Literal2Cpp::stringLiteral2CppString(token->getText());
        outputs.push_back(std::make_unique<StrLiteralNode>(string));
    }
    auto input_array = std::make_unique<OutputArrNode>(std::move(outputs));
    m_stack.push(std::move(input_array));
}

void ASTConstructionListener::exitAttribute_list(antlr_sonnx::S_ONNXParser::Attribute_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> attributes{};
    attributes.reserve(ctx->ATTRIBUTE().size());
    for (size_t i = 0; i < ctx->ATTRIBUTE().size(); ++i)
    {
        attributes.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto attribute_list = std::make_unique<AttributeListNode>(std::move(attributes));
    m_stack.push(std::move(attribute_list));
}

void ASTConstructionListener::exitAttribute_def(antlr_sonnx::S_ONNXParser::Attribute_defContext * /*ctx*/)
{
    auto value = std::move(m_stack.top());
    m_stack.pop();
    auto name = std::move(m_stack.top());
    m_stack.pop();

    auto attribute = std::make_unique<AttributeNode>(std::move(name), std::move(value));
    m_stack.push(std::move(attribute));
}

void ASTConstructionListener::exitValue_def(antlr_sonnx::S_ONNXParser::Value_defContext *ctx)
{
    processString(ctx);
}

void ASTConstructionListener::exitValue_info_def(antlr_sonnx::S_ONNXParser::Value_info_defContext * /*ctx*/)
{
    auto tensor_type = std::move(m_stack.top());
    m_stack.pop();
    auto name = std::move(m_stack.top());
    m_stack.pop();

    auto value_info = std::make_unique<ValueInfoNode>(std::move(name), std::move(tensor_type));
    m_stack.push(std::move(value_info));
}

void ASTConstructionListener::exitTensor_type_def(antlr_sonnx::S_ONNXParser::Tensor_type_defContext * /*ctx*/)
{
    auto shape = std::move(m_stack.top());
    m_stack.pop();
    auto elem_type = std::move(m_stack.top());
    m_stack.pop();

    auto tensor_type = std::make_unique<TensorTypeNode>(std::move(elem_type), std::move(shape));
    m_stack.push(std::move(tensor_type));
}

void ASTConstructionListener::exitElem_type_def(antlr_sonnx::S_ONNXParser::Elem_type_defContext *ctx)
{
    processTypeEnum(ctx);
}

void ASTConstructionListener::exitDim_list(antlr_sonnx::S_ONNXParser::Dim_listContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> dims{};
    dims.reserve(ctx->DIM().size());
    for (size_t i = 0; i < ctx->DIM().size(); ++i)
    {
        dims.push_back(std::move(m_stack.top()));
        m_stack.pop();
    }
    auto input_list = std::make_unique<ShapeNode>(std::move(dims));
    m_stack.push(std::move(input_list));
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
        m_stack.push(std::make_unique<ErrorNode>());
    }
}

void ASTConstructionListener::exitTensor_def(antlr_sonnx::S_ONNXParser::Tensor_defContext * /*ctx*/)
{
    auto raw_data = std::move(m_stack.top());
    m_stack.pop();
    auto dims_array = std::move(m_stack.top());
    m_stack.pop();
    auto data_type = std::move(m_stack.top());
    m_stack.pop();
    auto name = std::move(m_stack.top());
    m_stack.pop();

    auto value_info =
        std::make_unique<TensorNode>(std::move(name), std::move(data_type), std::move(dims_array), std::move(raw_data));
    m_stack.push(std::move(value_info));
}

void ASTConstructionListener::exitData_type_def(antlr_sonnx::S_ONNXParser::Data_type_defContext *ctx)
{
    processTypeEnum(ctx);
}

void ASTConstructionListener::exitDims_def(antlr_sonnx::S_ONNXParser::Dims_defContext *ctx)
{
    std::vector<std::unique_ptr<ASTNode>> dims{};
    dims.reserve(ctx->INTEGER_LITERAL().size());
    for (auto *dim : ctx->INTEGER_LITERAL())
    {
        if (dim->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
        {
            dims.push_back(std::make_unique<ErrorNode>());
        }
        auto *token = dim->getSymbol();
        try
        {
            auto integer = Literal2Cpp::integerLiteral2CppInteger(token->getText());
            if (std::holds_alternative<uint32_t>(integer))
            {
                dims.push_back(std::make_unique<U32LiteralNode>(std::get<uint32_t>(integer)));
            }
            else
            {
                dims.push_back(std::make_unique<U64LiteralNode>(std::get<uint64_t>(integer)));
            }
        }
        catch (const std::out_of_range &)
        {
            dims.push_back(std::make_unique<ErrorNode>());
        }
    }
    auto dims_array = std::make_unique<DimsArrayNode>(std::move(dims));
    m_stack.push(std::move(dims_array));
}

void ASTConstructionListener::exitRaw_data_def(antlr_sonnx::S_ONNXParser::Raw_data_defContext *ctx)
{
    auto *terminal_node = ctx->BYTES_LITERAL();
    if (terminal_node->getTreeType() == antlr4::tree::ParseTreeType::ERROR)
    {
        m_stack.push(std::make_unique<ErrorNode>());
    }
    auto *token = terminal_node->getSymbol();
    try
    {
        auto bytes = Literal2Cpp::bytesLiteral2CppBytes(token->getText());
        m_stack.push(std::make_unique<BytesLiteralNode>(std::move(bytes)));
    }
    catch (const std::invalid_argument &)
    {
        m_stack.push(std::make_unique<ErrorNode>());
    }
}

void ASTConstructionListener::exitOpset_import_def(antlr_sonnx::S_ONNXParser::Opset_import_defContext * /*ctx*/)
{
    auto version = std::move(m_stack.top());
    m_stack.pop();
    auto domain = std::move(m_stack.top());
    m_stack.pop();

    auto opset_import = std::make_unique<OpsetImportNode>(std::move(domain), std::move(version));
    m_stack.push(std::move(opset_import));
}

void ASTConstructionListener::exitVersion_def(antlr_sonnx::S_ONNXParser::Version_defContext *ctx)
{
    processU32U64(ctx);
}

} // namespace sonnx
