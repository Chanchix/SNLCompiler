#ifndef LEX_H_
#define LEX_H_

using LexCode = unsigned int;
#include <string>
#include <unordered_map>
#include"interface.h"

class Lex : public Decodable, public Encodable{
protected:
	using _WTC = std::unordered_map<std::string, LexCode>;
	using _CTW = std::unordered_map<LexCode, std::string>;

	_WTC word_to_code;	
	_CTW code_to_word;	//错误处理时使用的，由token的代码转换为词义信息
public:
	Lex(_WTC&& _wtc, _CTW&& _ctw): word_to_code(std::move(_wtc)), code_to_word(std::move(_ctw)) {}
	virtual bool isLexCode(LexCode code)const = 0;
	virtual LexCode getEnd() const = 0;
};

#endif
