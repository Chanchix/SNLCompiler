// SNLCompiler.cpp : 定义控制台应用程序的入口点。
//


#include<iostream>
#include<fstream>
#include<string>



#include"cl/lr1driver.h"
#include"cl/parser.h"

#include"snl_lex.h"
#include"snl_scanner.h"
#include"snl_grammer.h"
#include"snl_corrector.h"


#include"cl/exalgorithm.h"
#define isLA true


namespace test {	
	void testLR() {
		using Driver = LR1Driver<SNLGrammer>;
		const auto &stm = _Singleton(Driver).getStateMachine();
		stm.print();
	}
    void testParser(){
        std::ifstream fin("snl.txt");
        if(fin.bad()) {
            std::cout << "Can not open snl.txt\n";
            fin.close();
            std::cin.get();
            return;
        }
        Scanner<SNLLex> scanner(fin);
        TokenList *tList = scanner.getTokenList();
        using Driver = LR1Driver<SNLGrammer>;
        LR1Parser<Driver> parser;
        auto *stree = parser.getSyntaxTree(tList);
        if(stree) delete stree;
        fin.close();
    }
}
int main(int argc, char* argv[])
{
    test::testParser();
	return 0;
}
