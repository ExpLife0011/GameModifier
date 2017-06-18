// HookInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HookInject.h"


typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid, char * szDllPath);
HINSTANCE g_hDll;
LPFUN2 g_pfnSetHook = NULL;
LPFUN g_pfnUnHook = NULL;

void HookInject::StartInject(std::wstring stExe, std::string strDll)
{
	m_strDllPath = strDll;
	m_dwPid = GetCurrentProcessId();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	g_hDll = LoadLibrary(L"InjectDll.dll");

	if (g_hDll != NULL)
	{
		g_pfnSetHook = (LPFUN2)GetProcAddress(g_hDll, "SetHook");
		g_pfnUnHook = (LPFUN)GetProcAddress(g_hDll, "UnHook");
	}
	else
	{
		TipBox(L"����DLLʧ�ܣ�");
		return;
	}


	//��װ���Ӻ���
	if (g_pfnSetHook != NULL)
	{
		g_pfnSetHook(m_dwPid, (char *)m_strDllPath.c_str());
	}
	else
	{
		TipBox(L"��װ����ʧ�ܣ�");
		return;
	}
}


void HookInject::OnBnClickedUnhook()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (g_hDll != NULL)
	{
		//ж�ع��Ӻ���
		g_pfnUnHook();
		//Ĩ��DLL
		FreeLibrary(g_hDll);
	}
}
