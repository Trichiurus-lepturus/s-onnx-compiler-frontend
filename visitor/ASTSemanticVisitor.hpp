#ifndef AST_SEMANTIC_VISITOR_HPP
#define AST_SEMANTIC_VISITOR_HPP

#include "ASTBaseVisitor.hpp"
#include "utils/SymbolTable.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sonnx
{

class ASTSemanticVisitor final : public Visitor
{
  public:
    ASTSemanticVisitor()
        : processing_model_inputs_(false), processing_model_outputs_(false), processing_initializers_(false)
    {
    }

    ~ASTSemanticVisitor() = default;

    // Main visit methods for model structure
    void visit(const ModelNode &node) override;
    void visit(const NodeListNode &node) override;
    void visit(const InputListNode &node) override;
    void visit(const OutputListNode &node) override;
    void visit(const InitializerListNode &node) override;
    void visit(const NodeNode &node) override;
    void visit(const InputArrNode &node) override;
    void visit(const OutputArrNode &node) override;
    void visit(const IOTensorNode &node) override;
    void visit(const InitTensorNode &node) override;

    // Other required visit methods (from base class)
    void visit(const U32LiteralNode &node) override
    {
    }
    void visit(const U64LiteralNode &node) override
    {
    }
    void visit(const StrLiteralNode &node) override
    {
    }
    void visit(const BytesLiteralNode &node) override
    {
    }
    void visit(const TypeEnumNode &node) override
    {
    }
    void visit(const AttributeListNode &node) override
    {
    }
    void visit(const AttributeNode &node) override
    {
    }
    void visit(const IOShapeNode &node) override
    {
    }
    void visit(const IODimNode &node) override
    {
    }
    void visit(const InitShapeNode &node) override
    {
    }
    void visit(const ErrorNode &node) override
    {
    }

    // Get semantic analysis results
    const std::vector<std::string> &getErrors() const
    {
        return errors_;
    }
    bool hasErrors() const
    {
        return !errors_.empty();
    }
    const SymbolTable &getSymbolTable() const
    {
        return symbol_table_;
    }

  private:
    // Symbol table for storing all symbols
    SymbolTable symbol_table_;

    // Add placeholder for undefined type
    static constexpr DataType UNDEFINED = static_cast<DataType>(-1); // Fix: Declaration
    // Error collection
    std::vector<std::string> errors_;

    // Processing state flags
    bool processing_model_inputs_;
    bool processing_model_outputs_;
    bool processing_initializers_;

    // Track tensor declarations and definitions
    std::unordered_set<std::string> declared_tensors_;    // For input/output declarations
    std::unordered_set<std::string> defined_tensors_;     // For actual tensor definitions
    std::unordered_set<std::string> initializer_tensors_; // For initializer tensors
    std::unordered_set<std::string> model_input_tensors_;
    std::unordered_set<std::string> model_output_tensors_;
    std::unordered_set<std::string> tensor_references_;             // Fix: Declaration
    std::unordered_map<std::string, std::string> output_ownership_; // Fix: Declaration
    std::unordered_set<std::string> input_list_defined_;
    std::unordered_set<std::string> output_list_defined_;
    std::unordered_set<std::string> initializer_defined_;
    std::unordered_set<std::string> node_input_refs_;
    std::unordered_set<std::string> node_output_refs_;
    std::unordered_map<std::string, std::string> output_tensors_; // tensor_name -> node_name

    // Helper methods for semantic analysis
    void reportError(const std::string &message);
    void checkNameConflict(const std::string &name, bool is_tensor);
    void checkUndefinedReference(const std::string &tensor_name, bool is_initializer = false);
    void checkOutputConflict(const std::string &tensor_name, const std::string &node_name);
    void buildDAG();
    void validateTypeCompatibility();
    void validateModelIOCompatibility();
    void checkDeclarationDefinitionConsistency();
    bool isTensorDefined(const std::string &tensor_name) const;

    // Helper methods to extract information from AST nodes
    static std::string extractStringFromNode(const ASTNode *node);
    static std::vector<std::string> extractStringArrayFromNode(const ASTNode *node);
    static DataType extractDataTypeFromNode(const ASTNode *node);
    static std::vector<std::string> extractTensorNamesFromInput(const ASTNode *container);
    static std::vector<std::string> extractTensorNamesFromOutput(const ASTNode *container);
};

} // namespace sonnx

#endif // AST_SEMANTIC_VISITOR_HPP
