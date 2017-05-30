// stdafx.cpp : source file that includes just the standard includes
// TestHook.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
void TipBox(std::wstring str)
{
	MessageBox(NULL, str.c_str(), L"Tip", MB_OK);
}

void TipBox(std::string str)
{
	MessageBoxA(NULL, str.c_str(), "Tip", MB_OK);
}

//���ַ���ת���ɿ��ַ���
std::wstring string2Wstring(std::string sToMatch)
{
	int iWLen = MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), 0, 0); // ����ת������ַ����ĳ��ȡ����������ַ�����������
	wchar_t *lpwsz = new wchar_t[iWLen + 1];
	MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), lpwsz, iWLen); // ��ʽת����
	lpwsz[iWLen] = L'/0';
	std::wstring wsToMatch(lpwsz);
	delete[]lpwsz;
	return wsToMatch;
}
//�ѿ��ַ���ת�����ַ��������ʹ��
std::string wstring2string(std::wstring sToMatch)
{
	std::string sResult;
	int iLen = WideCharToMultiByte(CP_ACP, NULL, sToMatch.c_str(), -1, NULL, 0, NULL, FALSE); // ����ת�����ַ����ĳ��ȡ��������ַ�����������
	char *lpsz = new char[iLen];
	WideCharToMultiByte(CP_OEMCP, NULL, sToMatch.c_str(), -1, lpsz, iLen, NULL, FALSE); // ��ʽת����
	sResult.assign(lpsz, iLen - 1); // ��string������и�ֵ��
	delete[]lpsz;
	return sResult;
}