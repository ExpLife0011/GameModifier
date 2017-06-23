// ThreadInject.cpp : ʵ���ļ�
//

#include "ThreadInject.h"
#include <assert.h>
#include "include/Utils.h"
//777A01D5    68 78563412     push    12345678
//777A01DA    9C              pushfd
//777A01DB    60              pushad

//777A01EB    61              popad
//777A01EC    9D              popfd
//777A01ED    C3              retn
//�ṹ�����ֽڶ���1
#pragma pack(1)  
typedef struct _INJECT_CODE
{
	BYTE  byPUSH;
	DWORD dwPUSH_VALUE;
	BYTE  byPUSHFD;
	BYTE  byPUSHAD;
	BYTE  byMOV_EAX;          //mov eax, addr szDllpath
	DWORD dwMOV_EAX_VALUE;
	BYTE  byPUSH_EAX;         //push eax
	BYTE  byMOV_ECX;          //mov ecx, LoadLibrary
	DWORD dwMOV_ECX_VALUE;
	WORD  wCALL_ECX;          //call ecx
	BYTE  byPOPAD;
	BYTE  byPOPFD;
	BYTE  byRETN;
	CHAR  szDllPath[MAX_PATH];
}INJECT_CODE, *PINJECT_CODE;
#pragma pack()  


// ThreadInject �Ի���




HANDLE g_hProcess = 0;
LPVOID g_lpBuffer = NULL;
void ThreadInject::StartInject(std::wstring stExe, std::wstring strDll)
{

	m_strExePath = stExe;
	TCHAR szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, szPath);
	m_strDllPath = szPath + std::wstring(L"\\") + strDll;
	assert(_waccess(m_strDllPath.c_str(), 0) == 0);

	m_dwPid = GetProcessIdByName(WideChar2Utf8((wchar_t*)m_strExePath.c_str()));
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//��Ŀ�����
	BOOL   bRet = 0;
	
	INJECT_CODE ic = { 0 };
	THREADENTRY32 te32 = { 0 };
	HANDLE hThreadSnap = NULL;
	DWORD dwThreadId = 0;
	HANDLE hThread = NULL;
	CONTEXT oldContext = { 0 };
	CONTEXT newContext = { 0 };
	DWORD dwOldEip = 0;
	//�򿪽���
	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPid);
	
	if (!g_hProcess)
	{
		TipBox("OpenProcess ʧ��");
		return;
	}


	g_lpBuffer = VirtualAllocEx(g_hProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!g_lpBuffer)
	{
		TipBox("VirtualAllocEx ʧ��");
		return;
	}
	std::string dllPath = WideChar2Utf8((wchar_t*)m_strDllPath.c_str());
	//��ShellCode�ṹ�帳ֵ
	ic.byPUSH			= 0x68;
	ic.dwPUSH_VALUE		= 0x12345678;
	ic.byPUSHFD			= 0x9C;
	ic.byPUSHAD			= 0x60;
	ic.byMOV_EAX		= 0xB8;
	ic.dwMOV_EAX_VALUE = (DWORD)g_lpBuffer + offsetof(INJECT_CODE, szDllPath);
	ic.byPUSH_EAX		= 0x50;
	ic.byMOV_ECX		= 0xB9;
	ic.dwMOV_ECX_VALUE  = (DWORD)&LoadLibrary;
	ic.wCALL_ECX		= 0xD1FF;
	ic.byPOPAD			= 0x61;
	ic.byPOPFD			= 0x9D;
	ic.byRETN			= 0xC3;
	memcpy(ic.szDllPath, dllPath.c_str(), dllPath.length());

	//д��ShellCode
	bRet = WriteProcessMemory(g_hProcess, g_lpBuffer, &ic, sizeof(ic), NULL);
	if (!bRet)
	{
		TipBox("д���ڴ�ʧ��");
		return;
	}

	//�����߳̿��ղ���Ŀ��������߳�
	te32.dwSize = sizeof(te32);
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		TipBox("CreateToolhelp32Snapshot ʧ��");
		return;
	}

	//������ѯĿ��������߳�ID
	if (Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if (m_dwPid == te32.th32OwnerProcessID)
			{
				dwThreadId = te32.th32ThreadID;
				break;
			}
		} while (Thread32Next(hThreadSnap, &te32));
	}
	
	//��Ŀ�����߳�
	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwThreadId);

	if (!hThread)
	{
		TipBox("OpenThread ʧ��");
		return;
	}

	//����Ŀ�����߳�
	bRet = SuspendThread(hThread);

	if (bRet == -1)
	{
		TipBox("SuspendThread ʧ��");
		return;
	}

	oldContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(hThread, &oldContext);
	if (!bRet)
	{
		TipBox("GetThreadContext ʧ��");
		return;
	}
	newContext = oldContext;


#ifdef _WIN64
	newContext.Rip = (DWORD)g_lpBuffer;
	dwOldEip = newContext.Rip;
#else 
	newContext.Eip = (DWORD)g_lpBuffer;
	dwOldEip = newContext.Eip;
#endif

	//;��ָ��ָ��ShellCode��һ��push 12345678h�еĵ�ַ,д�뷵�ص�ַ
	bRet = WriteProcessMemory(g_hProcess, ((char*)g_lpBuffer) + 1, &dwOldEip, sizeof(DWORD), NULL);
	if (!bRet)
	{
		TipBox("д���ڴ�ʧ��");
		return;
	}

	bRet = SetThreadContext(hThread, &newContext);

	if (!bRet)
	{
		TipBox("SetThreadContext ʧ��");
		return;
	}

	//Ȼ������߳�������
	bRet = ResumeThread(hThread);
	
	if (bRet == -1)
	{
		TipBox("ResumeThread ʧ��");
		return;
	}


}


void ThreadInject::OnReleaseInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!VirtualFreeEx(g_hProcess, g_lpBuffer, 0, MEM_RELEASE))
	{
		TipBox("VirtualFreeEx ʧ��");
		return;
	}

	TipBox("�ͷŶԷ��ռ�ɹ�");
}
