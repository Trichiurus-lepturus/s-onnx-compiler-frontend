#ifndef AST_OUTPUT_VISITOR_HPP
#define AST_OUTPUT_VISITOR_HPP

#include "ASTBaseVisitor.hpp"
#include <sstream>

#define SPACE_1 " "
#define SPACE_4 SPACE_1 SPACE_1 SPACE_1 SPACE_1
#define SPACE_32 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4 SPACE_4
#define SPACE_256 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32 SPACE_32

namespace sonnx
{

class ASTOutputVisitor final : public ASTBaseVisitor
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

    void visit(const U32LiteralNode &node) override;
    void visit(const U64LiteralNode &node) override;
    void visit(const StrLiteralNode &node) override;
    void visit(const BytesLiteralNode &node) override;
    void visit(const TypeEnumNode &node) override;
    void visit(const ModelNode &node) override;
    void visit(const NodeListNode &node) override;
    void visit(const InputListNode &node) override;
    void visit(const OutputListNode &node) override;
    void visit(const InitializerListNode &node) override;
    void visit(const NodeNode &node) override;
    void visit(const InputArrNode &node) override;
    void visit(const OutputArrNode &node) override;
    void visit(const AttributeListNode &node) override;
    void visit(const AttributeNode &node) override;
    void visit(const IOTensorNode &node) override;
    void visit(const IOShapeNode &node) override;
    void visit(const IODimNode &node) override;
    void visit(const InitTensorNode &node) override;
    void visit(const InitShapeNode &node) override;
    void visit(const ErrorNode &node) override;
};

} // namespace sonnx

#endif // AST_OUTPUT_VISITOR_HPP
