#ifndef PARSER_ERROR_STRATEGY_HPP
#define PARSER_ERROR_STRATEGY_HPP

#include "antlr4-runtime.h"

namespace sonnx {
class ParserErrorStrategy final : public antlr4::DefaultErrorStrategy
{
};
}

#endif // PARSER_ERROR_STRATEGY_HPP
