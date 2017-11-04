#ifndef LEX_H_
#define LEX_H_

using LexCode = unsigned int;
#include <string>
#include <unordered_map>

class Lex {
public:
	using _WTC = std::unordered_map<std::string, LexCode>;
	using _CTW = std::unordered_map<LexCode, std::string>;

	_WTC word_to_code;	
	_CTW code_to_word;	//错误处理时使用的，由token的代码转换为词义信息

	Lex(_WTC&& _wtc, _CTW&& _ctw): word_to_code(std::move(_wtc)), code_to_word(std::move(_ctw)) {}
	virtual bool isLexCode(LexCode code)const = 0;
	virtual LexCode getEnd() const = 0;
};
class SNLLex : public Lex{
public:
	enum lex {
		end = 0,		unknown_symbol, corrupted_string,								//控制token
		assign = 14,																	//运算符	
		from_to,		minus,			plus,			multi,			div,			
		equal,			smaller_than,	greater_than,	open_paren,		close_paren,	
		open_bracket,	close_bracket,	point,			semi,			comma,
		resv_array = 35,																//关键字
		resv_begin,		resv_integer,	resv_char,		resv_do,		resv_else,
		resv_end,		resv_if,		resv_procedure,	resv_program,	resv_record,
		resv_then,		resv_fi,		resv_type,		resv_var,		resv_while,
		resv_endwh,		resv_read,		resv_write,		resv_return,	resv_of,
		unsigned_integer = 82,															//有实义Token
		value_char,		identifier
	};
	SNLLex() :Lex(
	{
		{"array", resv_array},				{"begin", resv_begin},
		{"integer", resv_integer},			{"char", resv_char},
		{"do", resv_do},					{"else", resv_else},
		{"end", resv_end},					{"if", resv_if},
		{"record", resv_record},			{"procedure", resv_procedure},
		{ "of", resv_of } ,					{"program", resv_program},
		{"then", resv_then},				{"fi", resv_fi},
		{"type", resv_type},				{"var", resv_var},
		{"while", resv_while},				{ "endwh", resv_endwh },
		{"read", resv_read},				{"write", resv_write},
		{"return", resv_return}
	},

	{
		{end, "program_end"},
		{corrupted_string, "corrupted_string"},
		{unknown_symbol, "unknown_symbol"},
		{assign, ":="},			{comma, ","},				{from_to, ".."},			{minus, "-"},
		{plus, "+"},			{multi, "*"},				{div, "/"},					{equal, "="},
		{smaller_than, "<"},	{greater_than, ">"},		{open_paren, "("},			{close_paren, ")"},
		{open_bracket, "["},	{close_bracket, "]"},		{point, "."},				{semi, ";"},
		{resv_array, "array"},	{resv_begin, "begin"},		{resv_integer, "integer"},	{resv_char, "char"},
		{resv_do, "do"},		{resv_else, "else"},		{resv_end, "end"},			{resv_if, "if"},
		{ resv_then, "then" },	{ resv_fi, "fi" },			{resv_record, "record"},	{resv_procedure, "procedure"},
		{ resv_type, "type" },	{resv_program, "program" },	{resv_of, "of" },			{ value_char, "char" },
		{resv_var, "var"},		{resv_while, "while"},		{resv_endwh, "endwh"},		{resv_read, "read"},
		{resv_write, "write"},	{resv_return, "return"},	{identifier, "id"},			{unsigned_integer, "unsigned integer"}
	}) {};
public:
	bool isLexCode(LexCode code) const{ return code <= identifier && code >= end; }
	LexCode getEnd() const { return end; }
};
#endif
