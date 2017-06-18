// BlookHook.cpp : ʵ���ļ�

#include "stdafx.h"
#include "BlookHook.h"


// BlookHook ��Ϣ�������
typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid);
HINSTANCE g_hBlookDll;
LPFUN2 g_pfnBlookSetHook = NULL;
LPFUN g_pfnBlookUnHook = NULL;

void BlookHook::OnBlockHook()
{
	m_dwPID = GetCurrentProcessId();
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	g_hBlookDll = LoadLibrary(L"BlockHook.dll");

	if (g_hBlookDll != NULL)
	{
		g_pfnBlookSetHook = (LPFUN2)GetProcAddress(g_hBlookDll, "SetHook");
		g_pfnBlookUnHook = (LPFUN)GetProcAddress(g_hBlookDll, "UnHook");
	}
	else
	{
		TipBox(L"����DLLʧ�ܣ�");
		return;
	}


	//��װ���Ӻ���
	if (g_pfnBlookSetHook != NULL)
	{
		g_pfnBlookSetHook(m_dwPID);
	}
	else
	{
		TipBox(L"��װ����ʧ�ܣ�");
		return;
	}

}
