#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "ast/AST.hpp"
#include <string>
#include <unordered_map>
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
    std::string op_type_; // Added op_type field
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

    const auto &getInputs() const
    {
        return inputs_;
    }
    const auto &getOutputs() const
    {
        return outputs_;
    }

    void addInput(TensorSymbol *tensor);
    void addOutput(TensorSymbol *tensor);
};

class TensorSymbol final : public BaseSymbol
{
    const DataType data_type_;
    const NodeSymbol *producer_ = nullptr;
    std::vector<const NodeSymbol *> users_;
    bool is_initializer_ = false;
    bool is_model_input_ = false;
    bool is_model_output_ = false;

  public:
    TensorSymbol(std::string name, const DataType dtype, const ASTNode *def)
        : BaseSymbol(std::move(name), def), data_type_(dtype)
    {
    }
    DataType getDataType() const
    {
        return data_type_;
    }
    const NodeSymbol *getProducer() const
    {
        return producer_;
    }
    const auto &getUsers() const
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

    void setProducer(const NodeSymbol *node);
    void addUser(const NodeSymbol *node);
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

// Enhanced SymbolTable.hpp additions
class SymbolTable
{
  private:
    std::unordered_map<std::string, std::unique_ptr<BaseSymbol>> symbols_;

  public:
    bool insertNodeSymbol(const std::string &name, const std::string &op_type, const ASTNode *def);
    bool insertTensorSymbol(const std::string &name, DataType dtype, const ASTNode *def);
    BaseSymbol *lookup(const std::string &name);
    NodeSymbol *getNodeSymbol(const std::string &name);
    TensorSymbol *getTensorSymbol(const std::string &name);

    // Additional methods for semantic analysis
    std::vector<NodeSymbol *> getAllNodeSymbols();
    std::vector<TensorSymbol *> getAllTensorSymbols();
    void clear();
    void buildEdges(NodeSymbol *node, const std::vector<std::string> &inputs, const std::vector<std::string> &outputs);
};

} // namespace sonnx

#endif // SYMBOL_TABLE_HPP
