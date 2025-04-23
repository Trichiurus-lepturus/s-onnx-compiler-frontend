#ifndef VISITOR_HPP
#define VISITOR_HPP

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
    virtual void visit(const class GraphNode &node) = 0;
    virtual void visit(const class NodeListNode &node) = 0;
    virtual void visit(const class InputListNode &node) = 0;
    virtual void visit(const class OutputListNode &node) = 0;
    virtual void visit(const class InitializerListNode &node) = 0;
    virtual void visit(const class NodeNode &node) = 0;
    virtual void visit(const class InputArrNode &node) = 0;
    virtual void visit(const class OutputArrNode &node) = 0;
    virtual void visit(const class AttributeListNode &node) = 0;
    virtual void visit(const class AttributeNode &node) = 0;
    virtual void visit(const class ValueInfoNode &node) = 0;
    virtual void visit(const class TensorTypeNode &node) = 0;
    virtual void visit(const class ShapeNode &node) = 0;
    virtual void visit(const class DimNode &node) = 0;
    virtual void visit(const class TensorNode &node) = 0;
    virtual void visit(const class DimsArrayNode &node) = 0;
    virtual void visit(const class OpsetImportNode &node) = 0;
    virtual void visit(const class ErrorNode &node) = 0;
};

} // namespace sonnx

#endif // VISITOR_HPP
