#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>
#include <vector>
#include "ast.hpp"
#include "lexer.hpp"

namespace marked {

    using std::string;

    class Parser
    {
    public:
        Parser();
        ~Parser();
        void parse(const string& input);
    private:
        Lexer *m_lexer;
        
        bool match(int type);
        
        bool parse_block_until(AstNode *block, int until, bool top = false);
        bool parse_span_until(AstNode *span, int until);
        bool merge_until(string& value, int until);
        
        bool parse_body(AstNode *body);
        bool parse_heading_atx(AstNode *heading);
        bool parse_horizontal_rule(AstNode *horizontal_rule);
        bool parse_blockqoute(AstNode *blockqoute);
        bool parse_ordered_list(AstNode *ordered_list);
        bool parse_ordered_listitem(AstNode *listitem);
        bool parse_unordered_list(AstNode *unordered_list);
        bool parse_unordered_listitem(AstNode *listitem);
        bool parse_code_block(AstNode *code_block);
        bool parse_heading_setext(AstNode *heading);
        bool parse_paragraph(AstNode *paragraph);
        bool parse_sentence(AstNode *sentence);
        bool parse_line(AstNode *line);
        bool parse_image(AstNode *image);
        bool parse_link(AstNode *link);
        bool parse_code(AstNode *code);
        bool parse_bold(AstNode *bold);
        bool parse_bold_u(AstNode *bold);
        bool parse_bold_s(AstNode *bold);
        bool parse_emphasis(AstNode *emphasis);
        bool parse_emphasis_u(AstNode *emphasis);
        bool parse_emphasis_s(AstNode *emphasis);
        bool parse_text(AstNode *text);
        
        bool merge_code_block(AstNode *code_block);
        bool merge_code(AstNode *code);
        bool merge_text(AstNode *image_or_link);
        bool merge_link(AstNode *image_or_link);
    };
    
}

#endif
