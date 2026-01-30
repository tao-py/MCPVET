#ifndef INPUT_AST_H
#define INPUT_AST_H

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace mcnp::parser {

enum class CardKind {
    Cell,
    Surface,
    Data,
    Comment,
    Unknown
};

enum class NodeKind {
    Root,
    Card,
    Parameter
};

struct CardInfo {
    CardKind kind = CardKind::Unknown;
    std::size_t line = 0;
    std::string keyword;
    std::string raw;
    std::vector<std::string> parameters;
};

struct AstNode {
    NodeKind kind;
    std::string value;
    std::optional<CardInfo> card;
    std::vector<std::unique_ptr<AstNode>> children;

    explicit AstNode(NodeKind kind, std::string value = {});
    AstNode(const AstNode&) = delete;
    AstNode& operator=(const AstNode&) = delete;
    AstNode(AstNode&&) noexcept = default;
    AstNode& operator=(AstNode&&) noexcept = default;
    ~AstNode() = default;
};

struct Ast {
    AstNode root{NodeKind::Root};
};

} // namespace mcnp::parser

#endif // INPUT_AST_H
