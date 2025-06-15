#include "ASTOutputVisitor.hpp"
#include "ast/AST.hpp"
#include <cstddef>
#include <string>

namespace sonnx
{

auto DataTypeToString(const DataType type) -> std::string
{
    switch (type)
    {
    case DataType::INT:
        return "INT";
    case DataType::FLOAT:
        return "FLOAT";
    case DataType::STRING:
        return "STRING";
    case DataType::BOOL:
        return "BOOL";
    default:
        return "UNKNOWN";
    }
}

auto nodeTypeToString(const NodeType type) -> std::string
{
    switch (type)
    {
    case NodeType::MODEL:
        return "MODEL";
    case NodeType::NODE_LIST:
        return "NODE_LIST";
    case NodeType::INPUT_LIST:
        return "INPUT_LIST";
    case NodeType::OUTPUT_LIST:
        return "OUTPUT_LIST";
    case NodeType::INITIALIZER_LIST:
        return "INITIALIZER_LIST";
    case NodeType::NODE:
        return "NODE";
    case NodeType::INPUT_ARR:
        return "INPUT_ARR";
    case NodeType::OUTPUT_ARR:
        return "OUTPUT_ARR";
    case NodeType::ATTRIBUTE_LIST:
        return "ATTRIBUTE_LIST";
    case NodeType::ATTRIBUTE:
        return "ATTRIBUTE";
    case NodeType::IO_TENSOR:
        return "IO_TENSOR";
    case NodeType::IO_SHAPE:
        return "IO_SHAPE";
    case NodeType::IO_DIM:
        return "IO_DIM";
    case NodeType::INIT_TENSOR:
        return "INIT_TENSOR";
    case NodeType::INIT_SHAPE:
        return "INIT_SHAPE";
    case NodeType::U32_LITERAL:
        return "U32_LITERAL";
    case NodeType::U64_LITERAL:
        return "U64_LITERAL";
    case NodeType::STR_LITERAL:
        return "STR_LITERAL";
    case NodeType::BYTES_LITERAL:
        return "BYTES_LITERAL";
    case NodeType::TYPE_ENUM:
        return "TYPE_ENUM";
    case NodeType::ERROR_NODE:
        return "ERROR_NODE";
    default:
        return "UNKNOWN";
    }
}

void ASTOutputVisitor::visit(const ModelNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    if (node.getIrVersion() != nullptr)
    {
        node.getIrVersion()->accept(*this);
    }
    if (node.getProducerName() != nullptr)
    {
        node.getProducerName()->accept(*this);
    }
    if (node.getProducerVersion() != nullptr)
    {
        node.getProducerVersion()->accept(*this);
    }
    if (node.getDomain() != nullptr)
    {
        node.getDomain()->accept(*this);
    }
    if (node.getModelVersion() != nullptr)
    {
        node.getModelVersion()->accept(*this);
    }
    if (node.getDocString() != nullptr)
    {
        node.getDocString()->accept(*this);
    }
    if (node.getGraphName() != nullptr)
    {
        node.getGraphName()->accept(*this);
    }
    if (node.getNodeList() != nullptr)
    {
        node.getNodeList()->accept(*this);
    }
    if (node.getInputList() != nullptr)
    {
        node.getInputList()->accept(*this);
    }
    if (node.getOutputList() != nullptr)
    {
        node.getOutputList()->accept(*this);
    }
    if (node.getInitializerList() != nullptr)
    {
        node.getInitializerList()->accept(*this);
    }
    if (node.getOpsetDomain() != nullptr)
    {
        node.getOpsetDomain()->accept(*this);
    }
    if (node.getOpsetVersion() != nullptr)
    {
        node.getOpsetVersion()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const NodeListNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &node_in_list : node.getNodes())
    {
        node_in_list->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InputListNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &input : node.getIOTensors())
    {
        input->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const OutputListNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &output : node.getIOTensors())
    {
        output->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InitializerListNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &initializer : node.getInitTensors())
    {
        initializer->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const NodeNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    if (node.getOpType() != nullptr)
    {
        node.getOpType()->accept(*this);
    }
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getInputListOrArray() != nullptr)
    {
        node.getInputListOrArray()->accept(*this);
    }
    if (node.getOutputListOrArray() != nullptr)
    {
        node.getOutputListOrArray()->accept(*this);
    }
    if (node.getAttributeList() != nullptr)
    {
        node.getAttributeList()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InputArrNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &elem : node.getInputElements())
    {
        elem->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const OutputArrNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &elem : node.getOutputElements())
    {
        elem->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const AttributeListNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &attr : node.getAttributes())
    {
        attr->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const AttributeNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getValue() != nullptr)
    {
        node.getValue()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const IOTensorNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getType() != nullptr)
    {
        node.getType()->accept(*this);
    }
    if (node.getIOShape() != nullptr)
    {
        node.getIOShape()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const IOShapeNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    for (const auto &elem : node.getIODims())
    {
        elem->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InitTensorNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << "\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getType() != nullptr)
    {
        node.getType()->accept(*this);
    }
    if (node.getInitShape() != nullptr)
    {
        node.getInitShape()->accept(*this);
    }
    if (node.getRawData() != nullptr)
    {
        node.getRawData()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InitShapeNode &node)
{
}

void ASTOutputVisitor::visit(const U32LiteralNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << " " << node.getValue() << ")\n";
}

void ASTOutputVisitor::visit(const U64LiteralNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << " " << node.getValue() << ")\n";
}

void ASTOutputVisitor::visit(const StrLiteralNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << " \"" << node.getValue() << "\")\n";
}

void ASTOutputVisitor::visit(const BytesLiteralNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << " [";
    const auto &bytes = node.getValue();
    for (size_t i = 0; i < bytes.size(); ++i)
    {
        m_ss << static_cast<int>(bytes[i]);
        if (i < bytes.size() - 1)
        {
            m_ss << ", ";
        }
    }
    m_ss << "])\n";
}

void ASTOutputVisitor::visit(const TypeEnumNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << " " << DataTypeToString(node.getValue()) << ")\n";
}

void ASTOutputVisitor::visit(const ErrorNode &node)
{
    addIndent();
    m_ss << "(" << nodeTypeToString(node.getASTNodeType()) << ")\n";
}

} // namespace sonnx
