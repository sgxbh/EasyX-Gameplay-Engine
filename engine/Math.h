#pragma once
#include<math.h>
#include<random>

class Math {
	
public:
	//获取随机整数
	static int RandomInt(int lower,int upper);
	//获取随机实数
	static double RandomReal(double lower, double upper);
	//获取随机0-1的数
	static double RandomPerc();
	//将某数限制在某个范围内
	static double Clamp(double value, double lower, double upper);
};