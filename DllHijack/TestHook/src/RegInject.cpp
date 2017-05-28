// RegInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "RegInject.h"
#include "afxdialogex.h"


// RegInject �Ի���

IMPLEMENT_DYNAMIC(RegInject, CDialogEx)

RegInject::RegInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(RegInject::IDD, pParent)
	, m_strDllPath(_T(""))
{

}

RegInject::~RegInject()
{
}

void RegInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strDllPath);
}


BEGIN_MESSAGE_MAP(RegInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &RegInject::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &RegInject::OnBnClickedButton2)
END_MESSAGE_MAP()


// RegInject ��Ϣ�������


void RegInject::OnBnClickedButton1()
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


void RegInject::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG nReg;
	char szDllPath[MAX_PATH] = { 0 };

	//��HKEY_LOCAL_MACHINE/Software/Microsoft/WindowsNT/CurrentVersion/Windows
	nReg = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
		0,
		KEY_ALL_ACCESS,
		&hKey);

	if (nReg != ERROR_SUCCESS)
	{
		MessageBox("��ע���ʧ��");
		RegCloseKey(hKey);
		return;
	}

	//����AppInit_DLLs�ļ�ֵΪ���ǵ�Dll
	nReg = RegSetValueEx(hKey,
		"AppInit_DLLs",
		0,
		REG_SZ,
		//(byte *)m_strDllPath.GetBuffer(0),
		NULL,
		m_strDllPath.GetLength()
		);
	if (nReg != ERROR_SUCCESS)
	{
		MessageBox("����ע���ʧ�ܣ�");
		RegCloseKey(hKey);
		return;
	}

	RegCloseKey(hKey);
	return;
}
