#ifndef SNL_GRAMMER_H_ 
#define SNL_GRAMMER_H_ 
#include"exAlgorithm.h"
#include"lex.h"
#include"grammer.h"

class SNLGrammer : public grammer::LR1Grammer<SNLLex> {
public:
	friend class exAlgorithm::Singleton<SNLGrammer>;
	enum grammerCode {
		ExCode = 100,
		Program,	ProgramHead,	ProgramName,	DeclarePart,	TypeDecpart,
		TypeDec,	TypeDecList,	TypeDecMore,	TypeId,			TypeDef,
		BaseType,	StructureType,	ArrayType,		Low,			Top,
		RecType,	FieldDecList,	FieldDecMore,	IdList,			IdMore,
		VarDecpart, VarDec,			VarDecList,		VarDecMore,		VarIdList,
		VarIdMore,	ProcDecpart,	ProcDec,		ProcDecMore,	ProcName,
		ParamList,	ParamDecList,	ParamMore,		Param,			FormList,
		FidMore,	ProcDecPart,	ProcBody,		ProgramBody,	StmList,
		StmMore,	Stm,			AssCall,		AssignmentRest, ConditionalStm,
		LoopStm,	InputStm,		Invar,			OutputStm,		ReturnStm,
		CallStmRest,ActParamList,	ActParamMore,	RelExp,			OtherRelE,
		Exp,		OtherTerm,		Term,			OtherFactor,	Factor,
		Variable,	VariMore,		FieldVar,		FieldVarMore,	CmpOp,
		AddOp,		MultOp
	};
private:
	SNLGrammer() :LR1Grammer(
		grammer::Rules({
			{ Program,{
				{ Program,{ ProgramHead, DeclarePart, ProgramBody, SNLLex::point } }
			} },
			{ ProgramHead,{
				{ ProgramHead,{ SNLLex::resv_program, ProgramName } }
			} },
			{ ProgramName,{
				{ ProgramName,{ SNLLex::identifier } }
			} },
			{ DeclarePart,{
				{ DeclarePart,{ TypeDecpart, VarDecpart, ProcDecpart } }
			} },
			{ TypeDecpart,{
				{ TypeDecpart,{} },
				{ TypeDecpart,{ TypeDec } }
			} },
			{ TypeDec,{
				{ TypeDec,{ SNLLex::resv_type, TypeDecList } }
			} },
			{ TypeDecList,{
				{ TypeDecList,{ TypeId, SNLLex::equal, TypeDef, SNLLex::semi, TypeDecMore } }
			} },
			{ TypeDecMore,{
				{ TypeDecMore,{} },
				{ TypeDecMore,{ TypeDecList } }
			} },
			{ TypeId,{
				{ TypeId,{ SNLLex::identifier } }
			} },
			{ TypeDef,{
				{ TypeDef,{ BaseType } },
				{ TypeDef,{ StructureType } },
				{ TypeDef,{ SNLLex::identifier } }
			} },
			{ BaseType,{
				{ BaseType,{ SNLLex::resv_integer } },
				{ BaseType,{ SNLLex::resv_char } }
			} },
			{ StructureType,{
				{ StructureType,{ ArrayType } },
				{ StructureType,{ RecType } }
			} },
			{ ArrayType,{
				{ ArrayType,{ SNLLex::resv_array, SNLLex::open_bracket, Low, SNLLex::from_to, Top, SNLLex::close_bracket, SNLLex::resv_of, BaseType } }
			} },
			{ Low,{
				{ Low,{ SNLLex::unsigned_integer } }
			} },
			{ Top,{
				{ Top,{ SNLLex::unsigned_integer } }
			} },
			{ RecType,{
				{ RecType,{ SNLLex::resv_record, FieldDecList, SNLLex::resv_end } }
			} },
			{ FieldDecList,{
				{ FieldDecList,{ BaseType, IdList, SNLLex::semi, FieldDecMore } },
				{ FieldDecList,{ ArrayType, IdList, SNLLex::semi, FieldDecMore } }
			} },
			{ FieldDecMore,{
				{ FieldDecMore,{} },
				{ FieldDecMore,{ FieldDecList } }
			} },
			{ IdList,{
				{ IdList,{ SNLLex::identifier, IdMore } }
			} },
			{ IdMore,{
				{ IdMore,{} },
				{ IdMore,{ SNLLex::comma, IdList } }
			} },
			{ VarDecpart,{
				{ VarDecpart,{} },
				{ VarDecpart,{ VarDec } }
			} },
			{ VarDec,{
				{ VarDec,{ SNLLex::resv_var, VarDecList } }
			} },
			{ VarDecList,{
				{ VarDecList,{ TypeDef, VarIdList, SNLLex::semi, VarDecMore } }
			} },
			{ VarDecMore,{
				{ VarDecMore,{} },
				{ VarDecMore,{ VarDecList } }
			} },
			{ VarIdList,{
				{ VarIdList,{ SNLLex::identifier, VarIdMore } }
			} },
			{ VarIdMore,{
				{ VarIdMore,{} },
				{ VarIdMore,{ SNLLex::comma, VarIdList } }
			} },
			{ ProcDecpart,{
				{ ProcDecpart,{} },
				{ ProcDecpart,{ ProcDec } }
			} },
			{ ProcDec,{
				{ ProcDec,{ SNLLex::resv_procedure, ProcName, SNLLex::open_paren, ParamList,
				SNLLex::close_paren, SNLLex::semi, ProcDecPart, ProcBody, ProcDecMore } },
			} },
			{ ProcDecMore,{
				{ ProcDecMore,{} },
				{ ProcDecMore,{ ProcDec } }
			} },
			{ ProcName,{
				{ ProcName,{ SNLLex::identifier } }
			} },
			{ ParamList,{
				{ ParamList,{} },
				{ ParamList,{ ParamDecList } }
			} },
			{ ParamDecList,{
				{ ParamDecList,{ Param, ParamMore } }
			} },
			{ ParamMore,{
				{ ParamMore,{} },
				{ ParamMore,{ SNLLex::semi, ParamDecList } }
			} },
			{ Param,{
				{ Param,{ TypeDef, FormList } },
				{ Param,{ SNLLex::resv_var, TypeDef, FormList } }
			} },
			{ FormList,{
				{ FormList,{ SNLLex::identifier, FidMore } }
			} },
			{ FidMore,{
				{ FidMore,{} },
				{ FidMore,{ SNLLex::comma, FormList } }
			} },
			{ ProcDecPart,{
				{ ProcDecPart,{ DeclarePart } }
			} },
			{ ProcBody,{
				{ ProcBody,{ ProgramBody } }
			} },
			{ ProgramBody,{
				{ ProgramBody,{ SNLLex::resv_begin, StmList, SNLLex::resv_end } }
			} },
			{ StmList,{
				{ StmList,{ Stm, StmMore } }
			} },
			{ StmMore,{
				{ StmMore,{} },
				{ StmMore,{ SNLLex::semi, StmList } }
			} },
			{ Stm,{
				{ Stm,{ ConditionalStm } },
				{ Stm,{ LoopStm } },
				{ Stm,{ InputStm } },
				{ Stm,{ OutputStm } },
				{ Stm,{ ReturnStm } },
				{ Stm,{ SNLLex::identifier, AssCall } }
			} },
			{ AssCall,{
				{ AssCall,{ AssignmentRest } },
				{ AssCall,{ CallStmRest } }
			} },
			{ AssignmentRest,{
				{ AssignmentRest,{ VariMore, SNLLex::assign, Exp } }
			} },
			{ ConditionalStm,{
				{ ConditionalStm,{ SNLLex::resv_if, RelExp, SNLLex::resv_then, StmList, SNLLex::resv_else, StmList, SNLLex::resv_fi } }
			} },
			{ LoopStm,{
				{ LoopStm,{ SNLLex::resv_while, RelExp, SNLLex::resv_do, StmList, SNLLex::resv_endwh } }
			} },
			{ InputStm,{
				{ InputStm,{ SNLLex::resv_read, SNLLex::open_paren, Invar, SNLLex::close_paren } }
			} },
			{ Invar,{
				{ Invar,{ SNLLex::identifier } }
			} },
			{ OutputStm,{
				{ OutputStm,{ SNLLex::resv_write, SNLLex::open_paren, Exp, SNLLex::close_paren } }
			} },
			{ ReturnStm,{
				{ ReturnStm,{ SNLLex::resv_return } }
			} },
			{ CallStmRest,{
				{ CallStmRest,{ SNLLex::open_paren, ActParamList, SNLLex::close_paren } }
			} },
			{ ActParamList,{
				{ ActParamList,{} },
				{ ActParamList,{ Exp, ActParamMore } }
			} },
			{ ActParamMore,{
				{ ActParamMore,{} },
				{ ActParamMore,{ SNLLex::comma, ActParamList } }
			} },
			{ RelExp,{
				{ RelExp,{ Exp, OtherRelE } }
			} },
			{ OtherRelE,{
				{ OtherRelE,{ CmpOp, Exp } }
			} },
			{ Exp,{
				{ Exp,{ Term, OtherTerm } }
			} },
			{ OtherTerm,{
				{ OtherTerm,{} },
				{ OtherTerm,{ AddOp, Exp } }
			} },
			{ Term,{
				{ Term,{ Factor, OtherFactor } }
			} },
			{ OtherFactor,{
				{ OtherFactor,{} },
				{ OtherFactor,{ MultOp, Term } }
			} },
			{ Factor,{
				{ Factor,{ SNLLex::open_paren, Exp, SNLLex::close_paren } },
				{ Factor,{ SNLLex::unsigned_integer } },
				{ Factor,{ Variable } }
			} },
			{ Variable,{
				{ Variable,{ SNLLex::identifier, VariMore } }
			} },
			{ VariMore,{
				{ VariMore,{} },
				{ VariMore,{ SNLLex::open_bracket, Exp, SNLLex::close_bracket } },
				{ VariMore,{ SNLLex::point, FieldVar } }
			} },
			{ FieldVar,{
				{ FieldVar,{ SNLLex::identifier, FieldVarMore } }
			} },
			{ FieldVarMore,{
				{ FieldVarMore,{} },
				{ FieldVarMore,{ SNLLex::open_bracket, Exp, SNLLex::close_bracket } }
			} },
			{ CmpOp,{
				{ CmpOp,{ SNLLex::smaller_than } },
				{ CmpOp,{ SNLLex::equal } }
			} },
			{ AddOp,{
				{ AddOp,{ SNLLex::plus } },
				{ AddOp,{ SNLLex::minus } }
			} },
			{ MultOp,{
				{ MultOp,{ SNLLex::multi } },
				{ MultOp,{ SNLLex::div } }
			} }
		}),Program, ExCode){
		check();
	}
public:
	virtual bool IsGrammerCode(GrammerCode code) { return code >= ExCode && code <= MultOp; }
	virtual bool IsLexCode(GrammerCode code) { return _Singleton(SNLLex).isLexCode(code); }
};

#endif
