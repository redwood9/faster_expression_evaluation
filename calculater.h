/**
* @auth   arvincheng
* @brief   规则表达式计算
	使用方式形如
	MSDK::Calculater cal;
	cal.SetNum("varnum",12);
	cal.SetNum("varnuma",1);
	cal.SetString("varstring","a");
	cal.SetString("varstringb","c");
	int ret = cal.Cal("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")",msg);

	ret<0:错误; ==0表达式结果为false; ==1表达式结果为true

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
//ptr指向异常
const int kCalPtrIsNull						= kCalBase - 1;
//没有找到
const int kDontFindFail						= kCalBase - 2;
//数字中太多点
const int kNumTooMuchPoint					= kCalBase - 3;
//数字中点后面没有接数字
const int kNumAfterPointDontFindNum			= kCalBase - 4;
//外部变量定义太多点
const int kOutsideVarTooMuchPoint			= kCalBase - 5;
//外部变量点后面没有接字母
const int kOutsideVarAfterPointDontFindChar	= kCalBase - 6;
//语法错
const int kSyntaxFail						= kCalBase - 7;
//括号不匹配
const int kGroupDontMatch					= kCalBase - 8;
//操作数只有一个
const int kOperatorDontMatch				= kCalBase - 9;
//缺少算符
const int kDontFindOprator					= kCalBase - 10;
//左值和右值类型不匹配
const int kLRVarKindDontMatch				= kCalBase - 11;
//算符要求的变量类型不符
const int kOPNeedVarKindDontMatch			= kCalBase - 12;
//算符对应的函数是NULL
const int kOPFunIsNull						= kCalBase - 14;
//最终的计算结果LIST节点数字有误,正常情况只能有一个
const int kFinalListSizeFail				= kCalBase - 15;
//变量必须是字母开头
const int kVarMustBeChar					= kCalBase - 16;
//字符串"不匹配
const int kVarQuotationDontMatch			= kCalBase - 17;
//变量不存在	
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

//全局算符表
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
		直接从RNP计算
	* @auth    arvincheng
	* @param std::string & msg
	* @return int32_t
	*/
	int32_t Cal(std::string &msg); 

	int32_t ParseToken(string &msg);
	int32_t ParseLex(string &msg);
private:

//	int32_t FindOp(const std::string &op, Token *&token, std::string &msg);
	//检查是否是个算符，返回1是，0不是，<0发生错误
	int32_t CheckOp(const std::string &op, oprator_hash_map::iterator &iter, std::string &msg);
	//把找到的算符设置为token
	int32_t SetTokenFromOp(oprator_hash_map::iterator &iter, Token *&token, std::string &msg);
	int32_t SetTokenFromOp(const Operator *oper, Token *&token, std::string &msg);

	//到达反括号后的处理
	int32_t GroupProcess(Token &token);
	//碰到优先级更高的
	int32_t OperatorPriorityProcess();
private:
	int32_t PrintFinalList();
	int32_t PrintTokenArr();


private:

	//词法分析, 所有函数满足以下条件：
	//1. 如果识别部分返回1；
	//2.全部识别返回0；
	//3. 没有识别返回3; 
	//4. 识别成功后, ptr_为有效字符下一个字符,current_len为0
	//词法分析算符

	int32_t ParseLexOperator();
	int32_t ParseLexOperatorNew();
	int32_t ParseLexVarNum();
	int32_t ParseLexVarString();
	int32_t ParseLexOutside();
	//rnp构造算符处理
	int32_t RnpOpratorProcess(Token *token);
	//rnp构造变量处理
	int32_t RnpVarProcess(Token *token);

	int32_t BuildRNP();
private:
	//括号层数
	int group_layout_count_;
	//变量字符串状态
// 	//变量数字状态
 	bool var_num_state;
	//数字变量中.的状态
	bool var_num_point_state;
	//外部变量
	bool var_outside_state;

private:
	//当前指针
	const char *ptr_;
	//当前扫描长度
	int32_t curr_scan_len;
private:
	//外部变量映射
	//std::map<string,Token>	outside_variable_map_;
	OutsideVarMap	outside_variable_map_;
	//算符栈
	std::stack<Token *>		operator_stack_;
	//变量队列
//	std::queue<Token>		variable_queue_;
	std::list<Token *>		final_rpn_list_;
	std::vector<Token *>		token_arr_;
private:
		CAllocatePool<Token> token_mem_pool_;
};

};
#endif


