#ifndef AST_SEMANTIC_VISITOR_HPP
#define AST_SEMANTIC_VISITOR_HPP
#include "ASTBaseVisitor.hpp"

namespace sonnx
{

class ASTSemanticVisitor final : public Visitor
{
  public:
    void visit(const U32LiteralNode &node) override;
    void visit(const U64LiteralNode &node) override;
    void visit(const StrLiteralNode &node) override;
    void visit(const BytesLiteralNode &node) override;
    void visit(const TypeEnumNode &node) override;
    void visit(const ModelNode &node) override;
    void visit(const GraphNode &node) override;
    void visit(const NodeListNode &node) override;
    void visit(const InputListNode &node) override;
    void visit(const OutputListNode &node) override;
    void visit(const InitializerListNode &node) override;
    void visit(const NodeNode &node) override;
    void visit(const InputArrNode &node) override;
    void visit(const OutputArrNode &node) override;
    void visit(const AttributeListNode &node) override;
    void visit(const AttributeNode &node) override;
    void visit(const ValueInfoNode &node) override;
    void visit(const TensorTypeNode &node) override;
    void visit(const ShapeNode &node) override;
    void visit(const DimNode &node) override;
    void visit(const TensorNode &node) override;
    void visit(const DimsArrayNode &node) override;
    void visit(const OpsetImportNode &node) override;
    void visit(const ErrorNode &node) override;
};

} // namespace sonnx

#endif // AST_SEMANTIC_VISITOR_HPP
