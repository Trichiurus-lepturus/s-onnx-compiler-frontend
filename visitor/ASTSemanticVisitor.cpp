#include "ASTSemanticVisitor.hpp"

#include <iomanip>
#include <sstream>

// #define DEBUG_IO_CONSISTENCY

#ifdef DEBUG_IO_CONSISTENCY
#include <iostream>
#endif
#include <iostream>
namespace sonnx
{

void ASTSemanticVisitor::visit(const ModelNode &node)
{
    if (should_terminate_analysis_)
        return;

    // Pass 1: Collect all declarations from nodes
    is_declaration_pass_ = true;
    if (node.getNodeList())
    {
        node.getNodeList()->accept(*this);
    }

    // Pass 2: Process all definitions and validate references
    is_declaration_pass_ = false;
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

    if (node.getOutputList())
    {
        processing_model_outputs_ = true;
        node.getOutputList()->accept(*this);
        processing_model_outputs_ = false;
    }

    if (node.getNodeList())
    {
        node.getNodeList()->accept(*this);
    }

    // Perform semantic analysis after both passes
    performSemanticAnalysis();
}

void ASTSemanticVisitor::visit(const NodeListNode &node)
{
    if (should_terminate_analysis_)
        return;

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
    if (should_terminate_analysis_)
        return;

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
    if (should_terminate_analysis_)
        return;

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
    if (should_terminate_analysis_)
        return;

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
    if (should_terminate_analysis_)
        return;

    std::string node_name = extractStringFromNode(node.getName());
    std::string op_type = extractStringFromNode(node.getOpType());

    if (node_name.empty())
    {
        reportError("Empty node name");
        return;
    }

    node_input_refs_.insert({node_name, {}});
    node_output_refs_.insert({node_name, {}});
    current_node_name_ = node_name;

    // Process inputs and outputs to collect references
    if (node.getInputListOrArray())
    {
        node.getInputListOrArray()->accept(*this);
    }

    if (node.getOutputListOrArray())
    {
        node.getOutputListOrArray()->accept(*this);
    }

    if (is_declaration_pass_)
    {
        // First pass: register the node
        if (!symbol_table_.insertNodeSymbol(node_name, op_type, &node))
        {
            reportError("Duplicate node definition: " + node_name);
        }
    }
    else
    {
        // Second pass: validate and link inputs/outputs

        // IMPORTANT: Get the node symbol first
        NodeSymbol *node_symbol = symbol_table_.getNodeSymbol(node_name);
        if (!node_symbol)
        {
            reportError("Node symbol not found for: " + node_name);
            return;
        }

        // Validate and link inputs
        for (const auto &input_ref : node_input_refs_.at(node_name))
        {
            if (input_ref.empty())
            {
                // Empty string is allowed for optional inputs
                continue;
            }

            if (auto *tensor = symbol_table_.getTensorSymbol(input_ref))
            {
                // FIX: Actually add the input to the node
                node_symbol->addInput(tensor);
                tensor->addUser(node_symbol);
            }
            else
            {
                reportError("Node '" + node_name + "' references undefined input: " + input_ref);
            }
        }

        // Validate and link outputs
        for (const auto &output_ref : node_output_refs_.at(node_name))
        {
            if (output_ref.empty())
            {
                reportError("Empty output reference in node: " + node_name);
                continue;
            }

            if (auto *tensor = symbol_table_.getTensorSymbol(output_ref))
            {
                // Check if this tensor already has a producer
                if (tensor->getProducer() && tensor->getProducer() != node_symbol)
                {
                    reportError("Tensor '" + output_ref + "' is already produced by node '" +
                                tensor->getProducer()->getName() + "', cannot be produced by '" + node_name + "'");
                }
                else
                {
                    // FIX: Actually add the output to the node and set producer
                    node_symbol->addOutput(tensor);
                    tensor->setProducer(node_symbol);
                }
            }
            else
            {
                // Output tensor doesn't exist, create it
                if (symbol_table_.insertTensorSymbol(output_ref, DataType::UNDEFINED, &node))
                {
                    if (auto *tensor_symbol = symbol_table_.getTensorSymbol(output_ref))
                    {
                        // FIX: Add the newly created tensor as output
                        node_symbol->addOutput(tensor_symbol);
                        tensor_symbol->setProducer(node_symbol);
                    }
                }
                else
                {
                    reportError("Failed to create output tensor: " + output_ref);
                }
            }
        }

        // Process attributes if any
        if (node.getAttributeList())
        {
            node.getAttributeList()->accept(*this);
        }
    }

    // Validate I/O consistency
    validateNodeIOTypeConsistency(node_name, op_type);

    current_node_name_ = "";
}

void ASTSemanticVisitor::visit(const InputArrNode &node)
{
    if (should_terminate_analysis_ | !is_declaration_pass_)
        return;

    for (const auto &input : node.getInputElements())
    {
        if (input)
        {
            std::string input_name = extractStringFromNode(input.get());
            if (!input_name.empty())
            {
                node_input_refs_.at(current_node_name_).push_back(input_name);
            }
        }
    }
}

void ASTSemanticVisitor::visit(const OutputArrNode &node)
{
    if (should_terminate_analysis_ | !is_declaration_pass_)
        return;

    for (const auto &output : node.getOutputElements())
    {
        if (output)
        {
            std::string output_name = extractStringFromNode(output.get());
            if (!output_name.empty())
            {
                node_output_refs_.at(current_node_name_).push_back(output_name);
            }
        }
    }
}

void ASTSemanticVisitor::visit(const AttributeListNode &node)
{
    if (should_terminate_analysis_)
        return;

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
    if (should_terminate_analysis_)
        return;

    if (is_declaration_pass_ && !processing_node_tensors_)
        return;

    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    if (tensor_name.empty())
    {
        reportError("Empty tensor name in IOTensor");
        return;
    }

    // FIX: Check if tensor exists (might be created as placeholder)
    auto *existing_tensor = symbol_table_.getTensorSymbol(tensor_name);
    if (existing_tensor && existing_tensor->getDataType() != DataType::UNDEFINED)
    {
        reportError("Duplicate tensor definition: '" + tensor_name + "'");
        return;
    }

    // Insert or update tensor symbol
    if (existing_tensor)
    {
        // Update existing placeholder with actual type
        // Note: This requires adding an update method to SymbolTable
    }
    else if (!symbol_table_.insertTensorSymbol(tensor_name, data_type, &node))
    {
        reportError("Failed to insert tensor: '" + tensor_name + "'");
        return;
    }

    defined_tensors_.insert(tensor_name);

    // Store shape string for TACode generation
    if (auto *tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
    {
        std::string shape_str = convertIOShapeToString(dynamic_cast<const IOShapeNode *>(node.getIOShape()));
        tensor_sym->setShapeString(shape_str);

        if (processing_model_inputs_)
        {
            tensor_sym->setIsModelInput(true);
            model_input_names_.insert(tensor_name);
        }
        else if (processing_model_outputs_)
        {
            tensor_sym->setIsModelOutput(true);
            model_output_names_.insert(tensor_name);
        }
    }
}

void ASTSemanticVisitor::visit(const InitTensorNode &node)
{
    if (should_terminate_analysis_)
        return;

    if (is_declaration_pass_)
        return;

    std::string tensor_name = extractStringFromNode(node.getName());
    DataType data_type = extractDataTypeFromNode(node.getType());

    if (tensor_name.empty())
    {
        reportError("Empty tensor name in InitTensor");
        return;
    }

    if (initializer_defined_.find(tensor_name) != initializer_defined_.end())
    {
        reportError("Duplicate initializer: '" + tensor_name + "'");
        return;
    }

    if (!symbol_table_.insertTensorSymbol(tensor_name, data_type, &node))
    {
        reportError("Failed to insert initializer: '" + tensor_name + "'");
    }
    else
    {
        initializer_defined_.insert(tensor_name);
        defined_tensors_.insert(tensor_name);

        if (auto *tensor_sym = symbol_table_.getTensorSymbol(tensor_name))
        {
            tensor_sym->setIsInitializer(true);

            // Convert and store shape
            std::string shape_str = convertInitShapeToString(dynamic_cast<const InitShapeNode *>(node.getInitShape()));
            tensor_sym->setShapeString(shape_str);

            // Convert and store raw data as hex
            if (auto *bytes_node = dynamic_cast<const BytesLiteralNode *>(node.getRawData()))
            {
                std::string hex_str = convertBytesToHexString(bytes_node);
                tensor_sym->setRawDataHex(hex_str);
            }
        }
    }
}

void ASTSemanticVisitor::performSemanticAnalysis()
{
    // Check that all pre-declared tensors were eventually defined
    for (const auto &tensor_name : pre_declared_tensors_)
    {
        if (defined_tensors_.find(tensor_name) == defined_tensors_.end() &&
            initializer_defined_.find(tensor_name) == initializer_defined_.end())
        {
            reportError("Tensor '" + tensor_name + "' was declared but never defined");
        }
    }

    // Validate model outputs exist
    for (const auto &output_name : model_output_names_)
    {
        if (symbol_table_.getTensorSymbol(output_name) == nullptr)
        {
            reportError("Model output tensor not found: '" + output_name + "'");
        }
    }

    // Build DAG for topological analysis
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

void ASTSemanticVisitor::reportError(const std::string &message, bool terminate)
{
    errors_.push_back(message);
    if (terminate)
    {
        should_terminate_analysis_ = true;
    }
}

std::string ASTSemanticVisitor::extractStringFromNode(const ASTNode *node)
{
    if (!node)
        return "";

    if (auto *str_node = dynamic_cast<const StrLiteralNode *>(node))
    {
        return str_node->getValue();
    }
    return "";
}

DataType ASTSemanticVisitor::extractDataTypeFromNode(const ASTNode *node)
{
    if (!node)
        return DataType::UNDEFINED;

    if (auto *type_node = dynamic_cast<const TypeEnumNode *>(node))
    {
        return type_node->getValue();
    }
    return DataType::UNDEFINED;
}

inline auto nodeTypeToString(const NodeType type) -> std::string
{
    switch (type)
    {
    case NodeType::MODEL:
        return "MODEL";
    case NodeType::NODE_LIST:
        return "NODE_LIST";
    case NodeType::INPUT_LIST:
        return "INPUT_LIST";
    case NodeType::OUTPUT_LIST:
        return "OUTPUT_LIST";
    case NodeType::INITIALIZER_LIST:
        return "INITIALIZER_LIST";
    case NodeType::NODE:
        return "NODE";
    case NodeType::INPUT_ARR:
        return "INPUT_ARR";
    case NodeType::OUTPUT_ARR:
        return "OUTPUT_ARR";
    case NodeType::ATTRIBUTE_LIST:
        return "ATTRIBUTE_LIST";
    case NodeType::ATTRIBUTE:
        return "ATTRIBUTE";
    case NodeType::IO_TENSOR:
        return "IO_TENSOR";
    case NodeType::IO_SHAPE:
        return "IO_SHAPE";
    case NodeType::IO_DIM:
        return "IO_DIM";
    case NodeType::INIT_TENSOR:
        return "INIT_TENSOR";
    case NodeType::INIT_SHAPE:
        return "INIT_SHAPE";
    case NodeType::U32_LITERAL:
        return "U32_LITERAL";
    case NodeType::U64_LITERAL:
        return "U64_LITERAL";
    case NodeType::STR_LITERAL:
        return "STR_LITERAL";
    case NodeType::BYTES_LITERAL:
        return "BYTES_LITERAL";
    case NodeType::TYPE_ENUM:
        return "TYPE_ENUM";
    case NodeType::ERROR_NODE:
        return "ERROR_NODE";
    default:
        return "UNKNOWN";
    }
}

std::string ASTSemanticVisitor::convertIOShapeToString(const IOShapeNode *shape_node)
{
    if (!shape_node)
        return "[]";

    std::string result = "[";
    const auto &dims = shape_node->getIODims();

    for (size_t i = 0; i < dims.size(); ++i)
    {
        if (i > 0)
            result += ", ";

        if (dims[i]->getASTNodeType() == NodeType::U32_LITERAL)
        {
            auto *u32_node = dynamic_cast<const U32LiteralNode *>(dims[i].get());
            result += std::to_string(u32_node->getValue());
        }
        else if (dims[i]->getASTNodeType() == NodeType::U64_LITERAL)
        {
            auto *u64_node = dynamic_cast<const U64LiteralNode *>(dims[i].get());
            result += std::to_string(u64_node->getValue());
        }
        else if (dims[i]->getASTNodeType() == NodeType::STR_LITERAL)
        {
            auto *str_node = dynamic_cast<const StrLiteralNode *>(dims[i].get());
            result += "\"" + str_node->getValue() + "\"";
        }
    }

    result += "]";
    return result;
}

std::string ASTSemanticVisitor::convertInitShapeToString(const InitShapeNode *shape_node)
{
    if (!shape_node)
        return "[]";

    std::string result = "[";
    const auto &dims = shape_node->getDimValues();

    for (size_t i = 0; i < dims.size(); ++i)
    {
        if (i > 0)
            result += ", ";

        if (dims[i]->getASTNodeType() == NodeType::U32_LITERAL)
        {
            auto *u32_node = dynamic_cast<const U32LiteralNode *>(dims[i].get());
            result += std::to_string(u32_node->getValue());
        }
        else if (dims[i]->getASTNodeType() == NodeType::U64_LITERAL)
        {
            auto *u64_node = dynamic_cast<const U64LiteralNode *>(dims[i].get());
            result += std::to_string(u64_node->getValue());
        }
    }

    result += "]";

    return result;
}

std::string ASTSemanticVisitor::convertBytesToHexString(const BytesLiteralNode *bytes_node)
{
    if (!bytes_node)
        return "0x";

    std::stringstream ss;
    ss << "0x";

    const auto &bytes = bytes_node->getValue();
    for (uint8_t byte : bytes)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }

    return ss.str();
}

std::string ASTSemanticVisitor::convertAttributesToString(const AttributeListNode *attr_list)
{
    if (!attr_list)
        return "";

    std::string result;
    const auto &attrs = attr_list->getAttributes();

    for (size_t i = 0; i < attrs.size(); ++i)
    {
        if (i > 0)
            result += ", ";

        if (auto *attr = dynamic_cast<const AttributeNode *>(attrs[i].get()))
        {
            std::string name = extractStringFromNode(attr->getName());
            std::string value = extractStringFromNode(attr->getValue());
            result += name + "=" + value;
        }
    }

    return result;
}

void ASTSemanticVisitor::validateNodeIOTypeConsistency(const std::string &node_name, const std::string &op_type)
{
#ifdef DEBUG_IO_CONSISTENCY
    std::cout << "DEBUG: validateNodeIOTypeConsistency - Node: " << node_name << ", op_type: " << op_type << std::endl;
    std::cout << "DEBUG: Input refs count: " << node_input_refs_.size() << std::endl;
    std::cout << "DEBUG: Output refs count: " << node_output_refs_.size() << std::endl;
#endif

    // Collect all tensor types from inputs and outputs
    DataType node_type;
    bool type_set = false;

#ifdef DEBUG_IO_CONSISTENCY
    std::cout << "DEBUG: Checking input types:" << std::endl;
#endif

    // Check input types
    for (const auto &input_ref : node_input_refs_.at(node_name))
    {
        if (auto *tensor = symbol_table_.getTensorSymbol(input_ref))
        {
#ifdef DEBUG_IO_CONSISTENCY
            std::cout << "DEBUG: Input tensor '" << input_ref << "' type: " << static_cast<int>(tensor->getDataType())
                      << std::endl;
#endif
            if (!type_set)
            {
                node_type = tensor->getDataType();
                type_set = true;
#ifdef DEBUG_IO_CONSISTENCY
                std::cout << "DEBUG: Set reference type to: " << static_cast<int>(node_type) << std::endl;
#endif
            }
            else if (tensor->getDataType() != node_type)
            {
#ifdef DEBUG_IO_CONSISTENCY
                std::cout << "DEBUG: Type mismatch detected - expected: " << static_cast<int>(node_type)
                          << ", found: " << static_cast<int>(tensor->getDataType()) << std::endl;
#endif
                reportError("Type mismatch in node '" + node_name + "' (op_type: " + op_type + "): input tensor '" +
                            input_ref + "' has different type than other tensors in this node");
                return;
            }
        }
    }

#ifdef DEBUG_IO_CONSISTENCY
    std::cout << "DEBUG: Checking output types:" << std::endl;
#endif

    // Check output types
    for (const auto &output_ref : node_output_refs_.at(node_name))
    {
        if (auto *tensor = symbol_table_.getTensorSymbol(output_ref))
        {
#ifdef DEBUG_IO_CONSISTENCY
            std::cout << "DEBUG: Output tensor '" << output_ref << "' type: " << static_cast<int>(tensor->getDataType())
                      << std::endl;
#endif
            if (!type_set)
            {
                node_type = tensor->getDataType();
                type_set = true;
#ifdef DEBUG_IO_CONSISTENCY
                std::cout << "DEBUG: Set reference type to: " << static_cast<int>(node_type) << std::endl;
#endif
            }
            else if (tensor->getDataType() != node_type)
            {
#ifdef DEBUG_IO_CONSISTENCY
                std::cout << "DEBUG: Type mismatch detected - expected: " << static_cast<int>(node_type)
                          << ", found: " << static_cast<int>(tensor->getDataType()) << std::endl;
#endif
                reportError("Type mismatch in node '" + node_name + "' (op_type: " + op_type + "): output tensor '" +
                            output_ref + "' has different type than other tensors in this node");
                return;
            }
        }
    }

#ifdef DEBUG_IO_CONSISTENCY
    std::cout << "DEBUG: Type consistency check passed for node: " << node_name << std::endl;
#endif
}

} // namespace sonnx
