#ifndef MCNP_PARSER_H
#define MCNP_PARSER_H

#include "input_ast.h"
#include <string_view>
#include <vector>

namespace mcnp::parser {

struct ParseError {
    std::size_t line = 0;
    std::string message;
};

struct ParseResult {
    Ast ast;
    std::vector<ParseError> errors;
};

class InputParser {
public:
    virtual ~InputParser() = default;
    virtual ParseResult parse(std::string_view text) const = 0;
};

class MCNPParser final : public InputParser {
public:
    ParseResult parse(std::string_view text) const override;
};

} // namespace mcnp::parser

#endif // MCNP_PARSER_H
