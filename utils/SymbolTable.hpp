#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "ast/AST.hpp"
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sonnx
{

class BaseSymbol
{
  protected:
    const std::string name_;
    const ASTNode *definition_;
    BaseSymbol(std::string name, const ASTNode *def) : name_(std::move(name)), definition_(def)
    {
    }

  public:
    virtual ~BaseSymbol() = default;
    const std::string &getName() const
    {
        return name_;
    }
    const ASTNode *getDefinition() const
    {
        return definition_;
    }
};

class TensorSymbol;
class NodeSymbol final : public BaseSymbol
{
    std::string op_type_;
    std::vector<const TensorSymbol *> inputs_;
    std::vector<const TensorSymbol *> outputs_;

    std::string attributes_string_; // For storing attributes as "kernel_shape=[3, 3], strides=[1, 1]"

  public:
    NodeSymbol(std::string name, std::string op_type, const ASTNode *def)
        : BaseSymbol(std::move(name), def), op_type_(std::move(op_type))
    {
    }

    const std::string &getOpType() const
    {
        return op_type_;
    }
    void addInput(TensorSymbol *tensor);
    void addOutput(TensorSymbol *tensor);
    const std::vector<const TensorSymbol *> &getInputs() const
    {
        return inputs_;
    }
    const std::vector<const TensorSymbol *> &getOutputs() const
    {
        return outputs_;
    }

    void setAttributesString(const std::string &attrs)
    {
        attributes_string_ = attrs;
    }
    const std::string &getAttributesString() const
    {
        return attributes_string_;
    }
};

class TensorSymbol final : public BaseSymbol
{
    DataType dtype_;
    NodeSymbol *producer_ = nullptr;
    std::vector<NodeSymbol *> users_;

    // Tensor properties
    bool is_initializer_ = false;
    bool is_model_input_ = false;
    bool is_model_output_ = false;

    std::string shape_string_; // For storing shape as "[1, 3, 224, 224]"
    std::string raw_data_hex_; // For storing raw data as "0x..."

  public:
    TensorSymbol(std::string name, DataType dtype, const ASTNode *def) : BaseSymbol(std::move(name), def), dtype_(dtype)
    {
    }

    DataType getDataType() const
    {
        return dtype_;
    }
    void setProducer(NodeSymbol *node)
    {
        producer_ = node;
    }
    NodeSymbol *getProducer() const
    {
        return producer_;
    }
    void addUser(NodeSymbol *node)
    {
        users_.push_back(node);
    }
    const std::vector<NodeSymbol *> &getUsers() const
    {
        return users_;
    }

    bool isInitializer() const
    {
        return is_initializer_;
    }
    bool isModelInput() const
    {
        return is_model_input_;
    }
    bool isModelOutput() const
    {
        return is_model_output_;
    }
    void setIsInitializer(bool value)
    {
        is_initializer_ = value;
    }
    void setIsModelInput(bool value)
    {
        is_model_input_ = value;
    }
    void setIsModelOutput(bool value)
    {
        is_model_output_ = value;
    }

    void setShapeString(const std::string &shape)
    {
        shape_string_ = shape;
    }
    const std::string &getShapeString() const
    {
        return shape_string_;
    }

    void setRawDataHex(const std::string &hex)
    {
        raw_data_hex_ = hex;
    }
    const std::string &getRawDataHex() const
    {
        return raw_data_hex_;
    }
};

class SymbolTable
{
  private:
    std::unordered_map<std::string, std::unique_ptr<BaseSymbol>> symbols_;

    // DAG structure
    std::map<NodeSymbol *, std::vector<NodeSymbol *>> dag_edges_;
    std::map<NodeSymbol *, std::vector<NodeSymbol *>> reverse_dag_edges_;
    std::vector<NodeSymbol *> topological_order_;
    bool has_cycle_ = false;

    // Helper for topological sort
    bool topologicalSortDFS(NodeSymbol *node, std::unordered_set<NodeSymbol *> &visited,
                            std::unordered_set<NodeSymbol *> &recursion_stack);

  public:
    // Symbol management
    bool insertNodeSymbol(const std::string &name, const std::string &op_type, const ASTNode *def);
    bool insertTensorSymbol(const std::string &name, DataType dtype, const ASTNode *def);
    BaseSymbol *lookup(const std::string &name);
    NodeSymbol *getNodeSymbol(const std::string &name);
    TensorSymbol *getTensorSymbol(const std::string &name);

    // Collection access
    std::vector<NodeSymbol *> getAllNodeSymbols() const;
    std::vector<TensorSymbol *> getAllTensorSymbols() const;

    // DAG construction and analysis
    void buildDAG();
    void performTopologicalSort();
    void detectConstantFolding();
    void detectDeadCode() const;
    void detectCommonSubexpressions();

    // DAG access
    const std::vector<NodeSymbol *> &getTopologicalOrder() const
    {
        return topological_order_;
    }
    bool hasCycle() const
    {
        return has_cycle_;
    }
    const std::map<NodeSymbol *, std::vector<NodeSymbol *>> &getDAGEdges() const
    {
        return dag_edges_;
    }

    void clear();

    std::string generateTACode() const;

private:
    static std::string dataTypeToString(DataType dtype);
    mutable int t_variable_counter_ = 1;
    mutable std::unordered_map<std::string, std::string> tensor_to_t_mapping_;
    std::string getOrCreateTVariableName(const std::string& original_name) const;
    static bool isModelInputOrOutput(const TensorSymbol* tensor) ;
};

} // namespace sonnx

#endif // SYMBOL_TABLE_HPP
