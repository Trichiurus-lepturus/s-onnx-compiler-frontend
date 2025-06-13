#include "ASTSemanticVisitor.hpp"
#include "ast/AST.hpp"

namespace sonnx
{

void ASTSemanticVisitor::visit(const ModelNode &node)
{
    // Process model components in order
    // 1. First process inputs (declarations)
    if (node.getInputList())
    {
        processing_model_inputs_ = true;
        node.getInputList()->accept(*this);
        processing_model_inputs_ = false;
    }

    // 2. Process outputs (declarations)
    if (node.getOutputList())
    {
        processing_model_outputs_ = true;
        node.getOutputList()->accept(*this);
        processing_model_outputs_ = false;
    }

    // 3. Process initializers (definitions)
    if (node.getInitializerList())
    {
        processing_initializers_ = true;
        node.getInitializerList()->accept(*this);
        processing_initializers_ = false;
    }

    // 4. Process nodes
    if (node.getNodeList())
    {
        node.getNodeList()->accept(*this);
    }

    // 5. Final validation
    buildDAG();
}

void ASTSemanticVisitor::visit(const NodeListNode &node)
{
    for (const auto &child : node.getNodes())
    {
        if (child)
        {
            child->accept(*this);
        }
    }
}

void ASTSemanticVisitor::visit(const InputArrNode &node)
{
    std::vector<std::string> tensor_names = extractStringArrayFromNode(&node);
    for (const auto &tensor_name : tensor_names)
    {
        declared_tensors_.insert(tensor_name);
    }
}

void ASTSemanticVisitor::visit(const OutputArrNode &node)
{
    std::vector<std::string> tensor_names = extractStringArrayFromNode(&node);
    for (const auto &tensor_name : tensor_names)
    {
        declared_tensors_.insert(tensor_name);
    }
}

void ASTSemanticVisitor::visit(const InputListNode &node)
{
    processing_model_inputs_ = true;

    for (const auto &io_tensor : node.getIOTensors())
    {
        io_tensor->accept(*this);
    }

    processing_model_inputs_ = false;
}

void ASTSemanticVisitor::visit(const OutputListNode &node)
{
    processing_model_outputs_ = true;

    for (const auto &io_tensor : node.getIOTensors())
    {
        io_tensor->accept(*this);
    }

    processing_model_outputs_ = false;
}

void ASTSemanticVisitor::visit(const InitializerListNode &node)
{
    processing_initializers_ = true;

    for (const auto &init_tensor : node.getInitTensors())
    {
        init_tensor->accept(*this);
    }

    processing_initializers_ = false;
}

void ASTSemanticVisitor::visit(const NodeNode &node)
{
    // Get node info using AST interfaces
    const ASTNode *name_node = node.getName();
    const ASTNode *op_type_node = node.getOpType();
    if (!name_node || !op_type_node)
        return;

    std::string node_name = extractStringFromNode(name_node);
    std::string op_type = extractStringFromNode(op_type_node);

    // 1. Node name conflict check
    if (symbol_table_.getNodeSymbol(node_name))
    {
        reportError("Node name conflict: '" + node_name + "'");
        return;
    }

    // Insert node
    symbol_table_.insertNodeSymbol(node_name, op_type, &node);
    NodeSymbol *node_sym = symbol_table_.getNodeSymbol(node_name);
    if (!node_sym)
        return;

    // Process INPUTS using container access methods
    const ASTNode *input_container = node.getInputListOrArray();
    auto input_names = extractTensorNamesFromInput(input_container);
    for (const auto &input_name : input_names)
    {
        node_input_refs_.insert(input_name);
    }

    // Register input references and connect
    for (const auto &input_name : input_names)
    {
        tensor_references_.insert(input_name);

        // Connect if tensor exists
        if (auto *tensor = symbol_table_.getTensorSymbol(input_name))
        {
            node_sym->addInput(tensor);
            tensor->addUser(node_sym);
        }
    }

    // Process OUTPUTS using container access methods
    const ASTNode *output_container = node.getOutputListOrArray();
    bool is_output_definition = (output_container && output_container->getASTNodeType() == NodeType::OUTPUT_LIST);
    auto output_names = extractTensorNamesFromOutput(output_container);
    for (const auto &output_name : output_names)
    {
        node_output_refs_.insert(output_name);
    }

    for (const auto &output_name : output_names)
    {
        tensor_references_.insert(output_name);

        // 3. Output name conflict check
        if (auto it = output_ownership_.find(output_name); it != output_ownership_.end() && it->second != node_name)
        {
            reportError("Output conflict: '" + output_name + "' already owned by '" + it->second + "'");
            continue;
        }

        // DECLARATION vs DEFINITION handling
        if (is_output_definition)
        {
            // 2. Tensor name conflict (redefinition)
            if (defined_tensors_.count(output_name))
            {
                reportError("Tensor redefinition: '" + output_name + "'");
                continue;
            }

            // Create or reuse tensor
            TensorSymbol *tensor_sym = symbol_table_.getTensorSymbol(output_name);
            if (!tensor_sym)
            {
                symbol_table_.insertTensorSymbol(output_name, DataType::UNDEFINED, &node);
                tensor_sym = symbol_table_.getTensorSymbol(output_name);
            }

            if (tensor_sym)
            {
                // Connect node to tensor
                node_sym->addOutput(tensor_sym);
                tensor_sym->setProducer(node_sym);

                // Update definition state
                defined_tensors_.insert(output_name);
                declared_tensors_.erase(output_name);
                output_ownership_[output_name] = node_name;
            }
        }
        else
        {
            // Output declaration handling
            if (!symbol_table_.getTensorSymbol(output_name))
            {
                symbol_table_.insertTensorSymbol(output_name, DataType::UNDEFINED, nullptr);
            }
            declared_tensors_.insert(output_name);
        }
    }
}

void ASTSemanticVisitor::visit(const IOTensorNode &node)
{
    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    if (processing_model_inputs_)
    {
        input_list_defined_.insert(tensor_name);

        if (declared_tensors_.count(tensor_name) == 0)
        {
        }
    }
    else if (processing_model_outputs_)
    {
        output_list_defined_.insert(tensor_name);

        if (declared_tensors_.count(tensor_name) == 0)
        {
        }
    }

    if (!symbol_table_.getTensorSymbol(tensor_name))
    {
        symbol_table_.insertTensorSymbol(tensor_name, data_type, &node);
        auto *tensor_sym = symbol_table_.getTensorSymbol(tensor_name);
        if (processing_model_inputs_)
        {
            tensor_sym->setIsModelInput(true);
        }
        else if (processing_model_outputs_)
        {
            tensor_sym->setIsModelOutput(true);
        }
    }
}

void ASTSemanticVisitor::visit(const InitTensorNode &node)
{
    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    initializer_defined_.insert(tensor_name);

    if (!symbol_table_.getTensorSymbol(tensor_name))
    {
        symbol_table_.insertTensorSymbol(tensor_name, data_type, &node);
    }
    if (auto *tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
    {
        tensor_sym->setIsInitializer(true);
    }
}

void ASTSemanticVisitor::buildDAG()
{
    // 4. Input tensor definition check
    std::unordered_set<std::string> undefined;
    for (const auto &tensor : tensor_references_)
    {
        const bool is_initializer = initializer_tensors_.count(tensor);

        // Missing definition - report specific error
        if (!defined_tensors_.count(tensor))
        {
            reportError((is_initializer ? "Initializer must be defined: '" : "Input tensor must be defined: '") +
                        tensor + "'");
            undefined.insert(tensor);
        }
    }

    // 5. Declaration without definition
    for (const auto &tensor : declared_tensors_)
    {
        if (!defined_tensors_.count(tensor) && !undefined.count(tensor))
        {
            reportError("Declared tensor never defined: '" + tensor + "'");
        }
    }
}

void ASTSemanticVisitor::reportError(const std::string &message)
{
    errors_.push_back(message);
}

void ASTSemanticVisitor::checkNameConflict(const std::string &name, bool is_tensor)
{
    if (is_tensor)
    {
        if (symbol_table_.getTensorSymbol(name) != nullptr)
        {
            reportError("Tensor name conflict: tensor '" + name + "' already exists");
        }
    }
    else
    {
        if (symbol_table_.getNodeSymbol(name) != nullptr)
        {
            reportError("Node name conflict: node '" + name + "' already exists");
        }
    }
}

void ASTSemanticVisitor::checkUndefinedReference(const std::string &tensor_name, bool is_initializer)
{
    // Check if tensor is defined
    bool is_defined = (defined_tensors_.find(tensor_name) != defined_tensors_.end()) ||
                      (model_input_tensors_.find(tensor_name) != model_input_tensors_.end());

    if (!is_defined)
    {
        if (is_initializer || initializer_tensors_.find(tensor_name) != initializer_tensors_.end())
        {
            reportError("Undefined initializer: initializer '" + tensor_name +
                        "' must be defined before being referenced");
        }
        else
        {
            reportError("Undefined tensor: input tensor '" + tensor_name + "' must be defined before being referenced");
        }
    }
}

void ASTSemanticVisitor::checkOutputConflict(const std::string &tensor_name, const std::string &node_name)
{
    if (auto it = output_tensors_.find(tensor_name); it != output_tensors_.end())
    {
        reportError("Output tensor '" + tensor_name + "' conflict: already produced by node '" + it->second + "'");
    }
    else
    {
        output_tensors_[tensor_name] = node_name;
    }
}

void ASTSemanticVisitor::validateTypeCompatibility()
{
    // Type compatibility validation (placeholder for future implementation)
    // This would check operator-specific type constraints
    auto all_nodes = symbol_table_.getAllNodeSymbols();
    for (auto *node_symbol : all_nodes)
    {
        // Example: Check binary operators require same input types
        if (node_symbol->getOpType() == "Add" || node_symbol->getOpType() == "Sub" ||
            node_symbol->getOpType() == "Mul" || node_symbol->getOpType() == "Div")
        {

            const auto &inputs = node_symbol->getInputs();
            if (inputs.size() >= 2)
            {
                DataType first_type = inputs[0]->getDataType();
                for (size_t i = 1; i < inputs.size(); ++i)
                {
                    if (inputs[i]->getDataType() != first_type)
                    {
                        reportError("Type mismatch: operator '" + node_symbol->getOpType() +
                                    "' requires all input tensors to have the same type");
                        break;
                    }
                }
            }
        }
    }
}

void ASTSemanticVisitor::validateModelIOCompatibility()
{
    // Check model input/output compatibility (placeholder for future implementation)
    for (const auto &output_name : model_output_tensors_)
    {
        TensorSymbol *output_tensor = symbol_table_.getTensorSymbol(output_name);
        if (output_tensor && output_tensor->getProducer())
        {
            const NodeSymbol *producer = output_tensor->getProducer();
            // Additional validation could be added here based on operation types
        }
    }
}

// Helper method implementations (these would need to be implemented based on your AST structure)
std::string ASTSemanticVisitor::extractStringFromNode(const ASTNode *node)
{
    // This method should extract string value from nodes like StrLiteralNode
    // Implementation depends on your specific AST node structure
    if (node && node->getASTNodeType() == NodeType::STR_LITERAL)
    {
        const auto *str_node = dynamic_cast<const StrLiteralNode *>(node);
        return str_node->getValue(); // Assuming getValue() method exists
    }
    return "";
}

std::vector<std::string> ASTSemanticVisitor::extractStringArrayFromNode(const ASTNode *node)
{
    // This method should extract array of strings from array nodes
    // Implementation depends on your specific AST node structure
    std::vector<std::string> result;

    // This is a placeholder implementation
    // You would need to traverse the array node and extract string values

    return result;
}

DataType ASTSemanticVisitor::extractDataTypeFromNode(const ASTNode *node)
{
    if (!node)
        return DataType::UNDEFINED;

    if (node->getASTNodeType() == NodeType::TYPE_ENUM)
    {
        const auto *type_node = dynamic_cast<const TypeEnumNode *>(node);
        return type_node->getValue();
    }
    return DataType::UNDEFINED;
}

// Helper to extract strings from input containers using proper AST interfaces
std::vector<std::string> ASTSemanticVisitor::extractTensorNamesFromInput(const ASTNode *container)
{
    std::vector<std::string> names;
    if (!container)
        return names;

    switch (container->getASTNodeType())
    {
    case NodeType::INPUT_LIST: {
        const auto *list = dynamic_cast<const InputListNode *>(container);
        for (const auto &io_tensor : list->getIOTensors())
        {
            if (io_tensor->getASTNodeType() == NodeType::IO_TENSOR)
            {
                names.push_back(extractStringFromNode(dynamic_cast<const IOTensorNode *>(io_tensor.get())->getName()));
            }
        }
        break;
    }
    case NodeType::INPUT_ARR: {
        const auto *arr = dynamic_cast<const InputArrNode *>(container);
        for (const auto &item : arr->getInputElements())
        {
            if (item->getASTNodeType() == NodeType::STR_LITERAL)
            {
                names.push_back(dynamic_cast<const StrLiteralNode *>(item.get())->getValue());
            }
        }
        break;
    }
    default:
        break;
    }
    return names;
}

// Helper to extract strings from output containers using proper AST interfaces
std::vector<std::string> ASTSemanticVisitor::extractTensorNamesFromOutput(const ASTNode *container)
{
    std::vector<std::string> names;
    if (!container)
        return names;

    switch (container->getASTNodeType())
    {
    case NodeType::OUTPUT_LIST: {
        const auto *list = dynamic_cast<const OutputListNode *>(container);
        for (const auto &io_tensor : list->getIOTensors())
        {
            if (io_tensor->getASTNodeType() == NodeType::IO_TENSOR)
            {
                names.push_back(extractStringFromNode(dynamic_cast<const IOTensorNode *>(io_tensor.get())->getName()));
            }
        }
        break;
    }
    case NodeType::OUTPUT_ARR: {
        const auto *arr = dynamic_cast<const OutputArrNode *>(container);
        for (const auto &item : arr->getOutputElements())
        {
            if (item->getASTNodeType() == NodeType::STR_LITERAL)
            {
                names.push_back(dynamic_cast<const StrLiteralNode *>(item.get())->getValue());
            }
        }
        break;
    }
    default:
        break;
    }
    return names;
}

bool ASTSemanticVisitor::isTensorDefined(const std::string &tensor_name) const
{
    return input_list_defined_.count(tensor_name) > 0 || output_list_defined_.count(tensor_name) > 0 ||
           initializer_defined_.count(tensor_name) > 0 || node_output_refs_.count(tensor_name) > 0 ||
           defined_tensors_.count(tensor_name) > 0 || initializer_defined_.count(tensor_name) > 0;
}

void ASTSemanticVisitor::checkDeclarationDefinitionConsistency()
{
    // Add initializers as valid definitions for declared tensors
    for (const auto &tensor : declared_tensors_)
    {
        if (!defined_tensors_.count(tensor) && !initializer_defined_.count(tensor))
        {
            reportError("Tensor '" + tensor + "' declared but not defined");
        }
    }

    for (const auto &input_ref : node_input_refs_)
    {
        if (!isTensorDefined(input_ref))
        {
            reportError("Undefined tensor reference: '" + input_ref + "' is referenced but never defined");
        }
    }

    std::unordered_set<std::string> all_definitions;

    for (const auto &tensor : input_list_defined_)
    {
        if (all_definitions.count(tensor))
        {
            reportError("Multiple definitions: tensor '" + tensor + "' defined multiple times");
        }
        all_definitions.insert(tensor);
    }

    for (const auto &tensor : output_list_defined_)
    {
        if (all_definitions.count(tensor))
        {
            reportError("Multiple definitions: tensor '" + tensor + "' defined multiple times");
        }
        all_definitions.insert(tensor);
    }

    // Check for initializers defining declared tensors
    for (const auto &tensor : initializer_defined_)
    {
        if (declared_tensors_.count(tensor))
        {
            // Mark initializer as valid definition for declared tensor
            defined_tensors_.insert(tensor);
            // Update tensor symbol to reflect dual nature
            if (auto *t_sym = symbol_table_.getTensorSymbol(tensor))
            {
                t_sym->setIsModelInput(true); // For input_arr declared tensors
                t_sym->setIsInitializer(true);
            }
        }
    }

    for (const auto &tensor : node_output_refs_)
    {
        if (all_definitions.count(tensor))
        {
            reportError("Multiple definitions: tensor '" + tensor + "' defined multiple times");
        }
        all_definitions.insert(tensor);
    }
}

} // namespace sonnx
