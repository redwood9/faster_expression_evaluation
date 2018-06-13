#include "calculater.h"

#include <iostream>
using namespace std;

//1:每个案例初始化一个对象; 2:所有案例都是一个案例
const int DEBUG_MODE = 2;

MSDK::Calculater kCal;



int compute(const string &cal_str)
{

	string msg;
	int ret;
	if (DEBUG_MODE==1)
	{
		MSDK::Calculater cal;
		cal.SetNum("varnum",12);
		cal.SetNum("varnuma",1);
		cal.SetString("varstring","a");
		cal.SetString("varstringb","c");
		ret = cal.Cal(cal_str,msg);
	}
	else
	{
		ret = kCal.Cal(cal_str,msg);
	}

	
	cout<<"cal str:"<<cal_str<<". ret:"<<ret<<". msg:"<<msg<<endl;
	return 0;
}

int performance()
{
	int start_time = time(NULL);
	cout<<start_time<<endl;
	int i = 0;

 	string msg;
	MSDK::Calculater cal;
	int ret = 0;
	cal.SetNum("varnum",12);
	cal.SetNum("varnuma",1);
	cal.SetString("varstring","a");
	cal.SetString("varstringb","c");

	for (;i<6000000;i++)
	{
		//cal.Cal("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")",msg);
		cal.Cal("(1==12 || 3==2) && (\"b\">=\"b\" || \"abc\"<\"d\")",msg);
		//cout<<"ret:"<<ret<<". msg:"<<msg<<endl;
		//compute("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")");
		//compute("(12==12 || 1==2) && (\"c\">=\"b\" || varstringb<\"d\")");
	}
	int end_time = time(NULL);
	cout<<"cost time:"<<(end_time-start_time)<<". qps:"<<i/(end_time-start_time)<<endl;
	return 0;
}

int main(int argc, char* argv[])
{
	string msg;
	
	//cout<<(1||0)<<endl;
	
	
	kCal.SetNum("varnum",12);
	kCal.SetNum("varnuma",1);
	kCal.SetString("varstring","a");
	kCal.SetString("varstringb","c");


	string cal_str = "varnum==12";
	//string cal_str = "(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")";
 	compute("varnum==12");
 	compute("varnum==13");
 	compute("varnum>0");
 	compute("varnum<16");
 	compute("varnum <\"16\"");

 	compute("varnum==13 || 1==2");
	compute("varnum==12 || 1==2");
	compute("varnum==13 || 1==1");


  	compute("varnum==13 && 1==2");
  	compute("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")");
	compute("(varnum==15 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")");
	compute("(1==12 || 3==2) && (\"b\">=\"b\" || \"abc\"<\"d\")");
	compute("((varnum==15 || 1==2) && 1==1) && (varstring>=\"b\" || varstringb<\"d\")");
	compute("(varnum==15 || 1==2) && ((varstring>=\"b\" || varstringb<\"d\")");


	performance();
	return 0;
	//cout<<"return:"<<ret<<".msg:"<<msg<<endl;
}
