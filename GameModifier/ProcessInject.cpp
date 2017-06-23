// ProcessInject.cpp : ʵ���ļ�
//


#include "ProcessInject.h"
#include "include/Utils.h"
#include <io.h>
#include <assert.h>
//ShellCode�ṹ��
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



// ProcessInject ��Ϣ�������

HANDLE g_hProcess1 = NULL;
LPVOID g_lpBuffer1 = NULL;
void ProcessInject::OnStartInject(std::string exePath,std::string dllPath)
{
	m_strExePath = exePath;
	CHAR szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, szPath);
	m_strDllPath = szPath + std::string("\\") + dllPath;
	assert(_access(m_strDllPath.c_str(), 0) == 0);

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	CONTEXT oldContext = { 0 };
	CONTEXT newContext = { 0 };
	INJECT_CODE ic = { 0 };
	DWORD dwOldEip = 0;
	si.wShowWindow = SW_SHOWNORMAL;
	si.cb = sizeof(PROCESS_INFORMATION);
	HANDLE hThread = NULL;
	//�Թ���ķ�ʽ��������
	std::wstring path = Utf82WideChar((char*)m_strExePath.c_str());
	bRet = CreateProcess(path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED,
		NULL, NULL, &si, &pi);

	if (!bRet)
	{
		TipBox("CreateProcess ʧ��");
		return;
	}

	g_hProcess1 = pi.hProcess;
	hThread = pi.hThread;
	//�����ڴ�
	g_lpBuffer1 = VirtualAllocEx(g_hProcess1, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (g_lpBuffer1 == NULL)
	{
		TipBox("VirtualAllocEx ʧ��");
		return;
	}

	//��ShellCode�ṹ�帳ֵ
	ic.byPUSH = 0x68;
	ic.dwPUSH_VALUE = 0x12345678;
	ic.byPUSHFD = 0x9C;
	ic.byPUSHAD = 0x60;
	ic.byMOV_EAX = 0xB8;
	ic.dwMOV_EAX_VALUE = (DWORD)g_lpBuffer1 + offsetof(INJECT_CODE, szDllPath);
	ic.byPUSH_EAX = 0x50;
	ic.byMOV_ECX = 0xB9;
	ic.dwMOV_ECX_VALUE = (DWORD)&LoadLibrary;
	ic.wCALL_ECX = 0xD1FF;
	ic.byPOPAD = 0x61;
	ic.byPOPFD = 0x9D;
	ic.byRETN = 0xC3;
	memcpy(ic.szDllPath, m_strDllPath.c_str(), m_strDllPath.length());

	//д��ShellCode
	bRet = WriteProcessMemory(g_hProcess1, g_lpBuffer1, &ic, sizeof(ic), NULL);
	if (!bRet)
	{
		TipBox("д���ڴ�ʧ��");
		return;
	}

	//��ȡ�߳�������
	oldContext.ContextFlags = CONTEXT_FULL;
	bRet = GetThreadContext(hThread, &oldContext);
	if (!bRet)
	{
		TipBox("GetThreadContext ʧ��");
		return;
	}
	newContext = oldContext;


#ifdef _WIN64
	newContext.Rip = (DWORD)g_lpBuffer1;
	dwOldEip = newContext.Rip;
#else 
	newContext.Eip = (DWORD)g_lpBuffer1;
	dwOldEip = newContext.Eip;
#endif



	//;��ָ��ָ��ShellCode��һ��push 12345678h�еĵ�ַ,д�뷵�ص�ַ
	bRet = WriteProcessMemory(g_hProcess1, ((char*)g_lpBuffer1) + 1, &dwOldEip, sizeof(DWORD), NULL);
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


void ProcessInject::OnReleaseInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!VirtualFreeEx(g_hProcess1, g_lpBuffer1, 0, MEM_RELEASE))
	{
		TipBox("VirtualFreeEx ʧ��");
		return;
	}

	TipBox("�ͷŶԷ��ռ�ɹ�");
}



