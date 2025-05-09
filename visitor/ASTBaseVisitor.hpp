#ifndef AST_BASE_VISITOR_HPP
#define AST_BASE_VISITOR_HPP

namespace sonnx
{

class Visitor
{
  public:
    Visitor() = default;
    virtual ~Visitor() = default;
    Visitor(const Visitor &) = delete;
    auto operator=(const Visitor &) -> Visitor & = delete;
    Visitor(Visitor &&) = delete;
    auto operator=(Visitor &&) -> Visitor & = delete;

    // Visitor methods for all possible AST nodes (to be overridden by concrete visitors)
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
    virtual void visit(const class IODimNode &node) = 0;
    virtual void visit(const class InitTensorNode &node) = 0;
    virtual void visit(const class InitShapeNode &node) = 0;
    virtual void visit(const class ErrorNode &node) = 0;
};

class ASTTag
{
public:
  ASTTag() = default;
  virtual ~ASTTag() = default;
  ASTTag(const ASTTag &) = default;
  auto operator=(const ASTTag &) -> ASTTag & = default;
  ASTTag(ASTTag &&) = default;
  auto operator=(ASTTag &&) -> ASTTag & = default;
};

} // namespace sonnx

#endif // AST_BASE_VISITOR_HPP
