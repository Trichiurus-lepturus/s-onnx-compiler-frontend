#ifndef AST_SEMANTIC_VISITOR_HPP
#define AST_SEMANTIC_VISITOR_HPP

#include "ASTBaseVisitor.hpp"
#include "utils/SymbolTable.hpp"
#include <string>
#include <unordered_set>
#include <vector>

namespace sonnx
{

class ASTSemanticVisitor final : public ASTBaseVisitor
{
  public:
    ASTSemanticVisitor()
        : processing_model_inputs_(false), processing_model_outputs_(false),
          processing_initializers_(false), current_node_name_("")
    {
    }

    ~ASTSemanticVisitor() override = default;

    // Main visitor methods
    void visit(const ModelNode &node) override;
    void visit(const NodeListNode &node) override;
    void visit(const InputListNode &node) override;
    void visit(const OutputListNode &node) override;
    void visit(const InitializerListNode &node) override;
    void visit(const NodeNode &node) override;
    void visit(const InputArrNode &node) override;
    void visit(const OutputArrNode &node) override;
    void visit(const AttributeListNode &node) override;
    void visit(const IOTensorNode &node) override;
    void visit(const InitTensorNode &node) override;

    // Other required visit methods (from base class)
    void visit(const U32LiteralNode &node) override {}
    void visit(const U64LiteralNode &node) override {}
    void visit(const StrLiteralNode &node) override {}
    void visit(const BytesLiteralNode &node) override {}
    void visit(const TypeEnumNode &node) override {}
    void visit(const AttributeNode &node) override {}
    void visit(const IOShapeNode &node) override {}
    void visit(const IODimNode &node) override {}
    void visit(const InitShapeNode &node) override {}
    void visit(const ErrorNode &node) override {}

    // Analysis results access
    const std::vector<std::string> &getErrors() const
    {
        return errors_;
    }
    bool hasErrors() const
    {
        return !errors_.empty();
    }
    SymbolTable &getSymbolTable()
    {
        return symbol_table_;
    }
    const SymbolTable &getSymbolTable() const
    {
        return symbol_table_;
    }

  private:
    // Symbol table
    SymbolTable symbol_table_;

    // Error tracking
    std::vector<std::string> errors_;

    // Processing state
    bool processing_model_inputs_;
    bool processing_model_outputs_;
    bool processing_initializers_;
    std::string current_node_name_;

    // Tracking sets
    std::unordered_set<std::string> defined_tensors_;
    std::unordered_set<std::string> model_input_names_;
    std::unordered_set<std::string> model_output_names_;
    std::unordered_set<std::string> initializer_defined_;

    // Temporary storage for current node
    std::vector<std::string> node_input_refs_;
    std::vector<std::string> node_output_refs_;

    // Helper methods
    void reportError(const std::string &message);
    static std::string extractStringFromNode(const ASTNode *node);
    static DataType extractDataTypeFromNode(const ASTNode *node);
    void performSemanticAnalysis();
    void checkUndefinedInputs();
    void checkUnusedOutputs() const;
    void runDAGAnalysis();
};

} // namespace sonnx

#endif // AST_SEMANTIC_VISITOR_HPP
