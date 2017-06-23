#include "TMemBase.h"
#include "Framework.h"
static int iValueToBeModified = 1023;

//%x����ʮ�����������Ӣ����ĸСд���Ҷ��롣
//%02X�����±仯��Ӣ����ĸ���д���������ַ�������λ�ģ������λ��ȣ��Ҷ��룬�յ�һλ��0��������λ�ģ�ȫ�������
static char * staticA = nullptr;
char* ExternA = nullptr;
void TMemBase::Test()
{
	HMODULE hModule = GetModuleHandle(NULL);
	int stackA = 0;
	printf("------------%x\n",(size_t)hModule);
	printf("---staticA = %p, offset = %x \n",&staticA,(size_t)&staticA - (size_t)hModule);
	printf("---ExternA = %p, offset = %x\n",&ExternA,(size_t)&ExternA - (size_t)hModule);
	printf("---stackA = %p,offset = %x\n",&stackA,(size_t)&stackA - (size_t)hModule);
	//printf("-----offset-------%x\n",(size_t)&stackA - (size_t)hModule);

	//search a process's memory for a specified value.
	//Memory Scanner
	printf("Value to be modified value:%d , addr: %p\n", iValueToBeModified++, &iValueToBeModified);
#if 0
	printf("Test mem base: %p\n", &pBuff);
	DWORD a = (DWORD)&pBuff;
	DWORD b = (DWORD)this;
	//assert(a == b);
	printf("The offset of private member:%d\n", (DWORD)&pBuff - (DWORD)this);
#endif
}

