// APCInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "APCInject.h"
#include "afxdialogex.h"


// APCInject �Ի���

IMPLEMENT_DYNAMIC(APCInject, CDialogEx)

APCInject::APCInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(APCInject::IDD, pParent)
	, m_strExePath(_T(""))
	, m_strDllPath(_T(""))
{

}

APCInject::~APCInject()
{
}

void APCInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strExePath);
	DDX_Text(pDX, IDC_EDIT2, m_strDllPath);
}


BEGIN_MESSAGE_MAP(APCInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &APCInject::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &APCInject::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_INJECT, &APCInject::OnBnClickedInject)
END_MESSAGE_MAP()


// APCInject ��Ϣ�������


void APCInject::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	char szFilter[] = "��ִ�г���|*.exe";
	CFileDialog fileDlg(TRUE, "exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	UpdateData(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strExePath = fileDlg.GetPathName();
	}
	UpdateData(FALSE);
}


void APCInject::OnBnClickedButton4()
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


void APCInject::OnBnClickedInject()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DWORD dwRet = 0;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	//�Թ���ķ�ʽ��������
	dwRet = CreateProcess(m_strExePath.GetBuffer(0),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi);

	if (!dwRet)
	{
		MessageBox("CreateProcessʧ�ܣ���");
		return;
	}

	PVOID lpDllName = VirtualAllocEx(pi.hProcess,
		NULL,
		m_strDllPath.GetLength(),
		MEM_COMMIT,
		PAGE_READWRITE);



	if (lpDllName)
	{
		//��DLL·��д��Ŀ����̿ռ�
		if (WriteProcessMemory(pi.hProcess, lpDllName, m_strDllPath.GetBuffer(0), m_strDllPath.GetLength(), NULL))
		{
			LPVOID nLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
			//��Զ��APC���в���LoadLibraryA
			if (!QueueUserAPC((PAPCFUNC)nLoadLibrary, pi.hThread, (DWORD)lpDllName))
			{
				MessageBox("QueueUserAPCʧ�ܣ���");
				return;
			}
		}
		else
		{
			MessageBox("WriteProcessMemoryʧ�ܣ���");
			return;
		}
	}

	//�ָ����߳�
	ResumeThread(pi.hThread);
	MessageBox("APCע��ɹ�");

}
