#include "MemProtect.h"
#include <stdio.h>
int n = 10212;
int m = 3434;
MemProtect::MemProtect()
{
}

MemProtect::~MemProtect()
{
}

int MemProtect::GetValue()
{
	return (n^ m_iEncodeValue) - m;
}

void MemProtect::SetValue(int a)
{
	m_iEncodeValue = n ^ a + m;
}

void MemProtect::Test()
{
	SetValue(3233);
	printf("origin : %d, cur: %d, getCur:%d \n", 3233, m_iEncodeValue, GetValue());
	GetValue();
}
