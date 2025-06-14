#include "ASTSemanticVisitor.hpp"

namespace sonnx
{

void ASTSemanticVisitor::visit(const ModelNode &node)
{
    // Visit components in order
    if (node.getInitializerList())
    {
        processing_initializers_ = true;
        node.getInitializerList()->accept(*this);
        processing_initializers_ = false;
    }

    if (node.getInputList())
    {
        processing_model_inputs_ = true;
        node.getInputList()->accept(*this);
        processing_model_inputs_ = false;
    }

    if (node.getNodeList())
    {
        node.getNodeList()->accept(*this);
    }

    if (node.getOutputList())
    {
        processing_model_outputs_ = true;
        node.getOutputList()->accept(*this);
        processing_model_outputs_ = false;
    }

    // Perform semantic analysis after visiting all nodes
    performSemanticAnalysis();
}

void ASTSemanticVisitor::visit(const NodeListNode &node)
{
    for (const auto &n : node.getNodes())
    {
        if (n)
        {
            n->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const InputListNode &node)
{
    for (const auto &tensor : node.getIOTensors())
    {
        if (tensor)
        {
            tensor->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const OutputListNode &node)
{
    for (const auto &tensor : node.getIOTensors())
    {
        if (tensor)
        {
            tensor->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const InitializerListNode &node)
{
    for (const auto &tensor : node.getInitTensors())
    {
        if (tensor)
        {
            tensor->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const NodeNode &node)
{
    // Extract node information
    std::string node_name = extractStringFromNode(node.getName());
    std::string op_type = extractStringFromNode(node.getOpType());

    // Store current node name for child visits
    current_node_name_ = node_name;

    // Check for duplicate node names
    if (!symbol_table_.insertNodeSymbol(node_name, op_type, &node))
    {
        reportError("Duplicate node name: '" + node_name + "'");
    }

    // Clear refs for this node
    node_input_refs_.clear();
    node_output_refs_.clear();

    // Visit inputs and outputs
    if (node.getInputListOrArray())
    {
        node.getInputListOrArray()->accept(*this);
    }
    if (node.getOutputListOrArray())
    {
        node.getOutputListOrArray()->accept(*this);
    }

    // Build edges in symbol table
    if (auto* node_symbol = symbol_table_.getNodeSymbol(node_name))
    {
        // Add input edges
        for (const auto& input_ref : node_input_refs_)
        {
            if (auto* tensor_symbol = symbol_table_.getTensorSymbol(input_ref))
            {
                node_symbol->addInput(tensor_symbol);
            }
            else if (defined_tensors_.find(input_ref) == defined_tensors_.end())
            {
                reportError("Node '" + node_name + "' references undefined input '" + input_ref + "'");
            }
        }

        // Add output edges
        for (const auto& output_ref : node_output_refs_)
        {
            if (auto* tensor_symbol = symbol_table_.getTensorSymbol(output_ref))
            {
                // Check if tensor already has a producer
                if (tensor_symbol->getProducer() != nullptr)
                {
                    reportError("Tensor '" + output_ref + "' is produced by multiple nodes");
                }
                else
                {
                    node_symbol->addOutput(tensor_symbol);
                }
            }
        }
    }

    // Visit attributes if present
    if (node.getAttributeList())
    {
        node.getAttributeList()->accept(*this);
    }
}

void ASTSemanticVisitor::visit(const InputArrNode &node)
{
    for (const auto &str_node : node.getInputElements())
    {
        if (str_node)
        {
            std::string tensor_ref = extractStringFromNode(str_node.get());
            node_input_refs_.push_back(tensor_ref);
        }
    }
}

void ASTSemanticVisitor::visit(const OutputArrNode &node)
{
    for (const auto &str_node : node.getOutputElements())
    {
        if (str_node)
        {
            std::string tensor_ref = extractStringFromNode(str_node.get());
            node_output_refs_.push_back(tensor_ref);

            // Define the tensor if not already defined
            if (defined_tensors_.find(tensor_ref) == defined_tensors_.end())
            {
                // Create tensor with undefined type (will be inferred later)
                symbol_table_.insertTensorSymbol(tensor_ref, DataType::UNDEFINED, str_node.get());
                defined_tensors_.insert(tensor_ref);
            }
        }
    }
}

void ASTSemanticVisitor::visit(const AttributeListNode &node)
{
    for (const auto &attr : node.getAttributes())
    {
        if (attr)
        {
            attr->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const IOTensorNode &node)
{
    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    // Check for duplicate tensor names
    if (defined_tensors_.count(tensor_name) > 0)
    {
        reportError("Duplicate tensor name: '" + tensor_name + "'");
        return;
    }

    // Insert tensor symbol
    if (!symbol_table_.insertTensorSymbol(tensor_name, data_type, &node))
    {
        reportError("Failed to insert tensor: '" + tensor_name + "'");
    }
    else
    {
        defined_tensors_.insert(tensor_name);

        if (processing_model_inputs_)
        {
            model_input_names_.insert(tensor_name);
            if (auto* tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
            {
                tensor_sym->setIsModelInput(true);
            }
        }
        else if (processing_model_outputs_)
        {
            model_output_names_.insert(tensor_name);
            if (auto* tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
            {
                tensor_sym->setIsModelOutput(true);
            }
        }
    }

    // Visit shape if present
    if (node.getIOShape())
    {
        node.getIOShape()->accept(*this);
    }
}

void ASTSemanticVisitor::visit(const InitTensorNode &node)
{
    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    // Check for duplicate tensor names
    if (defined_tensors_.count(tensor_name) > 0)
    {
        reportError("Duplicate tensor name: '" + tensor_name + "'");
        return;
    }

    // Insert tensor symbol
    if (!symbol_table_.insertTensorSymbol(tensor_name, data_type, &node))
    {
        reportError("Failed to insert initializer tensor: '" + tensor_name + "'");
    }
    else
    {
        defined_tensors_.insert(tensor_name);
        initializer_defined_.insert(tensor_name);

        if (auto* tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
        {
            tensor_sym->setIsInitializer(true);
        }
    }
}

void ASTSemanticVisitor::performSemanticAnalysis()
{
    checkUndefinedInputs();
    checkUnusedOutputs();
    runDAGAnalysis();
}

void ASTSemanticVisitor::checkUndefinedInputs()
{
    // Check that all model outputs are defined
    for (const auto& output_name : model_output_names_)
    {
        if (defined_tensors_.find(output_name) == defined_tensors_.end())
        {
            reportError("Model output '" + output_name + "' is not defined");
        }
    }
}

void ASTSemanticVisitor::checkUnusedOutputs() const
{
    // Check for tensors that are produced but never consumed
    for (auto* tensor : symbol_table_.getAllTensorSymbols())
    {
        if (!tensor->isModelOutput() && tensor->getUsers().empty() && tensor->getProducer())
        {
            // This tensor is produced but never used
            // Could report as warning
        }
    }
}

void ASTSemanticVisitor::runDAGAnalysis()
{
    // Build the DAG
    symbol_table_.buildDAG();

    // Perform topological sort
    symbol_table_.performTopologicalSort();

    if (symbol_table_.hasCycle())
    {
        reportError("Cycle detected in computation graph");
    }

    // Run optimization detection
    symbol_table_.detectConstantFolding();
    symbol_table_.detectDeadCode();
    symbol_table_.detectCommonSubexpressions();
}

void ASTSemanticVisitor::reportError(const std::string &message)
{
    errors_.push_back("Semantic error: " + message);
}

std::string ASTSemanticVisitor::extractStringFromNode(const ASTNode *node)
{
    if (!node) return "";

    if (auto* str_node = dynamic_cast<const StrLiteralNode*>(node))
    {
        return str_node->getValue();
    }
    return "";
}

DataType ASTSemanticVisitor::extractDataTypeFromNode(const ASTNode *node)
{
    if (!node) return DataType::UNDEFINED;

    if (auto* type_node = dynamic_cast<const TypeEnumNode*>(node))
    {
        return type_node->getValue();
    }
    return DataType::UNDEFINED;
}

} // namespace sonnx
