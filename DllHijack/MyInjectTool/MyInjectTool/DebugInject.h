#pragma once
#include "afxwin.h"


// DebugInject �Ի���

class DebugInject : public CDialogEx
{
	DECLARE_DYNAMIC(DebugInject)

public:
	DebugInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DebugInject();

// �Ի�������
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strExePath;
	CString m_strDllPath;
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();

};
