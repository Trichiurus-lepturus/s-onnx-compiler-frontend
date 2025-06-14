#ifndef PARSER_ERROR_LISTENER_HPP
#define PARSER_ERROR_LISTENER_HPP

#include "antlr4-runtime.h"
#include <sstream>

namespace sonnx
{

class ParserErrorListener final : public antlr4::BaseErrorListener
{
  public:
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                     size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override
    {
        std::ostringstream errorMsg;
        errorMsg << "FATAL Parser error at line " << line << ", column " << (charPositionInLine + 1) << ": " << msg;

        if (offendingSymbol != nullptr)
        {
            errorMsg << " (unexpected token: '" << offendingSymbol->getText() << "' of type "
                     << getTokenTypeName(recognizer, offendingSymbol) << ")";
        }

        // Immediately throw - no recovery possible
        throw antlr4::ParseCancellationException(errorMsg.str());
    }

  private:
    std::string getTokenTypeName(antlr4::Recognizer *recognizer, antlr4::Token *token)
    {
        if (recognizer != nullptr && token != nullptr)
        {
            auto vocabulary = recognizer->getVocabulary();
            return vocabulary.getDisplayName(token->getType());
        }
        return "UNKNOWN";
    }
};

} // namespace sonnx

#endif // PARSER_ERROR_LISTENER_HPP
