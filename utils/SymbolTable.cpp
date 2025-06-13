#include "SymbolTable.hpp"

namespace sonnx
{

void NodeSymbol::addInput(TensorSymbol *tensor)
{
    inputs_.push_back(tensor);
    tensor->addUser(this);
}

void NodeSymbol::addOutput(TensorSymbol *tensor)
{
    outputs_.push_back(tensor);
    tensor->setProducer(this);
}

void TensorSymbol::setProducer(const NodeSymbol *node)
{
    producer_ = node;
}

void TensorSymbol::addUser(const NodeSymbol *node)
{
    users_.push_back(node);
}

bool SymbolTable::insertNodeSymbol(const std::string &name, const std::string &op_type, const ASTNode *def)
{
    if (symbols_.find(name) != symbols_.end())
    {
        return false;
    }
    symbols_[name] = std::make_unique<NodeSymbol>(name, op_type, def);
    return true;
}

bool SymbolTable::insertTensorSymbol(const std::string &name, DataType dtype, const ASTNode *def)
{
    if (symbols_.find(name) != symbols_.end())
    {
        return false;
    }
    symbols_[name] = std::make_unique<TensorSymbol>(name, dtype, def);
    return true;
}

BaseSymbol *SymbolTable::lookup(const std::string &name)
{
    const auto it = symbols_.find(name);
    return (it != symbols_.end()) ? it->second.get() : nullptr;
}

NodeSymbol *SymbolTable::getNodeSymbol(const std::string &name)
{
    return dynamic_cast<NodeSymbol *>(lookup(name));
}

TensorSymbol *SymbolTable::getTensorSymbol(const std::string &name)
{
    return dynamic_cast<TensorSymbol *>(lookup(name));
}

// Additional implementations for SymbolTable.cpp
std::vector<NodeSymbol *> SymbolTable::getAllNodeSymbols()
{
    std::vector<NodeSymbol *> nodes;
    for (auto &pair : symbols_)
    {
        NodeSymbol *node = dynamic_cast<NodeSymbol *>(pair.second.get());
        if (node)
        {
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<TensorSymbol *> SymbolTable::getAllTensorSymbols()
{
    std::vector<TensorSymbol *> tensors;
    for (auto &pair : symbols_)
    {
        TensorSymbol *tensor = dynamic_cast<TensorSymbol *>(pair.second.get());
        if (tensor)
        {
            tensors.push_back(tensor);
        }
    }
    return tensors;
}

void SymbolTable::clear()
{
    symbols_.clear();
}

void SymbolTable::buildEdges(NodeSymbol* node,
                         const std::vector<std::string>& inputs,
                         const std::vector<std::string>& outputs) {
    for (const auto& in : inputs) {
        if (auto* tensor = getTensorSymbol(in)) {
            node->addInput(tensor);
        } // Else: Semantic visitor will report missing definition later
    }
    for (const auto& out : outputs) {
        if (auto* tensor = getTensorSymbol(out)) {
            node->addOutput(tensor);
        }
    }
}

} // namespace sonnx
