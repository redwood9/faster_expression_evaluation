#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__
#include <iostream>
#include <sstream>

using namespace std;

namespace FEE
{
//所有的token类型
enum TokenKind
{
	kBegin,
	//变量
	kVarBegin,
	//变量值为字符串
	kVarString,
	//变量值为数字
	kVarNum,
	//数字
	kNum,
	//字符串
	kString,
	kVarEnd,
	kOperatorBegin,
	//<
	kLT,
	//<=
	kLE,
	//>
	kGT,
	//>=
	kGE,
	//==
	kEqual,
	//!=
	kNotequal,
	//&&/and
	kAnd,
	// ||/or
	kOr,
	//(
	kGroup,
	//)
	kRgroup,
	kOperatorEnd,
	kEnd,
};

//算符优先级
enum PriorityKind
{
	//高优先级
	kHpriority=100,
	//中优先级
	kMpriority=50,
	//低优先级
	kLpriority=1,
};



struct Token;
//算符函数
//==0假，1真，<0出错
typedef int32_t (*OPFUN)(Token *tokenL, Token *tokenR);

//操作符
struct Operator
{
	Operator():literal(""),
		op_kind(kBegin),
		priority(0),
		op_fun(NULL){};

	Operator(std::string literalt,
		TokenKind op_kindt, 
		int priorityt,
		OPFUN op_fun_t):literal(literalt),
		op_kind(op_kindt),
		priority(priorityt),
		op_fun(op_fun_t){};

	//字面字符串
	std::string	literal;
	//算符类型
	TokenKind	op_kind;	
	//算符优先级
	int32_t		priority;	
	OPFUN		op_fun;
};

struct Token
{
	Token():kind(kBegin),
		literal(""),
		val_num(0),
		val_str(""),
		operator_info(NULL),
		is_outvar_(false){};
	//token类型
	TokenKind	kind;			
	//如果是算符, 字面字符串
	std::string	literal;

	//如果是数字型变量, 具体值
	double		val_num;
	//如果是字符型变量, 具体值
	std::string	val_str;

	//如果是算符, 算符的具体信息
	const Operator    *operator_info;	
	bool is_outvar_;
	int32_t Str(string &str)
	{
		stringstream stream_t;
		stream_t<<"|kind="<<kind<<"|literal="<<literal<<"|val_num="<<val_num<<"|val_str="<<val_str<<endl;
		str = stream_t.str();
		return 0;
	};
	int32_t Reset()
	{
		kind = kBegin;
		literal = "";
		val_num = 0;
		val_str = "";
		operator_info = NULL;
		is_outvar_ = false;
		return 0;
	};
};


//小于
int32_t OP_LT_FUN(Token *tokenL, Token *tokenR);

//小于等于
int32_t OP_LE_FUN(Token *tokenL, Token *tokenR);

//大于
int32_t OP_GT_FUN(Token *tokenL, Token *tokenR);

//大于等于
int32_t OP_GE_FUN(Token *tokenL, Token *tokenR);

//等于
int32_t OP_EQUAL_FUN(Token *tokenL, Token *tokenR);

//不等于
int32_t OP_NOEQUAL_FUN(Token *tokenL, Token *tokenR);

//与
int32_t OP_AND_FUN(Token *tokenL, Token *tokenR);

//或
int32_t OP_OR_FUN(Token *tokenL, Token *tokenR);

}

#endif
