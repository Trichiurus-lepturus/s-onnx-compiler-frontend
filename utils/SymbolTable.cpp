#include "SymbolTable.hpp"
#include <algorithm>
#include <queue>
#include <sstream>
#include <iostream>

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

std::vector<NodeSymbol *> SymbolTable::getAllNodeSymbols() const
{
    std::vector<NodeSymbol *> nodes;
    for (auto &pair : symbols_)
    {
        if (auto *node = dynamic_cast<NodeSymbol *>(pair.second.get()))
        {
            nodes.push_back(node);
        }
    }
    return nodes;
}

std::vector<TensorSymbol *> SymbolTable::getAllTensorSymbols() const
{
    std::vector<TensorSymbol *> tensors;
    for (auto &pair : symbols_)
    {
        if (auto *tensor = dynamic_cast<TensorSymbol *>(pair.second.get()))
        {
            tensors.push_back(tensor);
        }
    }
    return tensors;
}

void SymbolTable::buildDAG()
{
    dag_edges_.clear();
    reverse_dag_edges_.clear();

    // Build edges based on tensor producers and consumers
    for (auto *node : getAllNodeSymbols())
    {
        for (const auto *input : node->getInputs())
        {
            if (auto *producer = input->getProducer())
            {
                dag_edges_[producer].push_back(node);
                reverse_dag_edges_[node].push_back(producer);
            }
        }
    }
}

void SymbolTable::performTopologicalSort()
{
    topological_order_.clear();
    has_cycle_ = false;

    auto nodes = getAllNodeSymbols();
    std::unordered_set<NodeSymbol *> visited;
    std::unordered_set<NodeSymbol *> recursion_stack;

    for (auto *node : nodes)
    {
        if (visited.find(node) == visited.end())
        {
            if (!topologicalSortDFS(node, visited, recursion_stack))
            {
                has_cycle_ = true;
                topological_order_.clear();
                return;
            }
        }
    }

    // Reverse to get correct topological order
    std::reverse(topological_order_.begin(), topological_order_.end());
}

bool SymbolTable::topologicalSortDFS(NodeSymbol *node, std::unordered_set<NodeSymbol *> &visited,
                                     std::unordered_set<NodeSymbol *> &recursion_stack)
{
    visited.insert(node);
    recursion_stack.insert(node);

    if (dag_edges_.find(node) != dag_edges_.end())
    {
        for (auto *child : dag_edges_[node])
        {
            if (recursion_stack.find(child) != recursion_stack.end())
            {
                return false; // Cycle detected
            }
            if (visited.find(child) == visited.end())
            {
                if (!topologicalSortDFS(child, visited, recursion_stack))
                {
                    return false;
                }
            }
        }
    }

    recursion_stack.erase(node);
    topological_order_.push_back(node);
    return true;
}

void SymbolTable::detectConstantFolding()
{
    // Identify operations with all constant inputs
    for (auto *node : topological_order_)
    {
        bool all_inputs_constant = true;
        for (const auto *input : node->getInputs())
        {
            if (!input->isInitializer())
            {
                all_inputs_constant = false;
                break;
            }
        }

        if (all_inputs_constant && !node->getInputs().empty())
        {
            // This node could be constant folded
            // Mark for optimization (could add a flag to NodeSymbol)
        }
    }
}

void SymbolTable::detectDeadCode() const
{
    std::unordered_set<const TensorSymbol *> used_tensors;

    // Mark all model outputs as used
    for (const auto *tensor : getAllTensorSymbols())
    {
        if (tensor->isModelOutput())
        {
            used_tensors.insert(tensor);
        }
    }

    // Backward traversal to find all used nodes
    std::unordered_set<NodeSymbol *> used_nodes;
    std::queue<const TensorSymbol *> work_queue;

    for (const auto *tensor : used_tensors)
    {
        work_queue.push(tensor);
    }

    while (!work_queue.empty())
    {
        const auto *tensor = work_queue.front();
        work_queue.pop();

        if (auto *producer = tensor->getProducer())
        {
            if (used_nodes.insert(producer).second)
            {
                for (const auto *input : producer->getInputs())
                {
                    if (used_tensors.insert(input).second)
                    {
                        work_queue.push(input);
                    }
                }
            }
        }
    }

    // Any node not in used_nodes is dead code
}

void SymbolTable::detectCommonSubexpressions()
{
    std::map<std::string, std::vector<NodeSymbol *>> operation_patterns;

    for (auto *node : topological_order_)
    {
        // Create a signature for the operation
        std::string op_signature = node->getOpType() + ":";
        for (const auto *input : node->getInputs())
        {
            op_signature += input->getName() + ",";
        }
        operation_patterns[op_signature].push_back(node);
    }

    // Nodes with the same pattern could be optimized
    for (const auto &[pattern, nodes] : operation_patterns)
    {
        if (nodes.size() > 1)
        {
            // These nodes perform the same operation on the same inputs
            // Mark for potential CSE optimization
        }
    }
}

void SymbolTable::clear()
{
    symbols_.clear();
    dag_edges_.clear();
    reverse_dag_edges_.clear();
    topological_order_.clear();
    has_cycle_ = false;
}

std::string SymbolTable::generateTACode() const
{
    std::ostringstream code;

    // Generate Input tensors
    for (const auto *tensor : getAllTensorSymbols())
    {
        if (tensor->isModelInput())
        {
            std::string t_var = getOrCreateTVariableName(tensor->getName());
            code << t_var << " = Input(\"" << tensor->getName() << "\", " << dataTypeToString(tensor->getDataType())
                 << ", " << tensor->getShapeString() << ")\n";
        }
    }

    // Generate Initializer tensors
    for (const auto *tensor : getAllTensorSymbols())
    {
        if (tensor->isInitializer())
        {
            std::string t_var = getOrCreateTVariableName(tensor->getName());
            code << t_var << " = Initializer(\"" << tensor->getName() << "\", "
                 << dataTypeToString(tensor->getDataType()) << ", " << tensor->getShapeString()
                 << ", raw_data=" << tensor->getRawDataHex() << ")\n";
        }
    }

    // Generate Operations
    for (auto it = topological_order_.begin(); it != topological_order_.end(); ++it)
    {
        auto &node = *it;
        // For each output of this node
        for (const auto *output : node->getOutputs())
        {
            std::string result_var = getOrCreateTVariableName(output->getName());

            std::string op_line = result_var + " = " + node->getOpType() + "(";

            // Add input operands
            const auto &inputs = node->getInputs();
            for (size_t i = 0; i < inputs.size(); ++i)
            {
                if (i > 0)
                    op_line += ", ";
                op_line += getOrCreateTVariableName(inputs[i]->getName());
            }

            // Add attributes
            const std::string &attrs = node->getAttributesString();
            if (!attrs.empty())
            {
                if (!inputs.empty())
                    op_line += ", ";
                op_line += attrs;
            }

            op_line += ")";
            code << op_line << "\n";
        }
    }

    // Generate Output tensors
    for (const auto *tensor : getAllTensorSymbols())
    {
        if (tensor->isModelOutput())
        {
            std::string t_var = getOrCreateTVariableName(tensor->getName());
            code << "Output(\"" << tensor->getName() << "\", " << t_var << ")\n";
        }
    }

    return code.str();
}

std::string SymbolTable::dataTypeToString(DataType dtype)
{
    switch (dtype)
    {
    case DataType::FLOAT:
        return "FLOAT";
    case DataType::INT:
        return "INT";
    case DataType::STRING:
        return "STRING";
    case DataType::BOOL:
        return "BOOL";
    default:
        return "UNDEFINED";
    }
}

std::string SymbolTable::getOrCreateTVariableName(const std::string &original_name) const
{
    // Check if we already assigned a T-variable name
    auto it = tensor_to_t_mapping_.find(original_name);
    if (it != tensor_to_t_mapping_.end())
    {
        return it->second;
    }

    // Create new T-variable name
    std::string t_name = "T" + std::to_string(t_variable_counter_++);
    tensor_to_t_mapping_[original_name] = t_name;
    return t_name;
}

bool SymbolTable::isModelInputOrOutput(const TensorSymbol *tensor)
{
    if (!tensor)
        return false;
    return tensor->isModelInput() || tensor->isModelOutput() || tensor->isInitializer();
}

} // namespace sonnx
