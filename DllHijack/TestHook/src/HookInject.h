#pragma once


// HookInject �Ի���

class HookInject : public CDialogEx
{
	DECLARE_DYNAMIC(HookInject)

public:
	HookInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~HookInject();

// �Ի�������
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ����ID
	DWORD m_dwPid;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedInject();
	afx_msg void OnBnClickedUnhook();
};
