#pragma once
#include<math.h>
#include<random>

class Math {
	
public:
	//��ȡ�������
	static int RandomInt(int lower,int upper);
	//��ȡ���ʵ��
	static double RandomReal(double lower, double upper);
	//��ȡ���0-1����
	static double RandomPerc();
	//��ĳ��������ĳ����Χ��
	static double Clamp(double value, double lower, double upper);
};