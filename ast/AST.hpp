#ifndef AST_HPP
#define AST_HPP

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
    GRAPH,
    NODE_LIST,
    INPUT_LIST,
    OUTPUT_LIST,
    INITIALIZER_LIST,
    NODE,
    INPUT_ARR,
    OUTPUT_ARR,
    ATTRIBUTE_LIST,
    ATTRIBUTE,
    VALUE_INFO,
    TENSOR_TYPE,
    SHAPE,
    DIM,
    TENSOR,
    OPSET_IMPORT,
    U32_LITERAL,
    U64_LITERAL,
    STR_LITERAL,
    BYTES_LITERAL,
    TYPE_ENUM,
    ERROR
};

enum class ValueOrDataType
{
    INT,
    FLOAT,
    STRING,
    BOOL
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
    virtual void accept(class Visitor &visitor) const = 0;
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
    void accept(Visitor &visitor) const override;
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
    void accept(Visitor &visitor) const override;
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
    void accept(Visitor &visitor) const override;
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
    void accept(Visitor &visitor) const override;
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
    explicit TypeEnumNode(ValueOrDataType type) : value_(type)
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::BYTES_LITERAL;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> ValueOrDataType
    {
        return value_;
    }

  private:
    ValueOrDataType value_;
};

class ModelNode final : public ASTNode
{
  public:
    ModelNode(std::unique_ptr<ASTNode> ir_version, std::unique_ptr<ASTNode> producer_name,
              std::unique_ptr<ASTNode> producer_version, std::unique_ptr<ASTNode> domain,
              std::unique_ptr<ASTNode> model_version, std::unique_ptr<ASTNode> doc_string,
              std::unique_ptr<ASTNode> graph, std::unique_ptr<ASTNode> opset_import)
        : ir_version_(std::move(ir_version)), producer_name_(std::move(producer_name)),
          producer_version_(std::move(producer_version)), domain_(std::move(domain)),
          model_version_(std::move(model_version)), doc_string_(std::move(doc_string)), graph_(std::move(graph)),
          opset_import_(std::move(opset_import))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::MODEL;
    }
    void accept(Visitor &visitor) const override;

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
        return domain_.get();
    }
    [[nodiscard]] auto getModelVersion() const -> const ASTNode *
    {
        return model_version_.get();
    }
    [[nodiscard]] auto getDocString() const -> const ASTNode *
    {
        return doc_string_.get();
    }
    [[nodiscard]] auto getGraph() const -> const ASTNode *
    {
        return graph_.get();
    }
    [[nodiscard]] auto getOpsetImport() const -> const ASTNode *
    {
        return opset_import_.get();
    }

  private:
    std::unique_ptr<ASTNode> ir_version_;
    std::unique_ptr<ASTNode> producer_name_;
    std::unique_ptr<ASTNode> producer_version_;
    std::unique_ptr<ASTNode> domain_;
    std::unique_ptr<ASTNode> model_version_;
    std::unique_ptr<ASTNode> doc_string_;
    std::unique_ptr<ASTNode> graph_;
    std::unique_ptr<ASTNode> opset_import_;
};

class GraphNode final : public ASTNode
{
  public:
    GraphNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> node_list, std::unique_ptr<ASTNode> input_list,
              std::unique_ptr<ASTNode> output_list, std::unique_ptr<ASTNode> initializer_list = nullptr)
        : name_(std::move(name)), node_list_(std::move(node_list)), input_list_(std::move(input_list)),
          output_list_(std::move(output_list)), initializer_list_(std::move(initializer_list))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::GRAPH;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
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

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> node_list_;
    std::unique_ptr<ASTNode> input_list_;
    std::unique_ptr<ASTNode> output_list_;
    std::unique_ptr<ASTNode> initializer_list_;
};

class NodeListNode final : public ASTNode
{
  public:
    NodeListNode(std::vector<std::unique_ptr<ASTNode>> nodes) : nodes_(std::move(nodes))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::NODE_LIST;
    }
    void accept(Visitor &visitor) const override;
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
    InputListNode(std::vector<std::unique_ptr<ASTNode>> inputs) : inputs_(std::move(inputs))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INPUT_LIST;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getInputs() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return inputs_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> inputs_;
};

class OutputListNode final : public ASTNode
{
  public:
    OutputListNode(std::vector<std::unique_ptr<ASTNode>> outputs) : outputs_(std::move(outputs))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::OUTPUT_LIST;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getOutputs() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return outputs_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> outputs_;
};

class InitializerListNode final : public ASTNode
{
  public:
    InitializerListNode(std::vector<std::unique_ptr<ASTNode>> initializers) : initializers_(std::move(initializers))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INITIALIZER_LIST;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getInitializers() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return initializers_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> initializers_;
};

class NodeNode final : public ASTNode
{
  public:
    NodeNode(std::unique_ptr<ASTNode> op_type, std::unique_ptr<ASTNode> name,
             std::unique_ptr<ASTNode> input_list_or_array, std::unique_ptr<ASTNode> output_list_or_array_,
             std::unique_ptr<ASTNode> attribute_list = nullptr)
        : op_type_(std::move(op_type)), name_(std::move(name)), input_list_or_array_(std::move(input_list_or_array)),
          output_list_or_array_(std::move(output_list_or_array_)), attribute_list_(std::move(attribute_list))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::NODE;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getOpType() const -> const ASTNode *
    {
        return op_type_.get();
    }
    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getInputs() const -> const ASTNode *
    {
        return input_list_or_array_.get();
    }
    [[nodiscard]] auto getOutputs() const -> const ASTNode *
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
    InputArrNode(std::vector<std::unique_ptr<ASTNode>> input_elements) : input_elements_(std::move(input_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::INPUT_ARR;
    }
    void accept(Visitor &visitor) const override;
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
    OutputArrNode(std::vector<std::unique_ptr<ASTNode>> output_elements) : output_elements_(std::move(output_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::OUTPUT_ARR;
    }
    void accept(Visitor &visitor) const override;
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
    AttributeListNode(std::vector<std::unique_ptr<ASTNode>> attributes) : attributes_(std::move(attributes))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::ATTRIBUTE_LIST;
    }
    void accept(Visitor &visitor) const override;
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
    void accept(Visitor &visitor) const override;

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

class ValueInfoNode final : public ASTNode
{
  public:
    ValueInfoNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> tensor_type)
        : name_(std::move(name)), tensor_type_(std::move(tensor_type))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::VALUE_INFO;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getTensorType() const -> const ASTNode *
    {
        return tensor_type_.get();
    }

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> tensor_type_;
};

class TensorTypeNode final : public ASTNode
{
  public:
    TensorTypeNode(std::unique_ptr<ASTNode> elem_type, std::unique_ptr<ASTNode> shape)
        : elem_type_(std::move(elem_type)), shape_(std::move(shape))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::TENSOR_TYPE;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getElemType() const -> const ASTNode *
    {
        return elem_type_.get();
    }
    [[nodiscard]] auto getShape() const -> const ASTNode *
    {
        return shape_.get();
    }

  private:
    std::unique_ptr<ASTNode> elem_type_;
    std::unique_ptr<ASTNode> shape_;
};

class ShapeNode final : public ASTNode
{
  public:
    ShapeNode(std::vector<std::unique_ptr<ASTNode>> dim_elements) : dim_elements_(std::move(dim_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::SHAPE;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getDimElements() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return dim_elements_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> dim_elements_;
};

class DimNode final : public ASTNode
{
  public:
    explicit DimNode(std::unique_ptr<ASTNode> value_or_param) : value_or_param_(std::move(value_or_param))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::DIM;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getValue() const -> const ASTNode *
    {
        return value_or_param_.get();
    }

  private:
    std::unique_ptr<ASTNode> value_or_param_;
};

class TensorNode final : public ASTNode
{
  public:
    TensorNode(std::unique_ptr<ASTNode> name, std::unique_ptr<ASTNode> data_type, std::unique_ptr<ASTNode> dims_array,
               std::unique_ptr<ASTNode> raw_data)
        : name_(std::move(name)), data_type_(std::move(data_type)), dims_array_(std::move(dims_array)),
          raw_data_(std::move(raw_data))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::TENSOR;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getName() const -> const ASTNode *
    {
        return name_.get();
    }
    [[nodiscard]] auto getDataType() const -> const ASTNode *
    {
        return data_type_.get();
    }
    [[nodiscard]] auto getDimsArray() const -> const ASTNode *
    {
        return dims_array_.get();
    }
    [[nodiscard]] auto getRawData() const -> const ASTNode *
    {
        return raw_data_.get();
    }

  private:
    std::unique_ptr<ASTNode> name_;
    std::unique_ptr<ASTNode> data_type_;
    std::unique_ptr<ASTNode> dims_array_;
    std::unique_ptr<ASTNode> raw_data_;
};

class DimsArrayNode final : public ASTNode
{
  public:
    DimsArrayNode(std::vector<std::unique_ptr<ASTNode>> dims_elements) : dims_elements_(std::move(dims_elements))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::DIM;
    }
    void accept(Visitor &visitor) const override;
    [[nodiscard]] auto getDimsElements() const -> const std::vector<std::unique_ptr<ASTNode>> &
    {
        return dims_elements_;
    }

  private:
    std::vector<std::unique_ptr<ASTNode>> dims_elements_;
};

class OpsetImportNode final : public ASTNode
{
  public:
    OpsetImportNode(std::unique_ptr<ASTNode> domain, std::unique_ptr<ASTNode> version)
        : domain_(std::move(domain)), version_(std::move(version))
    {
    }
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::OPSET_IMPORT;
    }
    void accept(Visitor &visitor) const override;

    [[nodiscard]] auto getDomain() const -> const ASTNode *
    {
        return domain_.get();
    }
    [[nodiscard]] auto getVersion() const -> const ASTNode *
    {
        return version_.get();
    }

  private:
    std::unique_ptr<ASTNode> domain_;
    std::unique_ptr<ASTNode> version_;
};

class ErrorNode final : public ASTNode
{
  public:
    [[nodiscard]] auto getASTNodeType() const -> NodeType override
    {
        return NodeType::ERROR;
    }
    void accept(Visitor &visitor) const override;
};

} // namespace sonnx

#endif // AST_HPP
