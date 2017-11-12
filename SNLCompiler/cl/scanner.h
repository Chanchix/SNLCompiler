#ifndef _SCANNER_H_
#define _SCANNER_H_
#include<cctype>
#include<sstream>
#include<istream>
#include "lex.h"
#include "token.h"
#include "exalgorithm.h"

#define cstr(sstr) ((sstr).str().c_str())


template<class LEX>
class Scanner{
	std::istream& in;
public:
	Scanner(std::istream& _in): in(_in) {}
	virtual TokenList *getTokenList() = 0;
};

#endif
