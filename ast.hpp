#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <vector>

namespace marked {
    
    using std::vector;
    using std::string;

    enum AstNodeType
    {
        AT_NONE,
        AT_BLOCK,
        AT_SPAN,
        AT_BODY,
        AT_HORIZONTAL_RULE,
        AT_BLOCKQOUTE,
        AT_ORDERED_LIST,
        AT_UNORDERED_LIST,
        AT_LISTITEM,
        AT_CODE_BLOCK,
        AT_HEADING,
        AT_PARAGRAPH,
        AT_SENTENCE,
        AT_LINE,
        AT_IMAGE,
        AT_LINK,
        AT_CODE,
        AT_EMPHASIS,
        AT_BOLD,
        AT_TEXT,
    };

    class AstNode
    {
    public:
        AstNode();
        AstNode(AstNodeType type);
        AstNode(AstNodeType type, string text);
        ~AstNode();
        void print();
        void append(AstNode *child);
        void remove();
        AstNodeType type();
        void set_type(AstNodeType type);
        const string& text();
        void set_text(string text);
        const string& link();
        void set_link(string link);
        int level();
        void set_level(int level);
    private:
        AstNodeType m_type;
        string m_text;
        string m_link;
        int m_level;
        vector<AstNode*> m_children;
    };

    class Ast
    {
    public:
        Ast();
        ~Ast();
        void print();
        const AstNode* root();
        void set_root(AstNode *root);
    private:
        AstNode *m_root;
    };
    
}

#endif
