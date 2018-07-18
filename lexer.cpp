#include "lexer.hpp"

namespace marked {

    Token::Token(TokenType type, string value):
    m_type(type),
    m_value(value)
    {
    }

    const string& Token::value()
    {
        return m_value;
    }

    bool Token::operator==(int type) const
    {
        return (m_type & type) == m_type;
    }

    bool Token::operator!=(int type) const
    {
        return (m_type & type) == TT_NONE;
    }

    void Lexer::lex(const string& input)
    {
        // 重置lexer状态
        reset();
        
        bool is_cap = false;
        int i = 0; // 当前处理的字符下标
        int s = 0; // 待处理的TEXT类型token的启示位置
        
        while (i < input.length())
        {
            Token token;
            int j = i;
            
            if (input[i] == '!')
            {
                token = Token(TT_EXCLAMATION, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '(')
            {
                token = Token(TT_LEFT_PARENT, input.substr(i, 1));
                i++;
            }
            else if (input[i] == ')')
            {
                token = Token(TT_RIGHT_PARENT, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '[')
            {
                token = Token(TT_LEFT_SQUARE, input.substr(i, 1));
                i++;
            }
            else if (input[i] == ']')
            {
                token = Token(TT_RIGHT_SQUARE, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '.')
            {
                token = Token(TT_DOT, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '*')
            {
                token = Token(TT_STAR, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '+')
            {
                token = Token(TT_PLUS, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '_')
            {
                token = Token(TT_UNDERSCORE, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '\n')
            {
                token = Token(TT_NEWLINE, input.substr(i, 1));
                i++;
            }
            else if (input[i] == '`')
            {
                bool m = input[i + 1] == '`' && input[i + 2] == '`';
                token = Token(m ? TT_TRIPLE_QOUTE : TT_QOUTE, input.substr(i, m ? 3 : 1));
                i += m ? 3 : 1;
            }
            else if (input[i] == '#')
            {
                int n = 1;
                while (n < 6 && input[i + n] == '#') n++;
                token = Token(TT_POUND, input.substr(i, n));
                i += n;
            }
            else if (input[i] == '>')
            {
                int n = 1;
                while (input[i + n] == '>') n++;
                token = Token(TT_ARROW, input.substr(i, n));
                i += n;
            }
            else if (input[i] == '-')
            {
                int n = 1;
                while (input[i + n] == '-') n++;
                token = Token(n > 1 ? TT_MULTI_DASH : TT_DASH, input.substr(i, n));
                i += n;
            }
            else if (input[i] == '=')
            {
                int n = 1;
                while (input[i + n] == '=') n++;
                token = Token(n > 1 ? TT_MULTI_EQUAL : TT_EQUAL, input.substr(i, n));
                i += n;
            }
            else if (isspace(input[i]))
            {
                int n = 1;
                while (isspace(input[i + n])) n++;
                token = Token(n > 1 ? TT_MULTI_WHITESPACE : TT_WHITESPACE, input.substr(i, n));
                i += n;
            }
            else if (isdigit(input[i]))
            {
                int n = 1;
                while (isdigit(input[i + n])) n++;
                token = Token(TT_NUMBER, input.substr(i, n));
                i += n;
            }
            
            if (i != j)
            {
                if (is_cap)
                {
                    m_tokens.push_back(Token(TT_TEXT, input.substr(s, j - s)));
                    is_cap = false;
                }
                
                m_tokens.push_back(token);
                continue;
            }
            
            if (!is_cap)
            {
                is_cap = true;
                s = i;
            }
            
            i++;
        }
        
        if (is_cap)
        {
            m_tokens.push_back(Token(TT_TEXT, input.substr(s, i - s)));
        }
        
        // 追加一个换行符，会使得后续的parse过程更简单
        m_tokens.push_back(Token(TT_NEWLINE, "\n"));
        m_tokens.push_back(Token(TT_EOS));
    }

    bool Lexer::empty()
    {
        return m_cursor >= m_tokens.size();
    }

    const Token& Lexer::peek()
    {
        return m_tokens[m_cursor];
    }

    void Lexer::consume()
    {
        m_cursor += 1;
    }

    void Lexer::reset()
    {
        while (!m_record.empty())
            m_record.pop();
        
        m_cursor = 0;
    }

    void Lexer::save()
    {
        m_record.push(m_cursor);
    }

    void Lexer::commit()
    {
        m_record.pop();
    }

    void Lexer::restore()
    {
        m_cursor = m_record.top();
        m_record.pop();
    }
    
}
