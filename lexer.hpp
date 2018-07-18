#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <vector>
#include <stack>

namespace marked {
    
    using std::stack;
    using std::vector;
    using std::string;
    
    enum TokenType
    {
        TT_NONE =             0,
        TT_DOT =              (1 << 0), // \.
        TT_TEXT =             (1 << 1), // \w+
        TT_STAR =             (1 << 2), // \*
        TT_PLUS =             (1 << 3), // \+
        TT_DASH =             (1 << 4), // -
        TT_EQUAL =            (1 << 5), // =
        TT_QOUTE =            (1 << 6), // `
        TT_POUND =            (1 << 7), // #{1,6}
        TT_ARROW =            (1 << 8), // >+
        TT_NUMBER =           (1 << 9), // \d+
        TT_NEWLINE =          (1 << 10),// \n
        TT_WHITESPACE =       (1 << 11),// \s
        TT_UNDERSCORE =       (1 << 12),// _
        TT_LEFT_PARENT =      (1 << 13),// (
        TT_RIGHT_PARENT =     (1 << 14),// )
        TT_LEFT_SQUARE =      (1 << 15),// [
        TT_RIGHT_SQUARE =     (1 << 16),// ]
        TT_EXCLAMATION =      (1 << 17),// !
        TT_TRIPLE_QOUTE =     (1 << 18),// ```
        TT_MULTI_DASH =       (1 << 19),// -+
        TT_MULTI_EQUAL =      (1 << 20),// =+
        TT_MULTI_WHITESPACE = (1 << 21),// \s+
        TT_EOS =              (1 << 22),
    };

    class Token
    {
    public:
        Token(TokenType type = TT_NONE, string value = "");
        const string& value();
        bool operator==(int type) const;
        bool operator!=(int type) const;
    private:
        TokenType m_type;
        string m_value;
    };

    class Lexer
    {
    public:
        void lex(const string& input);
        bool empty();
        const Token& peek();
        void consume();
        void reset();
        void save();
        void commit();
        void restore();
    private:
        vector<Token> m_tokens;
        int m_cursor;
        stack<int> m_record;
    };
    
}

#endif
