#pragma once


// RemoteInject �Ի���

class RemoteInject : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteInject)

public:
	RemoteInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~RemoteInject();

// �Ի�������
	enum { IDD = IDD_DIALOG10 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedInject();
};
