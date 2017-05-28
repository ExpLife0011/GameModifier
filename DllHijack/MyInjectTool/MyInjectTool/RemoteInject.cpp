// RemoteInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "RemoteInject.h"
#include "afxdialogex.h"


// RemoteInject �Ի���

IMPLEMENT_DYNAMIC(RemoteInject, CDialogEx)

RemoteInject::RemoteInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(RemoteInject::IDD, pParent)
	, m_dwPID(0)
	, m_strDllPath(_T(""))
{

}

RemoteInject::~RemoteInject()
{
}

void RemoteInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwPID);
	DDX_Text(pDX, IDC_EDIT4, m_strDllPath);
}


BEGIN_MESSAGE_MAP(RemoteInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &RemoteInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_INJECT, &RemoteInject::OnBnClickedInject)
END_MESSAGE_MAP()


// RemoteInject ��Ϣ�������


void RemoteInject::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char szFilter[] = "��̬���ӿ�|*.dll";
	CFileDialog fileDlg(TRUE, "dll", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strDllPath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}


void RemoteInject::OnBnClickedInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	char* pszRemoteBuffer = NULL;
	DWORD * pDwTidRemote = NULL;



	//UpdateData(TRUE);


	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwPID);

	if (hProcess == NULL)
	{
		MessageBox("�򿪽���ʧ��!!!!");
		return;
	}


	//1.��Զ�̽����з����ڴ�
	pszRemoteBuffer = (char *)VirtualAllocEx(hProcess, NULL, m_strDllPath.GetLength(), MEM_COMMIT, PAGE_READWRITE);

	if (pszRemoteBuffer == NULL)
	{
		MessageBox("����Զ�̿ռ�ʧ��");
		return;
	}
	//2.��Զ������ĵ�ַ����д��DLL��·��
	SIZE_T dwWriten;
	if (!WriteProcessMemory(hProcess, pszRemoteBuffer, (LPVOID)m_strDllPath.GetBuffer(0), m_strDllPath.GetLength(), &dwWriten))
	{
		MessageBox("д���ڴ�ʧ��");
	}

	//3.��ȡԶ�̽�����LaodLibry�ĵ�ַ,�������õ��ɺ���ÿ�������е�kernel32�ĵ�ַ�Ķ�һ����Զ����Ҳһ����
	HMODULE hMouDle = GetModuleHandle("Kernel32");
	PTHREAD_START_ROUTINE pfnLoadLibrary = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");

	if (pfnLoadLibrary == NULL)
	{
		MessageBox("��ȡLoadLibrary��ַʧ�ܣ�����");
		return;
	}

	//4.����Զ���߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, pfnLoadLibrary, pszRemoteBuffer, 0, NULL);

	DWORD dwErrCode = GetLastError();
	if (hThread == NULL)
	{
		MessageBox("����Զ���߳�ʧ��");
		return;
	}

	WaitForSingleObject(hThread, 2000);
}
