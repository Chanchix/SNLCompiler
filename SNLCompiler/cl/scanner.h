#ifndef _SCANNER_H_
#define _SCANNER_H_
#include<cctype>
#include<sstream>
#include<istream>
#include "lex.h"
#include "token.h"
#include "exAlgorithm.h"

#define cstr(sstr) ((sstr).str().c_str())


template<class LEX>
class Scanner{
	std::istream& in;
public:
	Scanner(std::istream& _in): in(_in) {}
	virtual TokenList *getTokenList() = 0;
};
template<>
class Scanner<SNLLex> {
private:
	enum State {
		Normal, InId, InNum, InComment, InChar,
		InAssign, InRange
	};
	unsigned int row;			//词法分析器处理到的行数
	char ch;			//输入流当前字符
	bool read_flag;		//是否需要读入一个字符
	bool read_char;		//正在读取字符串
	bool next;			//是否有下一个Token
	std::istream &in;
	char getChar();
	static Token createIdToken(const char *, int);
	Token getToken();
public:
	Scanner(std::istream& _in) :in(_in), row(1) {}
	TokenList *getTokenList();
};
char Scanner<SNLLex>::getChar() {
	if (!read_flag) read_flag = true;
	else {
		if (ch == '\n') row++;
		in.get(ch);
	}
	if (!in) return -1;
	if (ch == ' ' || ch == '\t') {
		if (read_char) return ch;
		else return 0;
	}
	else if (ch == '\n') {
		return 0;
	}
	else return ch;
}
Token Scanner<SNLLex>::createIdToken(const char *id, int row) {
	SNLLex &lex = _Singleton(SNLLex);
    try{
        return Token(lex.string_to_code(id), row);
    }catch(const Encodable::StringNotFoundException & e){
        return Token(SNLLex::identifier, row, id);
    }
}

TokenList* Scanner<SNLLex>::getTokenList() {
	TokenList *tokenList = new TokenList();
	ch = 0;
    row = 1;
	read_flag = true;
	read_char = false;
	next = true;
	while (next) tokenList->push_back(getToken());
	return tokenList;
}
Token Scanner<SNLLex>::getToken() {
	State state = Normal;
	std::ostringstream id_builder;
	char ch = getChar();
	while (ch != -1) {
		switch (state) {
		case Normal:
			if (!ch) break;
			if (isalpha(ch)) {
				state = InId; id_builder << ch;
			}
			else if (isdigit(ch)) {
				state = InNum; id_builder << ch;
			}
			else switch (ch) {
			case '+': return Token(SNLLex::plus, row);
			case '-': return Token(SNLLex::minus, row);
			case '*': return Token(SNLLex::minus, row);
			case '/': return Token(SNLLex::div, row);
			case '(': return Token(SNLLex::open_paren, row);
			case ')': return Token(SNLLex::close_paren, row);
			case '[': return Token(SNLLex::open_bracket, row);
			case ']': return Token(SNLLex::close_bracket, row);
			case ';': return Token(SNLLex::semi, row);
			case ',': return Token(SNLLex::comma, row);
			case '=': return Token(SNLLex::equal, row);
			case '<': return Token(SNLLex::smaller_than, row);
			case'.':
				state = InRange; break;
			case ':':
				state = InAssign; break;
			case '{':
				state = InComment; break;
			case '\'':
				state = InChar;
				read_char = true;
				id_builder << ch;
				break;
			default:
				id_builder << ch;
				return Token(SNLLex::unknown_symbol, row, cstr(id_builder));
			}
			break;
		case InId:
			if (!ch) return createIdToken(cstr(id_builder), row);
			if (!isalpha(ch) && !isdigit(ch)) {
				read_flag = false;
				return createIdToken(cstr(id_builder), row);
			}
			else id_builder << ch;
			break;
		case InNum:
			if (!ch) return Token(SNLLex::unsigned_integer, row, cstr(id_builder));
			if (!isdigit(ch)) {
				read_flag = false;
				return Token(SNLLex::unsigned_integer, row, cstr(id_builder));
			}
			else id_builder << ch;
			break;
		case InAssign:
			if (ch == '=') return Token(SNLLex::assign, row);
			else {
				read_flag = false;
				return Token(SNLLex::unknown_symbol, row, ":");
			}
			break;
		case InComment:
			while (ch != -1 && ch != '}') {
				ch = getChar();
			}//一直到注释结束，注释结束后继续识别
			state = Normal;
			if (ch == -1) {
				next = false;
				return Token(SNLLex::end, row);
			}
			break;
		case InRange:
			if (ch == '.') return Token(SNLLex::from_to, row);
			else {
				read_flag = false;
				return Token(SNLLex::point, row);
			}
			break;
		case InChar:
			if (!ch) {
				read_char = false;
				return Token(SNLLex::corrupted_string, row, cstr(id_builder));
			}
			else {
				id_builder << ch;
				if (ch == '\'') {
					read_char = false;
					return Token(SNLLex::value_char, row, cstr(id_builder));
				}
			}
			break;
		}
		ch = getChar();
	}
	if (state == InRange) return Token(SNLLex::point, row);
	else if (state == InId) return createIdToken(cstr(id_builder), row);
	else if (state == InNum) return Token(SNLLex::unsigned_integer, row, cstr(id_builder));
	else if (state == InAssign) return Token(SNLLex::unknown_symbol, row, ":");
	else if (state == InRange) return Token(SNLLex::point, row);
	else {				//InComment和Normal
		next = false;
		return Token(SNLLex::end, row);
	}
}
#endif
