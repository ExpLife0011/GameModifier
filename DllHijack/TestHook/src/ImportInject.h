#pragma once
#include "afxcmn.h"


// ImportInject �Ի���

class ImportInject : public CDialogEx
{
	DECLARE_DYNAMIC(ImportInject)

public:
	ImportInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ImportInject();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFile;
	CString m_strDll;
	CString m_strFun;
	CString m_strTempPath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CListCtrl m_strFunList;
	afx_msg void OnBnClickedButton3();
};
