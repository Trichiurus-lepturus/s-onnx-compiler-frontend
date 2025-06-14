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

  public:
    TensorSymbol(std::string name, DataType dtype, const ASTNode *def)
        : BaseSymbol(std::move(name), def), dtype_(dtype)
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
    void detectDeadCode();
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
};

} // namespace sonnx

#endif // SYMBOL_TABLE_HPP
