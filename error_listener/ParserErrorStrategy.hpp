#ifndef PARSER_ERROR_STRATEGY_HPP
#define PARSER_ERROR_STRATEGY_HPP

#include "antlr4-runtime.h"
#include <sstream>

namespace sonnx
{

class ParserErrorStrategy final : public antlr4::BailErrorStrategy
{
  public:
    void recover(antlr4::Parser *recognizer, std::exception_ptr e) override
    {
        // Get current token position
        auto currentToken = recognizer->getCurrentToken();
        std::ostringstream errorMsg;

        errorMsg << "FATAL Parse error at line " << currentToken->getLine() << ", column "
                 << (currentToken->getCharPositionInLine() + 1);

        try
        {
            std::rethrow_exception(e);
        }
        catch (const antlr4::RecognitionException &re)
        {
            errorMsg << ": " << re.what();
            if (re.getOffendingToken() != nullptr)
            {
                errorMsg << " (near token: '" << re.getOffendingToken()->getText() << "')";
            }
        }
        catch (const std::exception &ex)
        {
            errorMsg << ": " << ex.what();
        }

        // Immediately throw - no recovery possible
        throw antlr4::ParseCancellationException(errorMsg.str());
    }

    antlr4::Token *recoverInline(antlr4::Parser *recognizer) override
    {
        auto currentToken = recognizer->getCurrentToken();
        std::ostringstream errorMsg;

        errorMsg << "FATAL Missing token at line " << currentToken->getLine() << ", column "
                 << (currentToken->getCharPositionInLine() + 1) << " (current token: '" << currentToken->getText()
                 << "')";

        // Immediately throw - no recovery possible
        throw antlr4::ParseCancellationException(errorMsg.str());
    }
};

} // namespace sonnx

#endif // PARSER_ERROR_STRATEGY_HPP
