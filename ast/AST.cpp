#include "AST.hpp"

#include <visitor/Visitor.hpp>

namespace sonnx
{

void U32LiteralNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void U64LiteralNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void StrLiteralNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void BytesLiteralNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void TypeEnumNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void ModelNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void GraphNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void NodeListNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void InputListNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void OutputListNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void InitializerListNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void NodeNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void InputArrNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void OutputArrNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void AttributeListNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void AttributeNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void ValueInfoNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void TensorTypeNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void ShapeNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void DimNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void TensorNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void DimsArrayNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void OpsetImportNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
void ErrorNode::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

} // namespace sonnx
