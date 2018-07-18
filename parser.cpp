#include "parser.hpp"

namespace marked {
    
    Parser::Parser()
    {
        m_lexer = new Lexer;
    }
    
    Parser::~Parser()
    {
        delete m_lexer;
    }
    
    // TokenType是按bit对比的
    // 方便处理多个参数的情况
    bool Parser::match(int type)
    {
        
        if (m_lexer->peek() == type)
        {
            m_lexer->consume();
            return true;
        }
        
        return false;
    }
    
    bool Parser::parse_block_until(AstNode *block, int until, bool top)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != until)
        {
            if (m_lexer->peek() == TT_NEWLINE)
            {
                m_lexer->consume();
                continue;
            }
            
            AstNode *node = new AstNode();
            
            if (parse_horizontal_rule(node))
            {
                node->set_type(AT_HORIZONTAL_RULE);
                block->append(node);
                match = true;
            }
            else if (top && parse_heading_atx(node))
            {
                node->set_type(AT_HEADING);
                block->append(node);
                match = true;
            }
            else if (top && parse_blockqoute(node))
            {
                node->set_type(AT_BLOCKQOUTE);
                block->append(node);
                match = true;
            }
            else if (parse_ordered_list(node))
            {
                node->set_type(AT_ORDERED_LIST);
                block->append(node);
                match = true;
            }
            else if (parse_unordered_list(node))
            {
                node->set_type(AT_UNORDERED_LIST);
                block->append(node);
                match = true;
            }
            else if (top && parse_code_block(node))
            {
                node->set_type(AT_CODE_BLOCK);
                block->append(node);
                match = true;
            }
            else if (top && parse_heading_setext(node))
            {
                node->set_type(AT_HEADING);
                block->append(node);
                match = true;
            }
            else if (top && parse_paragraph(node))
            {
                node->set_type(AT_PARAGRAPH);
                block->append(node);
                match = true;
            }
            else if (parse_line(node))
            {
                node->set_type(AT_LINE);
                block->append(node);
                match = true;
            }
            else
            {
                delete node;
                break;
            }
        }
        
        return match;
    }
    
    bool Parser::parse_span_until(AstNode *span, int until)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != until)
        {
            AstNode *node = new AstNode();
            
            if (parse_image(node))
            {
                node->set_type(AT_IMAGE);
                span->append(node);
                match = true;
            }
            else if (parse_link(node))
            {
                node->set_type(AT_LINK);
                span->append(node);
                match = true;
            }
            else if (parse_code(node))
            {
                node->set_type(AT_CODE);
                span->append(node);
                match = true;
            }
            else if (parse_bold(node))
            {
                node->set_type(AT_BOLD);
                span->append(node);
                match = true;
            }
            else if (parse_emphasis(node))
            {
                node->set_type(AT_EMPHASIS);
                span->append(node);
                match = true;
            }
            else if (parse_text(node))
            {
                node->set_type(AT_TEXT);
                span->append(node);
                match = true;
            }
            else
            {
                delete node;
                break;
            }
        }
        
        return match;
    }
    
    
    bool Parser::merge_until(string& value, int until)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != until)
        {
            Token token = m_lexer->peek();
            m_lexer->consume();
            
            value += token.value();
            match = true;
        }
        
        return match;
    }

    void Parser::parse(const string& input)
    {
        using std::cout;
        using std::endl;
        
        m_lexer->lex(input);
        
        Ast ast;
        AstNode *node = new AstNode(AT_BODY);
        
        if (parse_body(node))
        {
            ast.set_root(node);
            ast.print();
        }
        else
        {
            delete node;
            cout << "parser fail" << endl;
        }
    }

    bool Parser::parse_body(AstNode *body)
    {
        m_lexer->save();
        
        if (parse_block_until(body, TT_EOS, true) &&
            match(TT_EOS))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_horizontal_rule(AstNode *horizontal_rule)
    {
        m_lexer->save();
        
        if (match(TT_MULTI_DASH | TT_MULTI_EQUAL) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_heading_atx(AstNode *heading)
    {
        m_lexer->save();
        
        if (match(TT_POUND) &&
            match(TT_WHITESPACE) &&
            parse_span_until(heading, TT_NEWLINE) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }

        m_lexer->restore();
        return false;
    }

    bool Parser::parse_blockqoute(AstNode *blockqoute)
    {
        m_lexer->save();
        
        if (match(TT_ARROW) &&
            match(TT_WHITESPACE) &&
            parse_block_until(blockqoute, TT_ARROW | TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }

        m_lexer->restore();
        return false;
    }

    bool Parser::parse_ordered_list(AstNode *ordered_list)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != TT_EOS)
        {
            AstNode *node = new AstNode(AT_LISTITEM);
            
            if (parse_ordered_listitem(node))
            {
                ordered_list->append(node);
                match = true;
            }
            else
            {
                delete node;
                break;
            }
        }
        
        return match;
    }

    bool Parser::parse_ordered_listitem(AstNode *listitem)
    {
        m_lexer->save();
        
        // ignore，之后需要补充计算
        match(TT_WHITESPACE | TT_MULTI_WHITESPACE);
        
        if (match(TT_NUMBER) &&
            match(TT_DOT) &&
            match(TT_WHITESPACE) &&
            parse_block_until(listitem, TT_NUMBER | TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }

        m_lexer->restore();
        return false;
    }

    bool Parser::parse_unordered_list(AstNode *unordered_list)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != TT_EOS)
        {
            AstNode *node = new AstNode(AT_LISTITEM);
            
            if (parse_unordered_listitem(node))
            {
                unordered_list->append(node);
                match = true;
            }
            else
            {
                delete node;
                break;
            }
        }
        
        return match;
    }

    bool Parser::parse_unordered_listitem(AstNode *listitem)
    {
        m_lexer->save();
        
        // ignore，之后需要补充计算
        match(TT_WHITESPACE | TT_MULTI_WHITESPACE);
        
        if (match(TT_PLUS | TT_STAR | TT_DASH) &&
            match(TT_WHITESPACE) &&
            parse_block_until(listitem, TT_PLUS | TT_STAR | TT_DASH | TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }

        m_lexer->restore();
        return false;
    }

    bool Parser::parse_code_block(AstNode *code_block)
    {
        m_lexer->save();
        
        if (match(TT_TRIPLE_QOUTE) &&
            merge_code_block(code_block) &&
            match(TT_TRIPLE_QOUTE) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }

        m_lexer->restore();
        return false;
    }
    
    bool Parser::parse_heading_setext(AstNode *heading)
    {
        m_lexer->save();
        
        if (parse_span_until(heading, TT_NEWLINE) &&
            match(TT_NEWLINE) &&
            match(TT_MULTI_EQUAL | TT_MULTI_DASH) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_paragraph(AstNode *paragraph)
    {
        AstNode *node = new AstNode(AT_SENTENCE);
        m_lexer->save();
        
        if (parse_sentence(node) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            paragraph->append(node);
            
            return true;
        }

        m_lexer->restore();
        delete node;
        
        return false;
    }
    
    bool Parser::parse_sentence(AstNode *sentence)
    {
        bool match = false;
        
        while (!m_lexer->empty() && m_lexer->peek() != TT_NEWLINE)
        {
            AstNode *node = new AstNode(AT_LINE);
            
            if (parse_line(node))
            {
                sentence->append(node);
                
                AstNode* text = new AstNode(AT_TEXT, "\n");
                sentence->append(text);
                
                match = true;
            }
            else
            {
                delete node;
                break;
            }
        }
        
        if (match)
        {
            sentence->remove();
        }
        
        return match;
    }
    
    bool Parser::parse_line(AstNode *line)
    {
        m_lexer->save();
        
        if (parse_span_until(line, TT_NEWLINE) &&
            match(TT_NEWLINE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_image(AstNode *image)
    {
        m_lexer->save();
        
        if (match(TT_EXCLAMATION) &&
            match(TT_LEFT_SQUARE) &&
            merge_text(image) &&
            match(TT_RIGHT_SQUARE) &&
            match(TT_LEFT_PARENT) &&
            merge_link(image) &&
            match(TT_RIGHT_PARENT))
        {
            m_lexer->commit();
            return true;
        }
        
        image->set_link("");
        image->set_text("");
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_link(AstNode *link)
    {
        m_lexer->save();
        
        if (match(TT_LEFT_SQUARE) &&
            merge_text(link) &&
            match(TT_RIGHT_SQUARE) &&
            match(TT_LEFT_PARENT) &&
            merge_link(link) &&
            match(TT_RIGHT_PARENT))
        {
            m_lexer->commit();
            return true;
        }
        
        link->set_link("");
        link->set_text("");
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_code(AstNode *code)
    {
        m_lexer->save();
        
        if (match(TT_QOUTE) &&
            merge_code(code) &&
            match(TT_QOUTE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }
    
    bool Parser::parse_bold(AstNode *bold)
    {
        if (parse_bold_u(bold) ||
            parse_bold_s(bold))
            return true;
        else
            return false;
    }
    
    bool Parser::parse_bold_u(AstNode *bold)
    {
        m_lexer->save();
        
        if (match(TT_UNDERSCORE) &&
            match(TT_UNDERSCORE) &&
            parse_span_until(bold, TT_UNDERSCORE) &&
            match(TT_UNDERSCORE) &&
            match(TT_UNDERSCORE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_bold_s(AstNode *bold)
    {
        m_lexer->save();
        
        if (match(TT_STAR) &&
            match(TT_STAR) &&
            parse_span_until(bold, TT_STAR) &&
            match(TT_STAR) &&
            match(TT_STAR))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }
    
    bool Parser::parse_emphasis(AstNode *bold)
    {
        if (parse_emphasis_u(bold) ||
            parse_emphasis_s(bold))
            return true;
        else
            return false;
    }
    
    bool Parser::parse_emphasis_u(AstNode *emphasis)
    {
        m_lexer->save();
        
        if (match(TT_UNDERSCORE) &&
            parse_span_until(emphasis, TT_UNDERSCORE) &&
            match(TT_UNDERSCORE))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }
    
    bool Parser::parse_emphasis_s(AstNode *emphasis)
    {
        m_lexer->save();
        
        if (match(TT_STAR) &&
            parse_span_until(emphasis, TT_STAR) &&
            match(TT_STAR))
        {
            m_lexer->commit();
            return true;
        }
        
        m_lexer->restore();
        return false;
    }

    bool Parser::parse_text(AstNode *text)
    {
        if (m_lexer->peek() != TT_NEWLINE)
        {
            Token token = m_lexer->peek();
            m_lexer->consume();
            
            text->set_text(token.value());
            
            return true;
        }
        
        return false;
    }

    bool Parser::merge_code_block(AstNode *code_block)
    {
        string value = "";
        
        if (merge_until(value, TT_TRIPLE_QOUTE))
        {
            code_block->set_text(value);
            return true;
        }
        
        return false;
    }
    
    bool Parser::merge_code(AstNode *code)
    {
        string value = "";
        
        if (merge_until(value, TT_QOUTE))
        {
            code->set_text(value);
            return true;
        }
        
        return false;
    }
    
    bool Parser::merge_text(AstNode *image_or_link)
    {
        string value = "";
        
        if (merge_until(value, TT_RIGHT_SQUARE))
        {
            image_or_link->set_text(value);
            return true;
        }
        
        return false;
    }
    
    bool Parser::merge_link(AstNode *image_or_link)
    {
        string value = "";
        
        if (merge_until(value, TT_RIGHT_PARENT))
        {
            image_or_link->set_link(value);
            return true;
        }
        
        return false;
    }
}
