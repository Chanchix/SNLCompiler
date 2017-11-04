#ifndef TOKEN_H_
#define TOKEN_H_

#include<unordered_map>
#include<cstring>
#include<list>

using TokenCode = unsigned int;
class Token {
private:
	TokenCode code;													//Token代码
	unsigned int row;												//Token所在行数
	char *identifier;												//Token标识符或错误名称
	typedef std::unordered_map<TokenCode, std::string> mapping;
public:

	Token(const Token &);
	Token(Token &&);											    //move构造函数
	Token(TokenCode, unsigned int = 0, const char* = nullptr);		//词法分析用的构造函数
	Token() :Token(0, 0) {}										    //默认构造函数
    Token& operator=(const Token&);
	bool operator == (const Token &)const;
    
	TokenCode getcode() const { return code; }						//获取Token的代码
	unsigned int getrow() const { return row; }						//获取Token在源文件中的行数
	const char* getid()const { return identifier; }
	std::string tostring(mapping&)const;							//获取Token名称
	~Token();
};
Token::Token(const Token &other) {
	Token(other.code, other.row, other.identifier);
}
Token::Token(Token &&other) {
	code = other.code;
	row = other.row;
	identifier = other.identifier;
	other.identifier = nullptr;
}
Token::Token(TokenCode _code, unsigned int _row, const char *id) :code(_code), row(_row) {
	if (id == nullptr) identifier = nullptr;
	else {
		size_t length = strlen(id);
		identifier = new char[length + 1];
		strcpy(identifier, id);
	}
}
Token& Token::operator=(const Token& other){
    row = other.row;
    code = other.code;
    if(!identifier) delete []identifier;
    if(other.identifier == nullptr) this->identifier = nullptr;
    else {
        identifier = new char[strlen(other.identifier)+1];
        strcpy(identifier, other.identifier);
    }
    return *this;
}
bool Token::operator ==(const Token &other)const {
	if (code != other.code) return false;					//Token代码不同，则Token不同
	else if (!identifier) return true;						//Token代码相同且无语义信息，则Token相同
	else return
		other.identifier &&
		(strcmp(identifier, other.identifier) == 0);		//同为标识符，则比较标识符是否相同
}
std::string Token::tostring(mapping &code_lib) const {
    if (identifier) return std::string(identifier);
	mapping::iterator it = code_lib.find(code);
	if (it != code_lib.end()) return it->second;
	else return std::string("null");
}
Token::~Token() {
	if (!identifier) delete[] identifier;
}
typedef std::list<Token> TokenList;
#endif
