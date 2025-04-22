#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <sstream>

#define SPACE_1 " "
#define SPACE_4 SPACE_1 SPACE_1 SPACE_1 SPACE_1
#define SPACE_32 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4
#define SPACE_256 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32

namespace sonnx
{

class ASTOutputVisitor
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
    ASTOutputVisitor() = default;
    virtual ~ASTOutputVisitor() = default;
    ASTOutputVisitor(const ASTOutputVisitor &) = delete;
    auto operator=(const ASTOutputVisitor &) -> ASTOutputVisitor & = delete;
    ASTOutputVisitor(ASTOutputVisitor &&) = delete;
    auto operator=(ASTOutputVisitor &&) -> ASTOutputVisitor & = delete;

    auto getResult() const -> std::string
    {
        return m_ss.str();
    }

    void visit(const class U32LiteralNode &node);
    void visit(const class U64LiteralNode &node);
    void visit(const class StrLiteralNode &node);
    void visit(const class BytesLiteralNode &node);
    void visit(const class TypeEnumNode &node);
    void visit(const class ModelNode &node);
    void visit(const class GraphNode &node);
    void visit(const class NodeListNode &node);
    void visit(const class InputListNode &node);
    void visit(const class OutputListNode &node);
    void visit(const class InitializerListNode &node);
    void visit(const class NodeNode &node);
    void visit(const class InputArrNode &node);
    void visit(const class OutputArrNode &node);
    void visit(const class AttributeListNode &node);
    void visit(const class AttributeNode &node);
    void visit(const class ValueInfoNode &node);
    void visit(const class TensorTypeNode &node);
    void visit(const class ShapeNode &node);
    void visit(const class DimNode &node);
    void visit(const class TensorNode &node);
    void visit(const class DimsArrayNode &node);
    void visit(const class OpsetImportNode &node);
    void visit(const class ErrorNode &node);
};

} // namespace sonnx

#endif // VISITOR_HPP
