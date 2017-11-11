//
//  SNLLex.h
//  SNLCompiler
//
//  Created by 王振吉 on 2017/11/11.
//  Copyright © 2017年 王振吉. All rights reserved.
//

#ifndef SNLLex_h
#define SNLLex_h
#include"lex.h"

class SNLLex : public Lex{
public:
    enum lex {
        end = 0,        unknown_symbol, corrupted_string,                                //øÿ÷∆token
        assign = 14,                                                                    //‘ÀÀ„∑˚
        from_to,        minus,            plus,            multi,            div,
        equal,            smaller_than,    greater_than,    open_paren,        close_paren,
        open_bracket,    close_bracket,    point,            semi,            comma,
        resv_array = 35,                                                                //πÿº¸◊÷
        resv_begin,        resv_integer,    resv_char,        resv_do,        resv_else,
        resv_end,        resv_if,        resv_procedure,    resv_program,    resv_record,
        resv_then,        resv_fi,        resv_type,        resv_var,        resv_while,
        resv_endwh,        resv_read,        resv_write,        resv_return,    resv_of,
        unsigned_integer = 82,                                                            //”– µ“ÂToken
        value_char,        identifier
    };
    SNLLex() :Lex(
                  {
                      {"array", resv_array},                {"begin", resv_begin},
                      {"integer", resv_integer},            {"char", resv_char},
                      {"do", resv_do},                    {"else", resv_else},
                      {"end", resv_end},                    {"if", resv_if},
                      {"record", resv_record},            {"procedure", resv_procedure},
                      { "of", resv_of } ,                    {"program", resv_program},
                      {"then", resv_then},                {"fi", resv_fi},
                      {"type", resv_type},                {"var", resv_var},
                      {"while", resv_while},                { "endwh", resv_endwh },
                      {"read", resv_read},                {"write", resv_write},
                      {"return", resv_return}
                  },
                  
                  {
                      {end, "program_end"},
                      {corrupted_string, "corrupted_string"},
                      {unknown_symbol, "unknown_symbol"},
                      {assign, ":="},            {comma, ","},                {from_to, ".."},            {minus, "-"},
                      {plus, "+"},            {multi, "*"},                {div, "/"},                    {equal, "="},
                      {smaller_than, "<"},    {greater_than, ">"},        {open_paren, "("},            {close_paren, ")"},
                      {open_bracket, "["},    {close_bracket, "]"},        {point, "."},                {semi, ";"},
                      {resv_array, "array"},    {resv_begin, "begin"},        {resv_integer, "integer"},    {resv_char, "char"},
                      {resv_do, "do"},        {resv_else, "else"},        {resv_end, "end"},            {resv_if, "if"},
                      { resv_then, "then" },    { resv_fi, "fi" },            {resv_record, "record"},    {resv_procedure, "procedure"},
                      { resv_type, "type" },    {resv_program, "program" },    {resv_of, "of" },            { value_char, "char" },
                      {resv_var, "var"},        {resv_while, "while"},        {resv_endwh, "endwh"},        {resv_read, "read"},
                      {resv_write, "write"},    {resv_return, "return"},    {identifier, "id"},            {unsigned_integer, "unsigned integer"}
                  }) {};
public:
    bool isLexCode(LexCode code) const override{ return code <= identifier && code >= end; }
    LexCode getEnd() const override{ return end; }
    std::string code_to_string(LexCode code)const override{
        _CTW::const_iterator it = code_to_word.find(code);
        if (it != code_to_word.end()) return it->second;
        else throw Decodable::CodeNotFoundException(code);
    }
    LexCode string_to_code(const std::string &str)const override{
        _WTC::const_iterator it = word_to_code.find(str);
        if(it != word_to_code.end()) return it->second;
        else throw Encodable::StringNotFoundException(str);
    }
};

#endif /* SNLLex_h */
