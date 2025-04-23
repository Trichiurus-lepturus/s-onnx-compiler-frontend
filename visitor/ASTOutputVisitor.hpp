#ifndef AST_OUTPUT_VISITOR_HPP
#define AST_OUTPUT_VISITOR_HPP

#include "Visitor.hpp"
#include <sstream>

#define SPACE_1 " "
#define SPACE_4 SPACE_1 SPACE_1 SPACE_1 SPACE_1
#define SPACE_32 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4
#define SPACE_256 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32

namespace sonnx
{

class ASTOutputVisitor : public Visitor
{
  private:
    std::stringstream m_ss;
    int m_indent_level = 0;
    static constexpr int INDENT_SIZE = 2;
    static constexpr int MAX_INDENT_CHARS = 256;
    static constexpr const char *INDENT_BUFFER = SPACE_256;

    void addIndent()
    {
        int spaces = m_indent_level * INDENT_SIZE;
        if (spaces > 0 && spaces < MAX_INDENT_CHARS)
        {
            m_ss.write(INDENT_BUFFER, spaces);
        }
        else if (spaces >= MAX_INDENT_CHARS)
        {
            m_ss << std::string(spaces, ' ');
        }
    }

  public:
    auto getResult() const -> std::string
    {
        return m_ss.str();
    }

    void visit(const class U32LiteralNode &node) override;
    void visit(const class U64LiteralNode &node) override;
    void visit(const class StrLiteralNode &node) override;
    void visit(const class BytesLiteralNode &node) override;
    void visit(const class TypeEnumNode &node) override;
    void visit(const class ModelNode &node) override;
    void visit(const class GraphNode &node) override;
    void visit(const class NodeListNode &node) override;
    void visit(const class InputListNode &node) override;
    void visit(const class OutputListNode &node) override;
    void visit(const class InitializerListNode &node) override;
    void visit(const class NodeNode &node) override;
    void visit(const class InputArrNode &node) override;
    void visit(const class OutputArrNode &node) override;
    void visit(const class AttributeListNode &node) override;
    void visit(const class AttributeNode &node) override;
    void visit(const class ValueInfoNode &node) override;
    void visit(const class TensorTypeNode &node) override;
    void visit(const class ShapeNode &node) override;
    void visit(const class DimNode &node) override;
    void visit(const class TensorNode &node) override;
    void visit(const class DimsArrayNode &node) override;
    void visit(const class OpsetImportNode &node) override;
    void visit(const class ErrorNode &node) override;
};

} // namespace sonnx

#endif // AST_OUTPUT_VISITOR_HPP
