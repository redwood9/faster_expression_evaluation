/**
* @auth   arvincheng
* @brief   ������ʽ����
	ʹ�÷�ʽ����
	MSDK::Calculater cal;
	cal.SetNum("varnum",12);
	cal.SetNum("varnuma",1);
	cal.SetString("varstring","a");
	cal.SetString("varstringb","c");
	int ret = cal.Cal("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")",msg);

	ret<0:����; ==0���ʽ���Ϊfalse; ==1���ʽ���Ϊtrue

*/
#ifndef __CALCULATER_HPP_
#define __CALCULATER_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <list>
#include <sstream>
#include <ext/hash_map>
#include <tr1/unordered_map>
#include <string>
#include <string.h>

#include "alloc_poll.h"
#include "oprator_def.h"
#include "struct.h"



namespace FEE
{

using namespace std;
using namespace std::tr1;

const int kCalBase							= -1000;
//ptrָ���쳣
const int kCalPtrIsNull						= kCalBase - 1;
//û���ҵ�
const int kDontFindFail						= kCalBase - 2;
//������̫���
const int kNumTooMuchPoint					= kCalBase - 3;
//�����е����û�н�����
const int kNumAfterPointDontFindNum			= kCalBase - 4;
//�ⲿ��������̫���
const int kOutsideVarTooMuchPoint			= kCalBase - 5;
//�ⲿ���������û�н���ĸ
const int kOutsideVarAfterPointDontFindChar	= kCalBase - 6;
//�﷨��
const int kSyntaxFail						= kCalBase - 7;
//���Ų�ƥ��
const int kGroupDontMatch					= kCalBase - 8;
//������ֻ��һ��
const int kOperatorDontMatch				= kCalBase - 9;
//ȱ�����
const int kDontFindOprator					= kCalBase - 10;
//��ֵ����ֵ���Ͳ�ƥ��
const int kLRVarKindDontMatch				= kCalBase - 11;
//���Ҫ��ı������Ͳ���
const int kOPNeedVarKindDontMatch			= kCalBase - 12;
//�����Ӧ�ĺ�����NULL
const int kOPFunIsNull						= kCalBase - 14;
//���յļ�����LIST�ڵ���������,�������ֻ����һ��
const int kFinalListSizeFail				= kCalBase - 15;
//������������ĸ��ͷ
const int kVarMustBeChar					= kCalBase - 16;
//�ַ���"��ƥ��
const int kVarQuotationDontMatch			= kCalBase - 17;
//����������	
const int kVarDontExist						= kCalBase - 18;

#define RET(code, msgt) {msg=msgt;return code;}




struct equal_str
{
	bool operator()(const char* s1, const char *s2)
	{
		return strcmp(s1,s2)==0;
	};
};

//typedef hash_map<const char*, Operator,hash<const char*>,equal_str> oprator_hash_map;
typedef unordered_map<string,Token>			OutsideVarMap;
typedef unordered_map<string, Operator>		oprator_hash_map;

//ȫ�������
const oprator_hash_map::value_type oprator_arr[] = {
	oprator_hash_map::value_type("(",	Operator("(",	kGroup,		kHpriority,NULL)),
	oprator_hash_map::value_type(")",	Operator(")",	kRgroup,	kHpriority,NULL)),
	oprator_hash_map::value_type("<",	Operator("<",	kLT,		kMpriority,OP_LT_FUN)),
	oprator_hash_map::value_type("<=",	Operator("<=",	kLE,		kMpriority,OP_LE_FUN)),
	oprator_hash_map::value_type(">",	Operator(">",	kGT,		kMpriority,OP_GT_FUN)),
	oprator_hash_map::value_type(">=",	Operator(">=",	kGE,		kMpriority,OP_GE_FUN)),
	oprator_hash_map::value_type("==",	Operator("==",	kEqual,		kMpriority,OP_EQUAL_FUN)),
	oprator_hash_map::value_type("!=",	Operator("!=",	kNotequal,	kMpriority,OP_NOEQUAL_FUN)),
	oprator_hash_map::value_type("&&",	Operator("&&",	kAnd,		kLpriority,OP_AND_FUN)),
	oprator_hash_map::value_type("||",	Operator("||",	kOr,		kLpriority,OP_OR_FUN)),
};
static oprator_hash_map kOperators(oprator_arr,oprator_arr+sizeof(oprator_arr)/sizeof(oprator_hash_map::value_type)); 





class Calculater
{
public:
	Calculater():group_layout_count_(0),
// 		var_string_state(false),
 		var_num_state(false),
		var_num_point_state(false),
 		var_outside_state(false),
		ptr_(NULL),
		curr_scan_len(0)
	{};


	int32_t reset()
	{
		group_layout_count_ = 0;
 		var_num_state = false;
		var_num_point_state = false;
 		var_outside_state = false;
		ptr_ = NULL;
		curr_scan_len = 0;
		//clear stack
		std::stack<Token *> operator_stack_empty;
		std::swap( operator_stack_, operator_stack_empty );
 		final_rpn_list_.clear();

		std::vector<Token *>::iterator iter = token_arr_.begin();
		for (;iter!=token_arr_.end();++iter)
		{
// 			Token *token_ptr = *iter;
// 			if (token_ptr!=NULL)
// 			{
// 				token_mem_pool_.free_ptr(*iter);
// 			}
			
			//
			if ((*iter)->is_outvar_ == false)
			{
				token_mem_pool_.free_ptr(*iter);
				//delete *iter;
				
			}

		}
		
		token_arr_.clear();
//		outside_variable_map_.clear();
		return 0;
	};

	


	

public:
    int32_t SetNum(const std::string &key,double number);
    int32_t SetString(std::string key, std::string val);
	int32_t Cal(const std::string &expression, std::string &msg); 

	/**
	* @brief   Cal
		ֱ�Ӵ�RNP����
	* @auth    arvincheng
	* @param std::string & msg
	* @return int32_t
	*/
	int32_t Cal(std::string &msg); 

	int32_t ParseToken(string &msg);
	int32_t ParseLex(string &msg);
private:

//	int32_t FindOp(const std::string &op, Token *&token, std::string &msg);
	//����Ƿ��Ǹ����������1�ǣ�0���ǣ�<0��������
	int32_t CheckOp(const std::string &op, oprator_hash_map::iterator &iter, std::string &msg);
	//���ҵ����������Ϊtoken
	int32_t SetTokenFromOp(oprator_hash_map::iterator &iter, Token *&token, std::string &msg);
	int32_t SetTokenFromOp(const Operator *oper, Token *&token, std::string &msg);

	//���ﷴ���ź�Ĵ���
	int32_t GroupProcess(Token &token);
	//�������ȼ����ߵ�
	int32_t OperatorPriorityProcess();
private:
	int32_t PrintFinalList();
	int32_t PrintTokenArr();


private:

	//�ʷ�����, ���к�����������������
	//1. ���ʶ�𲿷ַ���1��
	//2.ȫ��ʶ�𷵻�0��
	//3. û��ʶ�𷵻�3; 
	//4. ʶ��ɹ���, ptr_Ϊ��Ч�ַ���һ���ַ�,current_lenΪ0
	//�ʷ��������

	int32_t ParseLexOperator();
	int32_t ParseLexOperatorNew();
	int32_t ParseLexVarNum();
	int32_t ParseLexVarString();
	int32_t ParseLexOutside();
	//rnp�����������
	int32_t RnpOpratorProcess(Token *token);
	//rnp�����������
	int32_t RnpVarProcess(Token *token);

	int32_t BuildRNP();
private:
	//���Ų���
	int group_layout_count_;
	//�����ַ���״̬
// 	//��������״̬
 	bool var_num_state;
	//���ֱ�����.��״̬
	bool var_num_point_state;
	//�ⲿ����
	bool var_outside_state;

private:
	//��ǰָ��
	const char *ptr_;
	//��ǰɨ�賤��
	int32_t curr_scan_len;
private:
	//�ⲿ����ӳ��
	//std::map<string,Token>	outside_variable_map_;
	OutsideVarMap	outside_variable_map_;
	//���ջ
	std::stack<Token *>		operator_stack_;
	//��������
//	std::queue<Token>		variable_queue_;
	std::list<Token *>		final_rpn_list_;
	std::vector<Token *>		token_arr_;
private:
		CAllocatePool<Token> token_mem_pool_;
};

};
#endif


