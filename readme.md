# Scenario
Used for rule matching, activity eligibility matching, rate-limiting condition matching, and other scenarios that require conditional expressions for evaluation.

# Invocation Method
```cpp
Calculater cal;
	cal.SetNum("varnum",12);
	cal.SetNum("varnuma",1);
	cal.SetString("varstring","a");
	cal.SetString("varstringb","c");
	int ret = cal.Cal("(varnum==12 || 1==2) && (varstring>=\"b\" || varstringb<\"d\")",msg);
 //ret<0:错误; ==0表达式结果为false; ==1表达式结果为true
```

# Features
1. Ultra-high performance, the example above achieves 200,000 QPS on a single core, with linear performance scaling on multiple cores (around 160W QPS on 8 cores). Actual measurements show a performance increase of more than 10 times compared to LUA.
2. Handwritten lexical analysis and syntax parsing.
3. Object pool optimization, operator lookup staticization, improving performance.

# Optimization Process
1. Initial 60,000 QPS
2. Found a large number of temporary small object allocations:
   1. All TOKEN objects in the container were changed to pointers, with all TOKENS stored in a vector, and pointers referencing them elsewhere.
   2. The calculation start and end would analyze TOKENs, resulting in object creation and destruction. This was changed to object pool allocation.
   3. After the modification, performance improved to 120,000 QPS.
3. Found that operator matching took too long:
   1. Initially, operator matching was a map, which was changed to a hash_map. Since operators are either ASCII characters or at most binary operators, it was optimized into a lookup table using a 128*129 array.
   2. After the modification, performance improved to 200,000 QPS.




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

# 特性
1. 超高性能，上述例子单核200000 qps, 多核性能线性增长(8核约160W qps)。实测超过LUA 10倍
2. 手写词法解析、语法解析
3. 对象池优化、算符查找静态化，加强性能。


# 优化过程
1. 初始60000 qps
1. 发现大量临时小对象分配
	1. 容器中TOKEN对象全部改为指针，所有TOKEN存放在一个vector中，其他地方指针指过来
	1. 计算开始和结束会分析TOKEN，有对象生成和销毁，改为对象池分配
	1. 改造后，提升到12W
1. 发现算符匹配耗时
	1. 算符匹配最开始是个map,改为hash_map,最终利用算符只可能是ascii以及最多是二元操作符，改为查表，用一个128*129的数组实现

	1. 改造后，提升到20W
