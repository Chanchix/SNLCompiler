#ifndef GRAMMER_H_ 
#define GRAMMER_H_ 

#include<sstream>
#include<vector>
#include<cassert>
#include<list>
#include<string>
#include<map>

#include"lex.h"
#include"exAlgorithm.h"

using GrammerCode = unsigned int;
namespace grammer {
	
	class Rule {					
		GrammerCode left;
		std::vector<GrammerCode> right;
	public:
		Rule(const std::vector<GrammerCode>& rule) {
			left = rule[0];
			for (auto iter = rule.begin() + 1; iter != rule.end(); iter++)
				right.push_back(*iter);
		}
		Rule(GrammerCode l, const std::vector<GrammerCode>& r) :right(r), left(l) {}
		Rule(GrammerCode l, std::vector<GrammerCode>&& r) :right(r), left(l) {}
		size_t length() const { return right.size(); }
		GrammerCode& operator[](size_t index) { return right[index]; }
		GrammerCode operator[](size_t index) const{ return right[index]; }
		GrammerCode getLeft() const { return left; }
		bool empty() const { return right.empty(); }
		size_t size() const { return right.size(); }
		const std::vector<GrammerCode>& getRight()const { return right; }
		std::string toString() const{
			std::ostringstream sout;
			sout << left << " -> ";
			for (const auto &right_code : right) sout << right_code << " ";
			if (right.size() == 0) sout << "e";
			return sout.str();
		}
		bool operator ==(const Rule &other) { return left == other.left && right == other.right; }
	};
	using Rules = std::map<GrammerCode, std::list<Rule>>;

	template<class LEX>
	class Grammer {
	protected:
		Rules rules;
		const GrammerCode start;
	public:
		Grammer(GrammerCode _start, Rules &&rules) :start(_start), rules(std::move(rules)) {}
		virtual bool IsGrammerCode(GrammerCode) = 0;
		static bool IsLexCode(LexCode c) { return _Singleton(LEX).isLexCode(c); }

		const Rules& getRules()const { return rules; }
		GrammerCode getStart()const { return start; }
		
		void check() {
			assert(IsGrammerCode(start));
			for (auto &pair : rules) {
				GrammerCode left = pair.first;
				assert(IsGrammerCode(left));
				for (auto &rule : pair.second) {
					assert(IsGrammerCode(rule.getLeft()) && left == rule.getLeft());
					for (auto right : rule.getRight()) {
						assert(IsGrammerCode(right) || IsLexCode(right));
					}
				}
			}
		}
	};
	
	template<class LEX>
	class LR1Grammer : public Grammer<LEX>{
	protected:	
		const GrammerCode empty;
		const GrammerCode ExCode;
		
		Rule exRule;
		
		void check() {
			#if _DEBUG
			assert(rules.find(ExCode) == rules.end());
			assert(IsGrammerCode(ExCode));
			Grammer::check();
			#endif
		}
	public:
		LR1Grammer(Rules &&rules, LexCode _start, LexCode _ex):
			Grammer<LEX>(_start, std::move(rules)), 
			empty(_Singleton(LEX).getEnd()),
			ExCode(_ex), 
			exRule(Rule(ExCode, { _start })) 
		{}
		const Rule& getExRule()const { return exRule;}
		GrammerCode getEmpty()const { return empty; }
		static LexCode getLexEnd() { return _Singleton(LEX).getEnd(); }
		GrammerCode getExcode() const { return ExCode; }
	};
}
#endif
