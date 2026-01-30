#include "input_ast.h"

namespace mcnp::parser {

AstNode::AstNode(NodeKind kind, std::string value)
    : kind(kind), value(std::move(value)) {}

} // namespace mcnp::parser
