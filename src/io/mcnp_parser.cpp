#include "mcnp_parser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace mcnp::parser {

namespace {

enum class Section {
    Cell,
    Surface,
    Data
};

std::string trim(std::string value) {
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

bool is_comment_line(const std::string& line) {
    if (line.empty()) {
        return false;
    }
    const char first = line.front();
    return first == 'c' || first == 'C' || first == '$';
}

CardKind kind_for_section(Section section) {
    switch (section) {
        case Section::Cell:
            return CardKind::Cell;
        case Section::Surface:
            return CardKind::Surface;
        case Section::Data:
            return CardKind::Data;
        default:
            return CardKind::Unknown;
    }
}

std::vector<std::string> split_tokens(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

AstNode* append_card(Ast& ast, CardInfo info) {
    auto node = std::make_unique<AstNode>(NodeKind::Card, info.keyword);
    node->card = std::move(info);
    AstNode* node_ptr = node.get();
    ast.root.children.push_back(std::move(node));
    return node_ptr;
}

} // namespace

ParseResult MCNPParser::parse(std::string_view text) const {
    ParseResult result;
    Section section = Section::Cell;
    std::istringstream stream{std::string(text)};
    std::string line;
    std::size_t line_number = 0;
    bool saw_content = false;

    while (std::getline(stream, line)) {
        ++line_number;
        const std::string trimmed = trim(line);
        if (trimmed.empty()) {
            if (saw_content) {
                if (section == Section::Cell) {
                    section = Section::Surface;
                } else if (section == Section::Surface) {
                    section = Section::Data;
                }
                saw_content = false;
            }
            continue;
        }

        saw_content = true;
        if (is_comment_line(trimmed)) {
            CardInfo info;
            info.kind = CardKind::Comment;
            info.line = line_number;
            info.raw = line;
            info.keyword = "comment";
            append_card(result.ast, std::move(info));
            continue;
        }

        auto tokens = split_tokens(trimmed);
        if (tokens.empty()) {
            continue;
        }

        CardInfo info;
        info.kind = kind_for_section(section);
        info.line = line_number;
        info.keyword = tokens.front();
        info.raw = line;
        info.parameters.assign(tokens.begin() + 1, tokens.end());

        AstNode* card_node = append_card(result.ast, info);
        for (const auto& param : info.parameters) {
            card_node->children.push_back(std::make_unique<AstNode>(NodeKind::Parameter, param));
        }
    }

    return result;
}

} // namespace mcnp::parser
