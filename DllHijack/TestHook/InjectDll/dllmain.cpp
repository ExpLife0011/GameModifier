// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"

#define EXPORTFUN extern "C" __declspec(dllexport) 

HWND g_hWinProcess;
HHOOK g_hHook;
char g_szDllPath[MAX_PATH];
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_PROCESS_ATTACH", L"Tip", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_THREAD_ATTACH", L"Tip", MB_OK);
		break;
	case DLL_THREAD_DETACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_THREAD_DETACH", L"Tip", MB_OK);
		break;
	case DLL_PROCESS_DETACH:
		MessageBoxW(NULL, L"Injectdd successfule DLL_PROCESS_DETACH", L"Tip", MB_OK);
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

