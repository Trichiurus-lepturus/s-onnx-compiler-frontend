#ifndef AST_HPP
#define AST_HPP

#include "visitor/ASTBaseVisitor.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sonnx
{

enum class NodeType
{
    MODEL,
    NODE_LIST,
    INPUT_LIST,
    OUTPUT_LIST,
    INITIALIZER_LIST,
    NODE,
    INPUT_ARR,
    OUTPUT_ARR,
    ATTRIBUTE_LIST,
    ATTRIBUTE,
    IO_TENSOR,
    IO_SHAPE,
    IO_DIM,
    INIT_TENSOR,
    INIT_SHAPE,
    U32_LITERAL,
    U64_LITERAL,
    STR_LITERAL,
    BYTES_LITERAL,
    TYPE_ENUM,
    ERROR_NODE
};

enum class DataType
{
    INT,
    FLOAT,
    STRING,
    BOOL,
    UNDEFINED
};

class ASTNode
{
  public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    ASTNode(const ASTNode &) = delete;
    auto operator=(const ASTNode &) -> ASTNode & = delete;
    ASTNode(ASTNode &&) = delete;
    auto operator=(ASTNode &&) -> ASTNode & = delete;
    [[nodiscard]] virtual auto getASTNodeType() const -> NodeType = 0;
    virtual void accept(class ASTBaseVisitor &visitor) const = 0;
};

class U32LiteralNode final : public ASTNode
{
  public:
    explicit U32LiteralNode(uint32_t value) : value_(value)
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::U32_LITERAL;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> uint32_t
    {
        return value_;
    }

  private:
    uint32_t value_;
};

class U64LiteralNode final : public ASTNode
{
  public:
    explicit U64LiteralNode(uint64_t value) : value_(value)
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::U64_LITERAL;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> uint64_t
    {
        return value_;
    }

  private:
    uint64_t value_;
};

class StrLiteralNode final : public ASTNode
{
  public:
    explicit StrLiteralNode(std::string value) : value_(std::move(value))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::STR_LITERAL;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> std::string
    {
        return value_;
    }

  private:
    std::string value_;
};

class BytesLiteralNode final : public ASTNode
{
  public:
    explicit BytesLiteralNode(std::vector<uint8_t> value) : value_(std::move(value))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::BYTES_LITERAL;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> const std::vector<uint8_t> &
    {
        return value_;
    }

  private:
    std::vector<uint8_t> value_;
};

class TypeEnumNode final : public ASTNode
{
  public:
    explicit TypeEnumNode(DataType type) : value_(type)
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::TYPE_ENUM;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> DataType
    {
        return value_;
    }

  private:
    DataType value_;
};

class ModelNode final : public ASTNode
{
  public:
    ModelNode(std::unique_ptr<ASTNode> ir_version, std::unique_ptr<ASTNode> producer_name,
              std::unique_ptr<ASTNode> producer_version, std::unique_ptr<ASTNode> model_domain,
              std::unique_ptr<ASTNode> model_version, std::unique_ptr<ASTNode> doc_string,
              std::unique_ptr<ASTNode> graph_name, std::unique_ptr<ASTNode> node_list,
              std::unique_ptr<ASTNode> input_list, std::unique_ptr<ASTNode> output_list,
              std::unique_ptr<ASTNode> initializer_list, std::unique_ptr<ASTNode> opset_domain,
              std::unique_ptr<ASTNode> opset_version)
        : ir_version_(std::move(ir_version)), producer_name_(std::move(producer_name)),
          producer_version_(std::move(producer_version)), model_domain_(std::move(model_domain)),
          model_version_(std::move(model_version)), doc_string_(std::move(doc_string)),
          graph_name_(std::move(graph_name)), node_list_(std::move(node_list)), input_list_(std::move(input_list)),
          output_list_(std::move(output_list)), initializer_list_(std::move(initializer_list)),
          opset_domain_(std::move(opset_domain)), opset_version_(std::move(opset_version))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::MODEL;
    }
    void accept(ASTBaseVisitor &visitor) const override;

    [[nodiscard]] auto getIrVersion() const -> const ASTNode *
    {
        return ir_version_.get();
    }
    [[nodiscard]] auto getProducerName() const -> const ASTNode *
    {
        return producer_name_.get();
    }
    [[nodiscard]] auto getProducerVersion() const -> const ASTNode *
    {
        return producer_version_.get();
    }
    [[nodiscard]] auto getDomain() const -> const ASTNode *
    {
        return model_domain_.get();
    }
    [[nodiscard]] auto getModelVersion() const -> const ASTNode *
    {
        return model_version_.get();
    }
    [[nodiscard]] auto getDocString() const -> const ASTNode *
    {
        return doc_string_.get();
    }
    [[nodiscard]] auto getGraphName() const -> const ASTNode *
    {
        return graph_name_.get();
    }
    [[nodiscard]] auto getNodeList() const -> const ASTNode *
    {
        return node_list_.get();
    }
    [[nodiscard]] auto getInputList() const -> const ASTNode *
    {
        return input_list_.get();
    }
    [[nodiscard]] auto getOutputList() const -> const ASTNode *
    {
        return output_list_.get();
    }
    [[nodiscard]] auto getInitializerList() const -> const ASTNode *
    {
        return initializer_list_.get();
    }
    [[nodiscard]] auto getOpsetDomain() const -> const ASTNode *
    {
        return opset_domain_.get();
    }
    [[nodiscard]] auto getOpsetVersion() const -> const ASTNode *
    {
        return opset_version_.get();
    }

  private:
    std::unique_ptr<ASTNode> ir_version_;
    std::unique_ptr<ASTNode> producer_name_;
    std::unique_ptr<ASTNode> producer_version_;
    std::unique_ptr<ASTNode> model_domain_;
    std::unique_ptr<ASTNode> model_version_;
    std::unique_ptr<ASTNode> doc_string_;
    std::unique_ptr<ASTNode> graph_name_;
    std::unique_ptr<ASTNode> node_list_;
    std::unique_ptr<ASTNode> input_list_;
    std::unique_ptr<ASTNode> output_list_;
    std::unique_ptr<ASTNode> initializer_list_;
    std::unique_ptr<ASTNode> opset_domain_;
    std::unique_ptr<ASTNode> opset_version_;
};

class NodeListNode final : public ASTNode
{
  public:
    explicit NodeListNode(std::vector<std::unique_ptr<ASTNode>> nodes) : nodes_(std::move(nodes))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::NODE_LIST;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getNodes() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return nodes_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> nodes_;
};

class InputListNode final : public ASTNode
{
  public:
    explicit InputListNode(std::vector<std::unique_ptr<ASTNode>> io_tensors) : io_tensors_(std::move(io_tensors))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INPUT_LIST;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getIOTensors() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return io_tensors_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> io_tensors_;
};

class OutputListNode final : public ASTNode
{
  public:
    explicit OutputListNode(std::vector<std::unique_ptr<ASTNode>> io_tensors) : io_tensors_(std::move(io_tensors))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::OUTPUT_LIST;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getIOTensors() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return io_tensors_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> io_tensors_;
};

class InitializerListNode final : public ASTNode
{
  public:
    explicit InitializerListNode(std::vector<std::unique_ptr<ASTNode>> init_tensors)
        : init_tensors_(std::move(init_tensors))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INITIALIZER_LIST;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getInitTensors() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return init_tensors_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> init_tensors_;
};

class NodeNode final : public ASTNode
{
  public:
    NodeNode(std::unique_ptr<ASTNode> op_type, std::unique_ptr<ASTNode> name,
             std::unique_ptr<ASTNode> input_list_or_array, std::unique_ptr<ASTNode> output_list_or_array,
             std::unique_ptr<ASTNode> attribute_list)
        : op_type_(std::move(op_type)), name_(std::move(name)), input_list_or_array_(std::move(input_list_or_array)),
          output_list_or_array_(std::move(output_list_or_array)), attribute_list_(std::move(attribute_list))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::NODE;
    }
    void accept(ASTBaseVisitor &visitor) const override;

    [[nodiscard]] auto getOpType() const -> const ASTNode *
    {
        return op_type_.get();
    }
    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getInputListOrArray() const -> const ASTNode *
    {
        return input_list_or_array_.get();
    }
    [[nodiscard]] auto getOutputListOrArray() const -> const ASTNode *
    {
        return output_list_or_array_.get();
    }
    [[nodiscard]] auto getAttributeList() const -> const ASTNode *
    {
        return attribute_list_.get();
    }

  private:
    std::unique_ptr<ASTNode> op_type_;
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> input_list_or_array_;
    std::unique_ptr<ASTNode> output_list_or_array_;
    std::unique_ptr<ASTNode> attribute_list_;
};

class InputArrNode final : public ASTNode
{
  public:
    explicit InputArrNode(std::vector<std::unique_ptr<ASTNode>> input_elements)
        : input_elements_(std::move(input_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INPUT_ARR;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getInputElements() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return input_elements_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> input_elements_;
};

class OutputArrNode final : public ASTNode
{
  public:
    explicit OutputArrNode(std::vector<std::unique_ptr<ASTNode>> output_elements)
        : output_elements_(std::move(output_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::OUTPUT_ARR;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getOutputElements() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return output_elements_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> output_elements_;
};

class AttributeListNode final : public ASTNode
{
  public:
    explicit AttributeListNode(std::vector<std::unique_ptr<ASTNode>> attributes) : attributes_(std::move(attributes))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::ATTRIBUTE_LIST;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getAttributes() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return attributes_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> attributes_;
};

class AttributeNode final : public ASTNode
{
  public:
    AttributeNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> value)
        : name_(std::move(name)), value_(std::move(value))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::ATTRIBUTE;
    }
    void accept(ASTBaseVisitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getValue() const -> const ASTNode *
    {
        return value_.get();
    }

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> value_;
};

class IOTensorNode final : public ASTNode
{
  public:
    IOTensorNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> io_shape)
        : name_(std::move(name)), type_(std::move(type)), io_shape_(std::move(io_shape))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::IO_TENSOR;
    }
    void accept(ASTBaseVisitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getType() const -> const ASTNode *
    {
        return type_.get();
    }
    [[nodiscard]] auto getIOShape() const -> const ASTNode *
    {
        return io_shape_.get();
    }

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> type_;
    std::unique_ptr<ASTNode> io_shape_;
};

class IOShapeNode final : public ASTNode
{
  public:
    explicit IOShapeNode(std::vector<std::unique_ptr<ASTNode>> io_dims) : io_dims_(std::move(io_dims))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::IO_SHAPE;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getIODims() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return io_dims_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> io_dims_;
};

class IODimNode final : public ASTNode
{
  public:
    explicit IODimNode(std::unique_ptr<ASTNode> value_or_param) : value_or_param_(std::move(value_or_param))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::IO_DIM;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> const ASTNode *
    {
        return value_or_param_.get();
    }

  private:
    std::unique_ptr<ASTNode> value_or_param_;
};

class InitTensorNode final : public ASTNode
{
  public:
    InitTensorNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> type, std::unique_ptr<ASTNode> init_shape,
                   std::unique_ptr<ASTNode> raw_data)
        : name_(std::move(name)), type_(std::move(type)), init_shape_(std::move(init_shape)),
          raw_data_(std::move(raw_data))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INIT_TENSOR;
    }
    void accept(ASTBaseVisitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getType() const -> const ASTNode *
    {
        return type_.get();
    }
    [[nodiscard]] auto getInitShape() const -> const ASTNode *
    {
        return init_shape_.get();
    }
    [[nodiscard]] auto getRawData() const -> const ASTNode *
    {
        return raw_data_.get();
    }

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> type_;
    std::unique_ptr<ASTNode> init_shape_;
    std::unique_ptr<ASTNode> raw_data_;
};

class InitShapeNode final : public ASTNode
{
  public:
    explicit InitShapeNode(std::vector<std::unique_ptr<ASTNode>> dim_values) : dim_values_(std::move(dim_values))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INIT_SHAPE;
    }
    void accept(ASTBaseVisitor &visitor) const override;
    [[nodiscard]] auto getDimValues() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return dim_values_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> dim_values_;
};

class ErrorNode final : public ASTNode
{
  public:
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::ERROR_NODE;
    }
    void accept(ASTBaseVisitor &visitor) const override;
};

} // namespace sonnx

#endif // AST_HPP
