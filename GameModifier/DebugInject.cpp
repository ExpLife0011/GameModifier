// DebugInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DebugInject.h"
#include <assert.h>
//�ṹ�����ֽڶ���!  
#pragma pack(1)  
typedef struct _INJECT_CODE
{
	BYTE  byMOV_EAX;          //mov eax, addr szDllpath
	DWORD dwMOV_EAX_VALUE;
	BYTE  byPUSH_EAX;         //push eax
	BYTE  byMOV_ECX;          //mov ecx, LoadLibrary
	DWORD dwMOV_ECX_VALUE;
	WORD  wCALL_ECX;          //call ecx
	BYTE  byINT3;             //int 3
	CHAR  szDllPath[MAX_PATH];
}INJECT_CODE, *PINJECT_CODE;
#pragma pack()
// DebugInject �Ի���


void DebugInject::StartInject(std::wstring stExe, std::wstring strDll)
{
	 m_strExePath = stExe;
	TCHAR szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, szPath);
	m_strDllPath = szPath + std::wstring(L"\\") + strDll;
	assert(_waccess(m_strDllPath.c_str(), 0) == 0);
	int idllPathSize = m_strDllPath.size() * 2;

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet;
	DWORD dwProcessId = 0;
	LPVOID lpBaseAddress = NULL;
	HANDLE hThread = NULL;
	HANDLE hProcess = NULL;
	DEBUG_EVENT dbgEvent = { 0 };
	CONTEXT ctxOld = { CONTEXT_FULL };
	CONTEXT ctxNew = { CONTEXT_FULL };
	INJECT_CODE ic = { 0 };
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	HMODULE hDll = NULL;
	BOOL bIsSystemBp = TRUE;
	DWORD dwOldEip = 0;
	bRet = CreateProcess(NULL,
		(LPWSTR)m_strExePath.c_str(),
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS,
		NULL,
		NULL,
		&si,
		&pi);
	if (!bRet)
	{
		TipBox(L"CreateProcess ʧ��");
		return;
	}

	//��ֹ�����Խ��̺͵�����һ��ر�
	bRet = DebugSetProcessKillOnExit(FALSE);

	while (WaitForDebugEvent(&dbgEvent, INFINITE))
	{
		switch (dbgEvent.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			hProcess = dbgEvent.u.CreateProcessInfo.hProcess;
			hThread = dbgEvent.u.CreateProcessInfo.hThread;

			//�����ڴ�,���ע��ָ��  
			lpBaseAddress = VirtualAllocEx(hProcess,
				NULL,
				sizeof(INJECT_CODE),
				MEM_COMMIT | MEM_RESERVE,
				PAGE_EXECUTE_READWRITE);
			if (NULL == lpBaseAddress)
			{
				MessageBox(NULL,L"VirtualAllocEx ʧ��",L"Tip",MB_OK);
				return;
			}

			//��ShellCode�ṹ�帳ֵ
			ic.byMOV_EAX = 0xB8;
			ic.dwMOV_EAX_VALUE = (DWORD)lpBaseAddress + offsetof(INJECT_CODE, szDllPath);
			ic.byPUSH_EAX = 0x50;
			ic.byMOV_ECX = 0xB9;
			ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
			ic.wCALL_ECX = 0xD1FF;
			ic.byINT3 = 0xCC;
			memcpy(ic.szDllPath, m_strDllPath.c_str(), m_strDllPath.size()*sizeof(TCHAR));

			//д��ShellCode
			bRet = WriteProcessMemory(hProcess, lpBaseAddress, &ic, sizeof(ic), NULL);
			if (!bRet)
			{
				MessageBox(NULL, L"WriteProcessMemory ʧ��", L"Tip", MB_OK);
				return;
			}
			else
			{
				printf("Shell code is written!\n");
			}


			//��ȡ��ǰ�߳�������
			bRet = GetThreadContext(hThread, &ctxOld);
			if (!bRet)
			{
				MessageBox(NULL, L"GetThreadContext ʧ��", L"Tip", MB_OK);
				return;
			}

			ctxNew = ctxOld;


#ifdef _WIN64
			ctxNew.Rip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Rip;
#else 
			ctxNew.Eip = (DWORD)lpBaseAddress;
			dwOldEip = ctxNew.Eip;
#endif

			bRet = SetThreadContext(hThread, &ctxNew);
			if (!bRet)
			{
				MessageBox(NULL, L"SetThreadContext ʧ��", L"Tip", MB_OK);
				return;
			}

			break;
		case EXCEPTION_DEBUG_EVENT:
			if (dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
			{
				//���ε�ϵͳ�ϵ�
				if (bIsSystemBp)
				{
					bIsSystemBp = FALSE;
					break;
				}

				//�ͷ��ڴ�
				bRet = VirtualFreeEx(hProcess,
					lpBaseAddress,
					0,
					MEM_RELEASE
					);

				if (!bRet)
				{
					MessageBox(NULL, L"VirtualFreeEx ʧ��", L"Tip", MB_OK);
					return;
				}

				//�ָ������򴴽�ʱ��EIP
				bRet = SetThreadContext(hThread, &ctxOld);
				if (!bRet)
				{
					TipBox(L"SetThreadContext ʧ��");
					return;
				}

				bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
				if (!bRet)
				{
					TipBox(L"ContinueDebugEvent ʧ��!!");
					return;
				}
				//�˳������̣��ñ����Գ���������
				ExitProcess(0);
				return;

			}
			break;
		}

		bRet = ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_EXCEPTION_NOT_HANDLED);
		if (!bRet)
		{
			TipBox(L"ContinueDebugEvent ʧ��!!");
			return;
		}
	}
}
