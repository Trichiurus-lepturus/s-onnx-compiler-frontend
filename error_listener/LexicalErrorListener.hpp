#ifndef LEXICAL_ERROR_LISTENER_HPP
#define LEXICAL_ERROR_LISTENER_HPP

#include "antlr4-runtime.h"
#include <sstream>

namespace sonnx
{

class LexicalErrorListener final : public antlr4::BaseErrorListener
{
  public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                     size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override
    {
        std::ostringstream errorMsg;
        errorMsg << "FATAL Lexical error at line " << line << ", column " << (charPositionInLine + 1) << ": " << msg;
        if (offendingSymbol != nullptr)
        {
            errorMsg << " (near token: '" << offendingSymbol->getText() << "')";
        }

        // Immediately throw - no recovery possible
        throw antlr4::ParseCancellationException(errorMsg.str());
    }
};

} // namespace sonnx

#endif // LEXICAL_ERROR_LISTENER_HPP
