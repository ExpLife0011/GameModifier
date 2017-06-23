// RemoteInject.cpp : ʵ���ļ�
//

#include "RemoteInject.h"
#include <tlhelp32.h>  
#include <assert.h>
#include <stdio.h>
#include <io.h>
#include "include/Utils.h"
void RemoteInject::StartInject(std::string stExe, std::string strDll)
{
	char szPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, szPath);
	m_strDllPath = szPath + std::string("\\") + strDll;
	assert(_access(m_strDllPath.c_str(), 0) == 0);

	m_dwPID = GetProcessIdByName(stExe);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	DWORD * pDwTidRemote = NULL;



	//UpdateData(TRUE);


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

	if (hProcess == NULL)
	{
		TipBox("�򿪽���ʧ��!!!!");
		return;
	}


	//1.��Զ�̽����з����ڴ�
	BYTE* 	pszRemoteBuffer = NULL;
	int  bufLen = m_strDllPath.size();
	pszRemoteBuffer = (BYTE *)VirtualAllocEx(hProcess, NULL, bufLen, MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		TipBox("����Զ�̿ռ�ʧ��");
		return;
	}
	//2.��Զ������ĵ�ַ����д��DLL��·��
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.c_str(), bufLen, &dwWriten))
	{
		TipBox("д���ڴ�ʧ��");
	}

	//3.��ȡԶ�̽�����LaodLibry�ĵ�ַ,�������õ��ɺ���ÿ�������е�kernel32�ĵ�ַ�Ķ�һ����Զ����Ҳһ����
	HMODULE hMouDle = GetModuleHandleA("Kernel32");
	PTHREAD_START_ROUTINE pfnLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA");

	if (pfnLoadLibrary == NULL)
	{
		TipBox("��ȡLoadLibrary��ַʧ�ܣ�����");
		return;
	}

	//4.����Զ���߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pszRemoteBuffer, 0, NULL);

	DWORD dwErrCode = GetLastError();
	if (hThread == NULL)
	{
		TipBox("����Զ���߳�ʧ��");
		return;
	}

	WaitForSingleObject(hThread, 2000);
}

DWORD RemoteInject::GetProcessIdByName(std::string name)
{
	// ���������Ϣ�ṹ  
	PROCESSENTRY32 pe32 = { sizeof(pe32) };

	// ����ϵͳ��ǰ���̿���  
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE)
		return FALSE;

	std::wstring strWname = Utf82WideChar((char*)name.c_str());
	if (Process32First(hProcessShot, &pe32))
	{
		do
		{
			if (wcscmp(pe32.szExeFile, strWname.c_str()) == 0)
			{
				return pe32.th32ProcessID;
			}

		} while (Process32Next(hProcessShot, &pe32));
	}
	return 0;
}
