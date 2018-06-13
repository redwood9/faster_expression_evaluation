#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__
#include <iostream>
#include <sstream>

using namespace std;

namespace FEE
{
//���е�token����
enum TokenKind
{
	kBegin,
	//����
	kVarBegin,
	//����ֵΪ�ַ���
	kVarString,
	//����ֵΪ����
	kVarNum,
	//����
	kNum,
	//�ַ���
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

//������ȼ�
enum PriorityKind
{
	//�����ȼ�
	kHpriority=100,
	//�����ȼ�
	kMpriority=50,
	//�����ȼ�
	kLpriority=1,
};



struct Token;
//�������
//==0�٣�1�棬<0����
typedef int32_t (*OPFUN)(Token *tokenL, Token *tokenR);

//������
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

	//�����ַ���
	std::string	literal;
	//�������
	TokenKind	op_kind;	
	//������ȼ�
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
	//token����
	TokenKind	kind;			
	//��������, �����ַ���
	std::string	literal;

	//����������ͱ���, ����ֵ
	double		val_num;
	//������ַ��ͱ���, ����ֵ
	std::string	val_str;

	//��������, ����ľ�����Ϣ
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


//С��
int32_t OP_LT_FUN(Token *tokenL, Token *tokenR);

//С�ڵ���
int32_t OP_LE_FUN(Token *tokenL, Token *tokenR);

//����
int32_t OP_GT_FUN(Token *tokenL, Token *tokenR);

//���ڵ���
int32_t OP_GE_FUN(Token *tokenL, Token *tokenR);

//����
int32_t OP_EQUAL_FUN(Token *tokenL, Token *tokenR);

//������
int32_t OP_NOEQUAL_FUN(Token *tokenL, Token *tokenR);

//��
int32_t OP_AND_FUN(Token *tokenL, Token *tokenR);

//��
int32_t OP_OR_FUN(Token *tokenL, Token *tokenR);

}

#endif
