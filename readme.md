# 场景
用于对请求进行规则匹配、活动资格匹配、限频条件匹配等需要进行条件表达式进行求值的场景

# 调用方式
```cpp
Calculater cal;
	cal.SetNum("varnum",12);
	cal.SetNum("varnuma",1);
	cal.SetString("varstring","a");
	cal.SetString("varstringb","c");
	int ret = cal.Cal("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")",msg);
 //ret<0:错误; ==0表达式结果为false; ==1表达式结果为true
```

# 性能
上述例子单核200000 qps, 多核性能线性增长(8核约160W qps)


# 优化过程
1. 初始60000 qps
1. 发现大量临时小对象分配
	1. 容器中TOKEN对象全部改为指针，所有TOKEN存放在一个vector中，其他地方指针指过来
	1. 计算开始和结束会分析TOKEN，有对象生成和销毁，改为对象池分配
	1. 改造后，提升到12W
1. 发现算符匹配耗时
	1. 算符匹配最开始是个map,改为hash_map,最终利用算符只可能是ascii以及最多是二元操作符，改为查表，用一个128*129的数组实现
	1. 改造后，提升到20W