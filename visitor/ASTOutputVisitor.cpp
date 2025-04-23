#include "ASTOutputVisitor.hpp"
#include "ast/AST.hpp"
#include <cstddef>
#include <string>

namespace sonnx
{
auto valueOrDataTypeToString(sonnx::ValueOrDataType type) -> std::string
{
    switch (type)
    {
    case ValueOrDataType::INT:
        return "INT";
    case ValueOrDataType::FLOAT:
        return "FLOAT";
    case ValueOrDataType::STRING:
        return "STRING";
    case ValueOrDataType::BOOL:
        return "BOOL";
    default:
        return "UNKNOWN";
    }
}
void ASTOutputVisitor::visit(const ModelNode &node)
{
    addIndent();
    m_ss << "(MODEL\n";
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
    if (node.getGraph() != nullptr)
    {
        node.getGraph()->accept(*this);
    }
    if (node.getOpsetImport() != nullptr)
    {
        node.getOpsetImport()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const GraphNode &node)
{
    addIndent();
    m_ss << "(GRAPH\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
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
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const NodeListNode &node)
{
    addIndent();
    m_ss << "(NODE_LIST\n";
    ++m_indent_level;
    for (const auto &node : node.getNodes())
    {
        node->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const InputListNode &node)
{
    addIndent();
    m_ss << "(INPUT_LIST\n";
    ++m_indent_level;
    for (const auto &input : node.getInputs())
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
    m_ss << "(OUTPUT_LIST\n";
    ++m_indent_level;
    for (const auto &output : node.getOutputs())
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
    m_ss << "(INITIALIZER_LIST\n";
    ++m_indent_level;
    for (const auto &initializer : node.getInitializers())
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
    m_ss << "(NODE\n";
    ++m_indent_level;
    if (node.getOpType() != nullptr)
    {
        node.getOpType()->accept(*this);
    }
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getInputs() != nullptr)
    {
        node.getInputs()->accept(*this);
    }
    if (node.getOutputs() != nullptr)
    {
        node.getOutputs()->accept(*this);
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
    m_ss << "(INPUT_ARR\n";
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
    m_ss << "(OUTPUT_ARR\n";
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
    m_ss << "(ATTRIBUTE_LIST\n";
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
    m_ss << "(ATTRIBUTE\n";
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

void ASTOutputVisitor::visit(const ValueInfoNode &node)
{
    addIndent();
    m_ss << "(VALUE_INFO\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getTensorType() != nullptr)
    {
        node.getTensorType()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const TensorTypeNode &node)
{
    addIndent();
    m_ss << "(TENSOR_TYPE\n";
    ++m_indent_level;
    if (node.getElemType() != nullptr)
    {
        node.getElemType()->accept(*this);
    }
    if (node.getShape() != nullptr)
    {
        node.getShape()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const ShapeNode &node)
{
    addIndent();
    m_ss << "(SHAPE\n";
    ++m_indent_level;
    for (const auto &dim : node.getDimElements())
    {
        dim->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const DimNode &node)
{
    addIndent();
    m_ss << "(DIM\n";
    ++m_indent_level;
    if (node.getValue() != nullptr)
    {
        node.getValue()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const TensorNode &node)
{
    addIndent();
    m_ss << "(TENSOR\n";
    ++m_indent_level;
    if (node.getName() != nullptr)
    {
        node.getName()->accept(*this);
    }
    if (node.getDataType() != nullptr)
    {
        node.getDataType()->accept(*this);
    }
    if (node.getDimsArray() != nullptr)
    {
        node.getDimsArray()->accept(*this);
    }
    if (node.getRawData() != nullptr)
    {
        node.getRawData()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const DimsArrayNode &node)
{
    addIndent();
    m_ss << "(DIM\n"; // Using DIM as per getASTNodeType() implementation
    ++m_indent_level;
    for (const auto &dim : node.getDimsElements())
    {
        dim->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

void ASTOutputVisitor::visit(const OpsetImportNode &node)
{
    addIndent();
    m_ss << "(OPSET_IMPORT\n";
    ++m_indent_level;
    if (node.getDomain() != nullptr)
    {
        node.getDomain()->accept(*this);
    }
    if (node.getVersion() != nullptr)
    {
        node.getVersion()->accept(*this);
    }
    --m_indent_level;
    addIndent();
    m_ss << ")\n";
}

// Terminal nodes with values
void ASTOutputVisitor::visit(const U32LiteralNode &node)
{
    addIndent();
    m_ss << "(U32_LITERAL " << node.getValue() << ")\n";
}

void ASTOutputVisitor::visit(const U64LiteralNode &node)
{
    addIndent();
    m_ss << "(U64_LITERAL " << node.getValue() << ")\n";
}

void ASTOutputVisitor::visit(const StrLiteralNode &node)
{
    addIndent();
    m_ss << "(STR_LITERAL \"" << node.getValue() << "\")\n";
}

void ASTOutputVisitor::visit(const BytesLiteralNode &node)
{
    addIndent();
    m_ss << "(BYTES_LITERAL [";
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
    // Note: TypeEnumNode's getASTNodeType() returns BYTES_LITERAL, but we print TYPE_ENUM
    m_ss << "(TYPE_ENUM " << valueOrDataTypeToString(node.getValue()) << ")\n";
}

void ASTOutputVisitor::visit(const ErrorNode & /*node*/)
{
    addIndent();
    m_ss << "(ERROR)\n";
}

// Function to traverse the AST and return the resulting string
auto traverseAST(ASTNode *root) -> std::string
{
    if (root == nullptr)
    {
        return "";
    }
    ASTOutputVisitor visitor;
    root->accept(visitor);
    return visitor.getResult();
}

} // namespace sonnx
