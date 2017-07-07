// Test.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>

#include <windows.h>
#include <string>
// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
inline void TipBox(std::wstring str)
{
	MessageBoxW(NULL, str.c_str(), L"Tip", MB_OK);
}
//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif
//
//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//    return TRUE;
//}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
/*
 extern "C"ʹ����C++��ʹ��C���뷽ʽ��Ϊ���ܡ��ڡ�C++���¶��塰C����������Ҫ��extern ��C���ؼ��ʡ�
	 ��extern "C"��ָ���ú���ʹ��C���뷽ʽ������ġ�C���������Դӡ�C�����������
	ʹ��΢��ר�õ�_declspec (dllexport)  
	cpp�ļ��ڱ���ΪOBJ�ļ�ʱҪ�Ժ�����������������C���Ի�Ѻ���name��������Ϊ_name,��C++����������Ϊ_name@@decoration��
	extern "C"��ʾ��C���Եĸ�ʽ������������
	Ҫ����������࣬����ʹ��_declspec(_dllexpot)��Ҫ�����ĳ�Ա��������Ըú���ʹ��_declspec(_dllexport)*/

#define EXPORTFUN extern "C" __declspec(dllexport) 
#include <stdio.h>
#include <Windows.h>

HWND g_hWinProcess;
HHOOK g_hHook;
char g_szDllPath[MAX_PATH];

EXPORTFUN LRESULT MyMessageProcess(int Code, WPARAM wParam, LPARAM lParam)
{
	//
	//���Լ�����Ϣ�Ĵ���
	//
	return CallNextHookEx(g_hHook, Code, wParam, lParam);
}
EXPORTFUN void TestMessageBox()
{
	//
	//���Լ�����Ϣ�Ĵ���
	MessageBoxW(NULL,L"Test",L"Test",MB_OK); 
}


BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//
		//����������Ŀ����̿ռ�HOOK�Ĵ���
		//
	//	MessageBoxW(NULL, L"Inject Success!", L"Message", 0);
		OutputDebugString("Injectdd successfule DLL_PROCESS_ATTACH\n");
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString("Injectdd successfule DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString("Injectdd successfule DLL_THREAD_DETACH\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString("Injectdd successfule DLL_PROCESS_DETACH\n");
		break;
	}
	return TRUE;
}


//���ӻص�����
LRESULT CALLBACK HookProc(
	int code,       // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
	)
{
	//��DLLע��ĳɹ���ʱ�������ټ�����������Ҫע���DLL

	char szDllPath[MAX_PATH] = { 0 };
	BOOL bRet = 0;
		
	
	FILE* pFile = fopen("C:\\temp.temp", "r+");
	fread(szDllPath, 1, MAX_PATH, pFile);
	fclose(pFile);
	
	if (!LoadLibrary(szDllPath))
	{
		return 1;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);;
}



EXPORTFUN void SetHook(DWORD  dwPid, char * szDllPath)
{
	THREADENTRY32 te32 = { 0 };
	HANDLE hThreadSnap = NULL;
	DWORD  dwThreadId = 0;
	
	memcpy(g_szDllPath, szDllPath, MAX_PATH);

	//�����߳̿��ղ���Ŀ��������߳�
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}

	//������ѯĿ��������߳�ID
	TipBox(L"Thread32First");
	if (Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (dwPid == te32.th32OwnerProcessID)
			{
				dwThreadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}

	HANDLE hFile = CreateFile("C:\\temp.temp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile)
	{
		TipBox(L"������ʱ�ļ�ʧ�ܣ���");
	}
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, szDllPath, strlen(szDllPath) + 1, &dwBytesWritten, NULL);
	CloseHandle(hFile);

	if (dwThreadId != -1)
	{
		//��װ���̹���
		g_hHook = SetWindowsHookExA(WH_KEYBOARD, HookProc, GetModuleHandle("InjectDll.dll"), dwThreadId);


	}
}

EXPORTFUN void UnHook()
{
	UnhookWindowsHookEx(g_hHook);
}

