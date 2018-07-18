#include <iostream>
#include "ast.hpp"

namespace marked {

    AstNode::AstNode():
    AstNode(AT_NONE, "")
    {
    }

    AstNode::AstNode(AstNodeType type):
    AstNode(type, "")
    {
    }

    AstNode::AstNode(AstNodeType type, string text):
    m_type(type),
    m_text(text),
    m_link(""),
    m_level(1)
    {
    }

    AstNode::~AstNode()
    {
        while (!m_children.empty())
        {
            remove();
        }
    }

    void AstNode::print()
    {
        using std::cout;
        using std::endl;
        
        if (m_type == AT_HORIZONTAL_RULE)
        {
            cout << "<hr />";
            return;
        }
        
        if (m_type == AT_CODE_BLOCK)
        {
            cout << "<p><code>" << m_text << "</code></p>";
            return;
        }
        
        if (m_type == AT_CODE)
        {
            cout << "<code>" << m_text << "</code>";
            return;
        }
        
        if (m_type == AT_IMAGE)
        {
            cout << "<img src=\"" << m_link << "\" alt=\"" << m_text << "\" />";
            return;
        }
        
        if (m_type == AT_LINK)
        {
            cout << "<a href=\"" << m_link << "\">" << m_text << "</a>";
            return;
        }
        
        if (m_type == AT_TEXT)
        {
            cout << m_text;
            return;
        }
        
        if (m_type == AT_BODY)
            cout << "<body>";
        else if (m_type == AT_HEADING)
            cout << "<h" << m_level << ">";
        else if (m_type == AT_PARAGRAPH)
            cout << "<p>";
        else if (m_type == AT_ORDERED_LIST)
            cout << "<ol>";
        else if (m_type == AT_UNORDERED_LIST)
            cout << "<ul>";
        else if (m_type == AT_LISTITEM)
            cout << "<li>";
        else if (m_type == AT_BLOCKQOUTE)
            cout << "<blockquote>";
        else if (m_type == AT_EMPHASIS)
            cout << "<em>";
        else if (m_type == AT_BOLD)
            cout << "<strong>";
        
        for (int i = 0; i < m_children.size(); i++)
        {
            m_children[i]->print();
        }
        
        if (m_type == AT_BODY)
            cout << "</body>";
        else if (m_type == AT_HEADING)
            cout << "</h" << m_level << ">";
        else if (m_type == AT_PARAGRAPH)
            cout << "</p>";
        else if (m_type == AT_ORDERED_LIST)
            cout << "</ol>";
        else if (m_type == AT_UNORDERED_LIST)
            cout << "</ul>";
        else if (m_type == AT_LISTITEM)
            cout << "</li>";
        else if (m_type == AT_BLOCKQOUTE)
            cout << "</blockquote>";
        else if (m_type == AT_EMPHASIS)
            cout << "</em>";
        else if (m_type == AT_BOLD)
            cout << "</strong>";
    }

    void AstNode::append(AstNode *child)
    {
        m_children.push_back(child);
    }
    
    void AstNode::remove()
    {
        AstNode *child = m_children.back();
        m_children.pop_back();
        
        delete child;
    }

    AstNodeType AstNode::type()
    {
        return m_type;
    }

    void AstNode::set_type(AstNodeType type)
    {
        m_type = type;
    }

    const string& AstNode::text()
    {
        return m_text;
    }

    void AstNode::set_text(string text)
    {
        m_text = text;
    }

    const string& AstNode::link()
    {
        return m_link;
    }

    void AstNode::set_link(string link)
    {
        m_link = link;
    }

    int AstNode::level()
    {
        return m_level;
    }

    void AstNode::set_level(int level)
    {
        m_level = level;
    }

    Ast::Ast():
    m_root(new AstNode(AT_NONE))
    {
    }

    Ast::~Ast()
    {
        delete m_root;
    }

    void Ast::print()
    {
        using std::cout;
        using std::endl;
        
        m_root->print();
        cout << endl;
    }

    const AstNode* Ast::root()
    {
        return m_root;
    }

    void Ast::set_root(AstNode *root)
    {
        delete m_root;
        m_root = root;
    }

}
