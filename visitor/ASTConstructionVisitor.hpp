#ifndef AST_CONSTRUCTION_VISITOR_HPP
#define AST_CONSTRUCTION_VISITOR_HPP

#include "S_ONNXBaseVisitor.h"
#include "S_ONNXParser.h"
#include "ast/AST.hpp"
#include <memory>
#include <stack>

// #define DEBUG_AST_CONSTRUCTION

namespace sonnx
{

class ASTConstructionVisitor final : public antlr_sonnx::S_ONNXBaseVisitor
{
  private:
    std::stack<std::unique_ptr<ASTNode>> stack_;
    bool has_initializer_list_ = false;
    bool has_attribute_list_ = false;

    template <typename CtxType> void processU32U64(CtxType ctx);
    template <typename CtxType> void processString(CtxType ctx);
    template <typename CtxType> void processTypeEnum(CtxType ctx);

    // Error reporting helper method
    static void reportError(const antlr4::ParserRuleContext *ctx, const std::string &message);
#ifdef DEBUG_AST_CONSTRUCTION
    void printStack() const;
#endif

  public:
    auto getTop() -> std::unique_ptr<ASTNode>
    {
        if (stack_.empty())
        {
            throw std::runtime_error("AST construction failed: empty stack");
        }
        auto result = std::move(stack_.top());
        stack_.pop();
        return result;
    }

    std::any visitModel(antlr_sonnx::S_ONNXParser::ModelContext *ctx) override;
    std::any visitModel_body_def(antlr_sonnx::S_ONNXParser::Model_body_defContext *ctx) override;
    std::any visitIr_version_def(antlr_sonnx::S_ONNXParser::Ir_version_defContext *ctx) override;
    std::any visitProducer_name_def(antlr_sonnx::S_ONNXParser::Producer_name_defContext *ctx) override;
    std::any visitProducer_version_def(antlr_sonnx::S_ONNXParser::Producer_version_defContext *ctx) override;
    std::any visitDomain_def(antlr_sonnx::S_ONNXParser::Domain_defContext *ctx) override;
    std::any visitModel_version_def(antlr_sonnx::S_ONNXParser::Model_version_defContext *ctx) override;
    std::any visitDoc_string_def(antlr_sonnx::S_ONNXParser::Doc_string_defContext *ctx) override;
    std::any visitGraph_def(antlr_sonnx::S_ONNXParser::Graph_defContext *ctx) override;
    std::any visitGraph_body_def(antlr_sonnx::S_ONNXParser::Graph_body_defContext *ctx) override;
    std::any visitName_def(antlr_sonnx::S_ONNXParser::Name_defContext *ctx) override;
    std::any visitNode_list(antlr_sonnx::S_ONNXParser::Node_listContext *ctx) override;
    std::any visitInput_list(antlr_sonnx::S_ONNXParser::Input_listContext *ctx) override;
    std::any visitOutput_list(antlr_sonnx::S_ONNXParser::Output_listContext *ctx) override;
    std::any visitInitializer_list(antlr_sonnx::S_ONNXParser::Initializer_listContext *ctx) override;
    std::any visitNode_def(antlr_sonnx::S_ONNXParser::Node_defContext *ctx) override;
    std::any visitOp_type_def(antlr_sonnx::S_ONNXParser::Op_type_defContext *ctx) override;
    std::any visitInput_arr(antlr_sonnx::S_ONNXParser::Input_arrContext *ctx) override;
    std::any visitOutput_arr(antlr_sonnx::S_ONNXParser::Output_arrContext *ctx) override;
    std::any visitAttribute_list(antlr_sonnx::S_ONNXParser::Attribute_listContext *ctx) override;
    std::any visitAttribute_def(antlr_sonnx::S_ONNXParser::Attribute_defContext *ctx) override;
    std::any visitValue_def(antlr_sonnx::S_ONNXParser::Value_defContext *ctx) override;
    std::any visitValue_info_def(antlr_sonnx::S_ONNXParser::Value_info_defContext *ctx) override;
    std::any visitType_def(antlr_sonnx::S_ONNXParser::Type_defContext *ctx) override;
    std::any visitTensor_type_def(antlr_sonnx::S_ONNXParser::Tensor_type_defContext *ctx) override;
    std::any visitElem_type_def(antlr_sonnx::S_ONNXParser::Elem_type_defContext *ctx) override;
    std::any visitShape_def(antlr_sonnx::S_ONNXParser::Shape_defContext *ctx) override;
    std::any visitDim_list(antlr_sonnx::S_ONNXParser::Dim_listContext *ctx) override;
    std::any visitDim_def(antlr_sonnx::S_ONNXParser::Dim_defContext *ctx) override;
    std::any visitTensor_def(antlr_sonnx::S_ONNXParser::Tensor_defContext *ctx) override;
    std::any visitData_type_def(antlr_sonnx::S_ONNXParser::Data_type_defContext *ctx) override;
    std::any visitDims_def(antlr_sonnx::S_ONNXParser::Dims_defContext *ctx) override;
    std::any visitRaw_data_def(antlr_sonnx::S_ONNXParser::Raw_data_defContext *ctx) override;
    std::any visitOpset_import_def(antlr_sonnx::S_ONNXParser::Opset_import_defContext *ctx) override;
    std::any visitVersion_def(antlr_sonnx::S_ONNXParser::Version_defContext *ctx) override;
};

} // namespace sonnx

#endif // AST_CONSTRUCTION_VISITOR_HPP
