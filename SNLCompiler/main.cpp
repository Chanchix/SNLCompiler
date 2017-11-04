// SNLCompiler.cpp : 定义控制台应用程序的入口点。
//


#include<iostream>
#include<fstream>
#include<string>



#include"LR1Driver.h"
#include"SNLGrammer.h"
#include"parser.h"
#include"scanner.h"
#include"exAlgorithm.h"
#define isLA true


namespace test {	
	void testLR() {
		using Driver = LR1Driver<SNLGrammer>;
		exAlgorithm::Timer timer;
		timer.start();
		const auto &stm = _Singleton(Driver).getStateMachine();
		timer.end();
		std::cout << "Running Time : " << timer.getDuration() << "s" << std::endl << std::endl;
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
        auto *tree = parser.getSyntaxTree(tList);
        if(tree) delete tree;
        fin.close();
    }
}

int main(int argc, char* argv[])
{
    test::testParser();
	return 0;
}
