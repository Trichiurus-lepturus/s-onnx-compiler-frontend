#include "AST.hpp"

#include <visitor/ASTBaseVisitor.hpp>

namespace sonnx
{

void U32LiteralNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void U64LiteralNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void StrLiteralNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void BytesLiteralNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void TypeEnumNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void ModelNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void NodeListNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void InputListNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void OutputListNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void InitializerListNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void NodeNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void InputArrNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void OutputArrNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void AttributeListNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void AttributeNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void IOTensorNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void IOShapeNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void InitTensorNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void InitShapeNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}
void ErrorNode::accept(ASTBaseVisitor &visitor) const
{
    visitor.visit(*this);
}

} // namespace sonnx
