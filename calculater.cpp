#include "calculater.h"


namespace MSDK
{


int32_t Calculater::SetNum(const string &key,double number)
{
	Token token;
	token.kind = kVarNum;
	token.val_num = number;
	token.is_outvar_ = true;
	outside_variable_map_[key] = token;
	return 0;
}

int32_t Calculater::SetString(string key, string val)
{
	Token token;
	token.kind = kVarString;
	token.val_str = val;
	token.is_outvar_ = true;
	outside_variable_map_[key] = token;
	return 0;
}


int32_t Calculater::Cal(std::string &msg)
{
	
	//PrintFinalList();
	//PrintTokenArr();

	std::list<Token *>::iterator iter_var1,iter_var2,iter_op,iter_tmp;
	iter_op = final_rpn_list_.begin();
	while(true)
	{
		if (iter_op==final_rpn_list_.end())
		{
			return 0;
		}
		
		if (!((*iter_op)->kind>kOperatorBegin && (*iter_op)->kind<kOperatorEnd))
		{
			iter_op++;
			continue;
		}
		iter_tmp = iter_op;
		iter_var2 = --iter_tmp;
		iter_var1 = --iter_tmp;
		
		if ((*iter_op)->kind>kOperatorBegin && (*iter_op)->kind<kOperatorEnd)
		{
			int ret = 0;
			if ((*iter_op)->operator_info->op_fun==NULL)
			{
				return kOPFunIsNull;
			}
			ret = (*iter_op)->operator_info->op_fun(*iter_var1,*iter_var2);
			if (ret<0)
			{
				return ret;
			}
			(*iter_op)->kind = kNum;
			(*iter_op)->val_num = ret;
			final_rpn_list_.erase(iter_var1);
			final_rpn_list_.erase(iter_var2);
		}
		

	}
}


// int32_t Calculater::FindOp(const std::string &op, Token &token, std::string &msg)
// {
// 	//const char *ptr = op.c_str();
// 	oprator_hash_map::iterator iter = kOperators.find(op);
// 	if (iter==kOperators.end())
// 	{
// 		return kDontFindFail;
// 	}
// 	else
// 	{
// 		token.kind = iter->second.op_kind;
// 		token.literal = iter->second.literal;
// 		token.operator_info = iter->second;
// 		return 0;
// 	}
// 	
// }



int32_t Calculater::CheckOp(const std::string &op, oprator_hash_map::iterator &iter, std::string &msg)
{

	iter = kOperators.find(op);
	if (iter == kOperators.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


int32_t Calculater::SetTokenFromOp(oprator_hash_map::iterator &iter, Token *&token, std::string &msg)
{
	if (token == NULL)
	{
		token = token_mem_pool_.alloc_ptr();
	}
	token->kind = iter->second.op_kind;
	token->literal = iter->second.literal;
	token->operator_info = &(iter->second);
	return 0;
}

int32_t Calculater::SetTokenFromOp(const Operator *oper, Token *&token, std::string &msg)
{
	if (token == NULL)
	{
		token = token_mem_pool_.alloc_ptr();
	}
	token->kind = oper->op_kind;
	token->literal = oper->literal;
	token->operator_info = oper;
	return 0;
}

int32_t Calculater::PrintFinalList()
{
	std::list<Token *>::iterator iter = final_rpn_list_.begin();
	string str;
	int count = 0;
	for (;iter!=final_rpn_list_.end();++iter)
	{
		(*iter)->Str(str);
		cout<<"element "<<count<<": "<<str<<endl;
		count++;
	}
	cout<<endl;
	return 0;
}

int32_t Calculater::PrintTokenArr()
{
	std::vector<Token *>::iterator iter = token_arr_.begin();
	string str;
	int count = 0;
	cout<<"token arr:"<<endl;
	for (;iter!=token_arr_.end();++iter)
	{
		(*iter)->Str(str);
		cout<<"element "<<count<<": "<<str<<endl;
		count++;
	}
	cout<<endl;
	return 0;
}



int32_t Calculater::ParseLex(string &msg)
{
	if (ptr_ == NULL)
	{
		RET(kCalPtrIsNull,"ptr is NULL");
	}


	int ret;
	Token *token = NULL;
	while(*ptr_!='\0')
	{
		//printf("buff:%s\n",ptr_);
		//token->Reset();
		//把空白符忽略掉
		if (*ptr_=='\t' || *ptr_==' ')
		{
			ptr_++;
			//curr_scan_len = 0;
			continue;
		}

		curr_scan_len++;
		

		token = NULL;
		ret = ParseLexVarNum();

		if (ret == 0)
		{
			//ptr_+=curr_scan_len;
			//curr_scan_len = 0;
			continue;
		}
		else if (ret==1)
		{
			curr_scan_len++;
			continue;
		}

		token = NULL;
		ret = ParseLexVarString();

		if (ret == 0)
		{
			//ptr_+=curr_scan_len;
			//curr_scan_len = 0;
			continue;
		}
		else if (ret==1)
		{
			curr_scan_len++;
			continue;
		}

		token = NULL;
		ret = ParseLexOutside();

		if (ret == 0)
		{
			//ptr_+=curr_scan_len;
			//curr_scan_len = 0;
			continue;
		}
		else if (ret==1)
		{
			curr_scan_len++;
			continue;
		}
		
		//算符,ret==0,找到,==1，进入状态,<0错误

		token = NULL;
		ret = ParseLexOperator();

		if (ret == 0)
		{
			//ptr_+=curr_scan_len;
			//curr_scan_len = 0;
			continue;
		}
		else if (ret==1)
		{
			curr_scan_len++;
			continue;
		}

		//都没有命中
		string failExpStr = string(ptr_);
		msg = "^"+failExpStr+"; has syntax fail.";
		curr_scan_len = 0;
		return kSyntaxFail;
	}
	return 0;
}

int32_t Calculater::ParseLexOperator()
{
//	int ret;
	string msg;


	const char *curr_end_ptr = ptr_;
	Token *token = NULL;

	oprator_hash_map::iterator iter,iter_tmp;
	uint8_t charc_l = *ptr_;
	uint8_t charc_c = 0;
	if (charc_l > 128)
	{
		//不可能是算符了
		return 3;
	}
	
	//一元算符
	const Operator *op_ptr = oprator_arr_new_s[charc_l][128];
	const Operator *op_ptr_second = NULL;
	//二元算符
	if (*(curr_end_ptr+1)!='\0')
	{
		charc_c = *(curr_end_ptr+1);
		if (charc_c<=128)
		{
			op_ptr_second = oprator_arr_new_s[charc_l][charc_c];

		}
	}

	//优先适配二元算符
	if (op_ptr_second!=NULL)
	{
		string token_str_tmp = string(ptr_,(curr_end_ptr + 2 - ptr_));
		ptr_ = curr_end_ptr + 2;
		curr_scan_len = 0;
		SetTokenFromOp(op_ptr_second, token, msg);
		token_arr_.push_back(token);
		return 0;
	}
	if (op_ptr!=NULL)
	{

		string token_str_tmp = string(ptr_,(curr_end_ptr + 1 - ptr_));
		ptr_ = curr_end_ptr + 1;
		curr_scan_len = 0;
		SetTokenFromOp(op_ptr, token, msg);
		token_arr_.push_back(token);
		return 0;
	}
	
	return 3;
}

int32_t Calculater::ParseLexOperatorNew()
{
	//	int ret;
	string msg;


	const char *curr_end_ptr = ptr_;
	Token *token = NULL;
	const Operator *op_ptr = NULL;
	oprator_hash_map::iterator iter,iter_tmp;
	uint8_t charc_l = *ptr_;
	uint8_t charc_c = 0;
	if (charc_l > 128)
	{
		//不可能是算符了
		return 3;
	}
	//cout<<"str:"<<string(ptr_,1)<<endl;
	iter = kOperators.find(string(ptr_,1));
	if (iter!=kOperators.end())
	{
		op_ptr = &(iter->second);
	}
	
	const Operator *op_ptr_second = NULL;
	//二元算符
	if (*(curr_end_ptr+1)!='\0')
	{
		charc_c = *(curr_end_ptr+1);
		if (charc_c<=128)
		{
			//op_ptr_second = oprator_arr_new_s[charc_l][charc_c];
			//cout<<"str:"<<string(ptr_,2)<<endl;
			iter = kOperators.find(string(curr_end_ptr,2));
			if (iter!=kOperators.end())
			{
				op_ptr_second = &(iter->second);
			}
		}
	}

	//优先适配二元算符
	if (op_ptr_second!=NULL)
	{
		string token_str_tmp = string(ptr_,(curr_end_ptr + 2 - ptr_));
		ptr_ = curr_end_ptr + 2;
		curr_scan_len = 0;
		SetTokenFromOp(op_ptr_second, token, msg);
		token_arr_.push_back(token);
		return 0;
	}
	if (op_ptr!=NULL)
	{

		string token_str_tmp = string(ptr_,(curr_end_ptr + 1 - ptr_));
		ptr_ = curr_end_ptr + 1;
		curr_scan_len = 0;
		SetTokenFromOp(op_ptr, token, msg);
		token_arr_.push_back(token);
		return 0;
	}

	return 3;
}

int32_t Calculater::ParseLexVarNum()
{
	Token *token = NULL;
	bool num_state = false;
	const char *curr_end_ptr = ptr_;
	while(*curr_end_ptr >= '0' && *curr_end_ptr <='9')
	{
		
		if (!num_state)
		{
			num_state = true;
		}
		
		curr_end_ptr++;
	}
	
	if (num_state)
	{
		string num_str = string(ptr_,curr_end_ptr-ptr_);
		if (token == NULL)
		{
			token = token_mem_pool_.alloc_ptr();
		}
		token->kind = kNum;
		token->val_num = atoi(num_str.c_str());

		var_num_state = false;
		var_num_point_state = false;
		ptr_ = curr_end_ptr;
		curr_scan_len = 0;
		token_arr_.push_back(token);
		return 0;
	}
	
	return 3;
}

int32_t Calculater::ParseLexVarString()
{
	Token *token = NULL;
	bool var_string_state = false;
	const char *curr_end_ptr = ptr_;
	//没有字符串开始符，直接跳过
	if (*curr_end_ptr!='\"' && !var_string_state)
	{
		return 3;
	}
	while (curr_end_ptr!='\0')
	{
		
		
		if (*curr_end_ptr=='\"' && !var_string_state)
		{
			var_string_state = true;
			curr_end_ptr++;
			continue;
		}

		if (*curr_end_ptr=='\"' && var_string_state)
		{
			//多了一个"符号，所以是curr_end_ptr - ptr_
			int word_len = curr_end_ptr - ptr_;
			if (token == NULL)
			{
				token = token_mem_pool_.alloc_ptr();
			}
			//不要第一个"符号
			token->val_str = string(ptr_+1,word_len-1);
			token->kind = kString;
			var_string_state = false;
			//多一个“符号，跳到下个符号
			ptr_ = curr_end_ptr+1;
			curr_scan_len = 0;
			token_arr_.push_back(token);
			return 0;
		}
		curr_end_ptr++;
	}

	//如果var_string_state为true, 循环结束，说明到结束都没有找到结束"符号
	if (var_string_state)
	{
		var_string_state = false;
		return kVarQuotationDontMatch;
	}
	
	return 3;
}

int32_t Calculater::ParseLexOutside()
{
	Token *token = NULL;
	const char *curr_end_ptr = ptr_;
	if (!((*curr_end_ptr >= 'a' && *curr_end_ptr <='z') || (*curr_end_ptr >= 'A' && *curr_end_ptr <='Z')))
	{
		return 3;
	}
	while (((*curr_end_ptr >= 'a' && *curr_end_ptr <='z') || (*curr_end_ptr >= 'A' && *curr_end_ptr <='Z')) ||
		    *curr_end_ptr=='_' ||
			(*curr_end_ptr >= '0' && *curr_end_ptr <='9'))
	{
		curr_end_ptr++;
	}
	
	if ((curr_end_ptr - ptr_) > 0)
	{
		string var_key = string(ptr_,(curr_end_ptr - ptr_));
		OutsideVarMap::iterator iter = outside_variable_map_.find(var_key);
		if (iter!=outside_variable_map_.end())
		{
			token = &(iter->second);
			var_outside_state = false;
			ptr_ = curr_end_ptr;
			curr_scan_len = 0;
			token_arr_.push_back(token);
			return 0;
		}
		else
		{
			return kVarDontExist;
		}
	}
	return 3;
}

int32_t Calculater::Cal(const std::string &expression, std::string &msg)
{
	reset();
	ptr_ = expression.c_str();
	int ret = ParseLex(msg);
	if (ret<0)
	{ 
		return ret;
	}
	
//	PrintTokenArr();
	ret = BuildRNP();
	if (ret<0)
	{
		return ret;
	}
//	PrintFinalList();

	//对最终的RNP进行求值
	ret = Cal(msg);
	if (ret<0)
	{
		stringstream streamt;
		streamt<<"computer RNP fail. ret:"<<ret<<endl;
		msg = streamt.str();
		return ret;
	}
	if (final_rpn_list_.empty() || final_rpn_list_.size()>1)
	{
		return kFinalListSizeFail;
	}

	std::list<Token *>::iterator iter = final_rpn_list_.begin();
	return (int)(*iter)->val_num;
}

int32_t Calculater::BuildRNP()
{
//	size_t arr_len = token_arr_.size();
	vector<Token *>::iterator iter;
	int ret = 0;
	Token *token_tmp = NULL;
	for (iter=token_arr_.begin();iter!=token_arr_.end();++iter)
	{
//		ptr = &(*iter);
		if ((*iter)->kind>kOperatorBegin && (*iter)->kind<kOperatorEnd)
		{
			ret = RnpOpratorProcess(*iter);
			if (ret<0)
			{
				return ret;
			}
		}
		else if ((*iter)->kind>kVarBegin && (*iter)->kind<kVarEnd)
		{
			ret = RnpVarProcess(*iter);
			if (ret<0)
			{
				return ret;
			}
			
		}
	}

	while (true)
	{
		if(operator_stack_.empty())
		{
			break;
		}
		token_tmp = operator_stack_.top();
		if (token_tmp->kind == kGroup || token_tmp->kind == kRgroup)
		{
			return kGroupDontMatch;
		}
		
		final_rpn_list_.push_back(token_tmp);
		operator_stack_.pop();
	}

	return 0;
}

int32_t Calculater::RnpOpratorProcess(Token *token)
{
	
	if (token->kind==kGroup)
	{
		operator_stack_.push(token);
	}
	else if (token->kind==kRgroup)
	{
		
		//弹出,直到遇到(
		while (true)
		{
			Token *token_tmp = operator_stack_.top();
			if (token_tmp->kind == kGroup)
			{
				operator_stack_.pop();
				break;
			}
			final_rpn_list_.push_back(token_tmp);
			operator_stack_.pop();
		}
	}
	else
	{
		if (operator_stack_.empty())
		{
			operator_stack_.push(token);
			return 0;
		}
		
		//算符优先级处理
		Token *token_tmp = operator_stack_.top();
		if (token_tmp->operator_info->priority >  token->operator_info->priority && token_tmp->kind!=kGroup)
		{
			final_rpn_list_.push_back(token_tmp);
			operator_stack_.pop();
			operator_stack_.push(token);
		}
		else
		{
			operator_stack_.push(token);
		}
	}
	return 0;
}

int32_t Calculater::RnpVarProcess(Token *token)
{
	final_rpn_list_.push_back(token);
// 	if (operator_priority_state)
// 	{
// 		operator_priority_state = false;
// 		//算符优先级处理
// 		int ret = OperatorPriorityProcess();
// 		if (ret<0)
// 		{
// 			string failExpStr = string(ptr_);
// //			msg = "^"+failExpStr+"; has syntax fail.";
// 			curr_scan_len = 1;
// 			return ret;
// 		}
// 	}
	return 0;
}


//小于
int32_t OP_LT_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str<tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num < tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//小于等于
int32_t OP_LE_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str<=tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num <= tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//大于
int32_t OP_GT_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str>tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num > tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//大于等于
int32_t OP_GE_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str>=tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num >= tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//等于
int32_t OP_EQUAL_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str == tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num == tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//不等于
int32_t OP_NOEQUAL_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return tokenL->val_str != tokenR->val_str?1:0;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num != tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//与
int32_t OP_AND_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return kOPNeedVarKindDontMatch;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num && tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};

//或
int32_t OP_OR_FUN(Token *tokenL, Token *tokenR)
{
	if ((tokenL->kind==kString || tokenL->kind==kVarString) && 
		(tokenR->kind==kString|| tokenR->kind==kVarString) )
	{
		return kOPNeedVarKindDontMatch;
	}
	else if ((tokenL->kind==kNum || tokenL->kind==kVarNum) && 
		(tokenR->kind==kNum || tokenR->kind==kVarNum))
	{
		return tokenL->val_num || tokenR->val_num?1:0;
	}
	else
		return kLRVarKindDontMatch;
};
};
