#ifndef TOKEN_H_
#define TOKEN_H_

#include<unordered_map>
#include<cstring>
#include<list>

using TokenCode = unsigned int;
class Token {
private:
	TokenCode code;													//Token����
	unsigned int row;												//Token��������
	char *identifier;												//Token��ʶ�����������
	typedef std::unordered_map<TokenCode, std::string> mapping;
public:

	Token(const Token &);
	Token(Token &&);											    //move���캯��
	Token(TokenCode, unsigned int = 0, const char* = nullptr);		//�ʷ������õĹ��캯��
	Token() :Token(0, 0) {}										    //Ĭ�Ϲ��캯��
    Token& operator=(const Token&);
	bool operator == (const Token &)const;
    
	TokenCode getcode() const { return code; }						//��ȡToken�Ĵ���
	unsigned int getrow() const { return row; }						//��ȡToken��Դ�ļ��е�����
	const char* getid()const { return identifier; }
	std::string tostring(mapping&)const;							//��ȡToken����
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
	if (code != other.code) return false;					//Token���벻ͬ����Token��ͬ
	else if (!identifier) return true;						//Token������ͬ����������Ϣ����Token��ͬ
	else return
		other.identifier &&
		(strcmp(identifier, other.identifier) == 0);		//ͬΪ��ʶ������Ƚϱ�ʶ���Ƿ���ͬ
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
