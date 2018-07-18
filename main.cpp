#include <streambuf>
#include <fstream>
#include <sstream>
#include "parser.hpp"

int main(int argc, char* argv[])
{
    using std::stringstream;
    using std::ifstream;
    using std::string;
    using std::cout;
    using std::endl;
    
    if (argc < 2) {
        cout << "marked need a markdown file" << endl;
        exit(-1);
    }

    // 读取文件内容
    ifstream file(argv[1]);
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    string input(buffer.str());
    
    marked::Parser markdown;
    markdown.parse(input);
    
    return 0;
}
