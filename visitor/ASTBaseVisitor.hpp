#ifndef AST_BASE_VISITOR_HPP
#define AST_BASE_VISITOR_HPP

namespace sonnx
{

class ASTBaseVisitor
{
  public:
    ASTBaseVisitor() = default;
    virtual ~ASTBaseVisitor() = default;
    ASTBaseVisitor(const ASTBaseVisitor &) = delete;
    auto operator=(const ASTBaseVisitor &) -> ASTBaseVisitor & = delete;
    ASTBaseVisitor(ASTBaseVisitor &&) = delete;
    auto operator=(ASTBaseVisitor &&) -> ASTBaseVisitor & = delete;

    virtual void visit(const class U32LiteralNode &node) = 0;
    virtual void visit(const class U64LiteralNode &node) = 0;
    virtual void visit(const class StrLiteralNode &node) = 0;
    virtual void visit(const class BytesLiteralNode &node) = 0;
    virtual void visit(const class TypeEnumNode &node) = 0;
    virtual void visit(const class ModelNode &node) = 0;
    virtual void visit(const class NodeListNode &node) = 0;
    virtual void visit(const class InputListNode &node) = 0;
    virtual void visit(const class OutputListNode &node) = 0;
    virtual void visit(const class InitializerListNode &node) = 0;
    virtual void visit(const class NodeNode &node) = 0;
    virtual void visit(const class InputArrNode &node) = 0;
    virtual void visit(const class OutputArrNode &node) = 0;
    virtual void visit(const class AttributeListNode &node) = 0;
    virtual void visit(const class AttributeNode &node) = 0;
    virtual void visit(const class IOTensorNode &node) = 0;
    virtual void visit(const class IOShapeNode &node) = 0;
    virtual void visit(const class InitTensorNode &node) = 0;
    virtual void visit(const class InitShapeNode &node) = 0;
    virtual void visit(const class ErrorNode &node) = 0;
};

} // namespace sonnx

#endif // AST_BASE_VISITOR_HPP
