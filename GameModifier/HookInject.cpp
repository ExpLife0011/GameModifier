// HookInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HookInject.h"

// hookExample.cpp
#include "stdafx.h"
#include <strsafe.h>// for using StringCchCopy,StringCchLength functions etc.

// MessageHook.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <Tlhelp32.h>

BOOL SetWinHookInject(WCHAR * wzDllPath, WCHAR * wzProcessName);
UINT32 GetTargetThreadIdFromProcessName(WCHAR *ProcessName);


typedef void(*LPFUN)();
typedef void(*LPFUN2)(DWORD dwPid, char * szDllPath);
HINSTANCE g_hDll;
LPFUN2 g_pfnSetHook = NULL;
LPFUN g_pfnUnHook = NULL;

void HookInject::StartInject(std::wstring stExe, std::wstring strDll)
{

	m_dwPid = GetCurrentProcessId();
	TCHAR szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, szPath);
	m_strDllPath = szPath + std::wstring(L"\\") + strDll;

	if (!SetWinHookInject((WCHAR*)m_strDllPath.c_str(), (WCHAR*)stExe.c_str()))
	{
		OutputDebugString(L"Set Hook Unsuccess!\r\n");
		return ;
	}
	OutputDebugString(L"Inject Success!\r\n");


	//// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//g_hDll = LoadLibrary(L"InjectDll.dll");

	//if (g_hDll != NULL)
	//{
	//	g_pfnSetHook = (LPFUN2)GetProcAddress(g_hDll, "SetHook");
	//	g_pfnUnHook = (LPFUN)GetProcAddress(g_hDll, "UnHook");
	//}
	//else
	//{
	//	TipBox(L"����DLLʧ�ܣ�");
	//	return;
	//}


	////��װ���Ӻ���
	//if (g_pfnSetHook != NULL)
	//{
	//	g_pfnSetHook(m_dwPid, (char *)m_strDllPath.c_str());
	//}
	//else
	//{
	//	TipBox(L"��װ����ʧ�ܣ�");
	//	return;
	//}
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




//
//����Windows API SetWindowsHookExʵ��ע��DLL
//
BOOL SetWinHookInject(WCHAR * wzDllPath, WCHAR * wzProcessName)
{
	HMODULE ModuleHandle = NULL;
	BOOL    bOk = FALSE;
	DWORD   FunctionAddress = NULL;
	UINT32  dwThreadId = 0;
	HHOOK   g_hHook = NULL;
	PVOID   pShareM = NULL;

	OutputDebugString(L"[+] SetWinHKInject Enter!\n");


	ModuleHandle = LoadLibrary(wzDllPath);
	if (!ModuleHandle)
	{
		OutputDebugString(L"[+] LoadLibrary error!\n");
		goto Exit;
	}


	FunctionAddress = (DWORD)GetProcAddress(ModuleHandle, "MyMessageProc");
	if (!FunctionAddress)
	{
		wprintf(L"[+] GetProcAddress error!\n");
		goto Exit;
	}


	dwThreadId = GetTargetThreadIdFromProcessName(wzProcessName);
	if (!dwThreadId)
		goto Exit;

	//����Ϣ����
	g_hHook = SetWindowsHookEx(
		WH_GETMESSAGE,//WH_KEYBOARD,//WH_CALLWNDPROC,
		(HOOKPROC)FunctionAddress,
		ModuleHandle,
		dwThreadId
		);

	if (!g_hHook)
	{
		OutputDebugString(L"[-] SetWindowsHookEx error !\n");
		goto Exit;
	}

	OutputDebugString(L"[!] SetWinHKInject Exit!\n");
	bOk = TRUE;
Exit:
	if (ModuleHandle)
		FreeLibrary(ModuleHandle);
	return bOk;

}

//ͨ������������߳�ID
UINT32 GetTargetThreadIdFromProcessName(WCHAR *ProcessName)
{
	PROCESSENTRY32 pe;
	HANDLE SnapshotHandle = NULL;
	HANDLE ProcessHandle = NULL;
	BOOL Return, ProcessFound = FALSE;
	UINT32 pTID, ThreadID;

	SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (SnapshotHandle == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, L"Error: unable to create toolhelp snapshot", L"Loader", NULL);
		return FALSE;
	}

	pe.dwSize = sizeof(PROCESSENTRY32);

	Return = Process32First(SnapshotHandle, &pe);

	while (Return)
	{
		if (_wcsicmp(pe.szExeFile, ProcessName) == 0)
		{
			ProcessFound = TRUE;
			break;
		}

		Return = Process32Next(SnapshotHandle, &pe);
		pe.dwSize = sizeof(PROCESSENTRY32);

	}

	CloseHandle(SnapshotHandle);
	//ͨ��fs�Ĵ�����ȡTID
	_asm
	{
		mov eax, fs:[0x18]
			add eax, 36
			mov[pTID], eax
	}

	ProcessHandle = OpenProcess(PROCESS_VM_READ, FALSE, pe.th32ProcessID);
	ReadProcessMemory(ProcessHandle, (LPCVOID)pTID, &ThreadID, 4, NULL);
	CloseHandle(ProcessHandle);

	return ThreadID;
}