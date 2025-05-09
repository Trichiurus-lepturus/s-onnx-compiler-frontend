#ifndef AST_SEMANTIC_VISITOR_HPP
#define AST_SEMANTIC_VISITOR_HPP
#include "ASTBaseVisitor.hpp"
#include <ast/AST.hpp>
#include <string>
#include <unordered_map>

namespace sonnx
{

struct SymbolInfo
{
    std::string name_;
    NodeType type_;
    const ASTNode *definition_;
    SymbolInfo() = delete;
    SymbolInfo(std::string name, const NodeType type, const ASTNode *definition) noexcept
        : name_(std::move(name)), type_(type), definition_(definition)
    {
    }
    SymbolInfo(const SymbolInfo &) = delete;
    SymbolInfo &operator=(const SymbolInfo &) = delete;
    SymbolInfo(SymbolInfo &&symbol_info) noexcept
        : name_(std::move(symbol_info.name_)), type_(symbol_info.type_), definition_(symbol_info.definition_)
    {
        symbol_info.definition_ = nullptr;
    }
    SymbolInfo &operator=(SymbolInfo &&symbol_info) noexcept
    {
        this->name_ = std::move(symbol_info.name_);
        this->type_ = symbol_info.type_;
        this->definition_ = symbol_info.definition_;
        symbol_info.definition_ = nullptr;
        return *this;
    }
    ~SymbolInfo() = default;
};

class ASTSemanticTag final : public ASTTag
{
  private:
    SymbolInfo symbol_info_;

  public:
    ASTSemanticTag(std::string name, const NodeType type, const ASTNode *definition) noexcept
        : symbol_info_(std::move(name), type, definition)
    {
    }
    auto getSymbolInfo() -> const SymbolInfo &
    {
        return symbol_info_;
    }
};

class ASTSemanticVisitor final : public Visitor
{
  public:
    void visit(const U32LiteralNode &node) override;
    void visit(const U64LiteralNode &node) override;
    void visit(const StrLiteralNode &node) override;
    void visit(const BytesLiteralNode &node) override;
    void visit(const TypeEnumNode &node) override;
    void visit(const ModelNode &node) override;
    void visit(const NodeListNode &node) override;
    void visit(const InputListNode &node) override;
    void visit(const OutputListNode &node) override;
    void visit(const InitializerListNode &node) override;
    void visit(const NodeNode &node) override;
    void visit(const InputArrNode &node) override;
    void visit(const OutputArrNode &node) override;
    void visit(const AttributeListNode &node) override;
    void visit(const AttributeNode &node) override;
    void visit(const IOTensorNode &node) override;
    void visit(const IOShapeNode &node) override;
    void visit(const IODimNode &node) override;
    void visit(const InitTensorNode &node) override;
    void visit(const InitShapeNode &node) override;
    void visit(const ErrorNode &node) override;

  private:
    std::unordered_map<std::string, SymbolInfo> symbol_table_;
};

} // namespace sonnx

#endif // AST_SEMANTIC_VISITOR_HPP
