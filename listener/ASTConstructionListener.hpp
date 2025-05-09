#ifndef AST_CONSTRUCTION_LISTENER_HPP
#define AST_CONSTRUCTION_LISTENER_HPP

#include "S_ONNXBaseListener.h"
#include "ast/AST.hpp"
#include <memory>

namespace sonnx
{

class ASTConstructionListener final : public antlr_sonnx::S_ONNXBaseListener
{
  private:
    std::stack<std::unique_ptr<ASTNode>> stack_;
    bool has_attribute_list_ = false;

    template <typename CtxType> void processU32U64(CtxType ctx);
    template <typename CtxType> void processString(CtxType ctx);
    template <typename CtxType> void processTypeEnum(CtxType ctx);

  public:
    auto getTop() -> std::unique_ptr<ASTNode>
    {
        auto result = std::move(stack_.top());
        stack_.pop();
        return result;
    }

    void visitErrorNode(antlr4::tree::ErrorNode *node) override;
    void exitModel(antlr_sonnx::S_ONNXParser::ModelContext *ctx) override;
    void exitIr_version_def(antlr_sonnx::S_ONNXParser::Ir_version_defContext *ctx) override;
    void exitProducer_name_def(antlr_sonnx::S_ONNXParser::Producer_name_defContext *ctx) override;
    void exitProducer_version_def(antlr_sonnx::S_ONNXParser::Producer_version_defContext *ctx) override;
    void exitDomain_def(antlr_sonnx::S_ONNXParser::Domain_defContext *ctx) override;
    void exitModel_version_def(antlr_sonnx::S_ONNXParser::Model_version_defContext *ctx) override;
    void exitDoc_string_def(antlr_sonnx::S_ONNXParser::Doc_string_defContext *ctx) override;
    void exitName_def(antlr_sonnx::S_ONNXParser::Name_defContext *ctx) override;
    void exitNode_list(antlr_sonnx::S_ONNXParser::Node_listContext *ctx) override;
    void exitInput_list(antlr_sonnx::S_ONNXParser::Input_listContext *ctx) override;
    void exitOutput_list(antlr_sonnx::S_ONNXParser::Output_listContext *ctx) override;
    void exitInitializer_list(antlr_sonnx::S_ONNXParser::Initializer_listContext *ctx) override;
    void exitNode_def(antlr_sonnx::S_ONNXParser::Node_defContext *ctx) override;
    void exitOp_type_def(antlr_sonnx::S_ONNXParser::Op_type_defContext *ctx) override;
    void exitInput_arr(antlr_sonnx::S_ONNXParser::Input_arrContext *ctx) override;
    void exitOutput_arr(antlr_sonnx::S_ONNXParser::Output_arrContext *ctx) override;
    void exitAttribute_list(antlr_sonnx::S_ONNXParser::Attribute_listContext *ctx) override;
    void exitAttribute_def(antlr_sonnx::S_ONNXParser::Attribute_defContext *ctx) override;
    void exitValue_def(antlr_sonnx::S_ONNXParser::Value_defContext *ctx) override;
    void exitValue_info_def(antlr_sonnx::S_ONNXParser::Value_info_defContext *ctx) override;
    void exitElem_type_def(antlr_sonnx::S_ONNXParser::Elem_type_defContext *ctx) override;
    void exitDim_list(antlr_sonnx::S_ONNXParser::Dim_listContext *ctx) override;
    void exitDim_def(antlr_sonnx::S_ONNXParser::Dim_defContext *ctx) override;
    void exitTensor_def(antlr_sonnx::S_ONNXParser::Tensor_defContext *ctx) override;
    void exitData_type_def(antlr_sonnx::S_ONNXParser::Data_type_defContext *ctx) override;
    void exitDims_def(antlr_sonnx::S_ONNXParser::Dims_defContext *ctx) override;
    void exitRaw_data_def(antlr_sonnx::S_ONNXParser::Raw_data_defContext *ctx) override;
    void exitVersion_def(antlr_sonnx::S_ONNXParser::Version_defContext *ctx) override;
};

} // namespace sonnx

#endif // AST_CONSTRUCTION_LISTENER_HPP
