#pragma once


// InputInject �Ի���

class InputInject : public CDialogEx
{
	DECLARE_DYNAMIC(InputInject)

public:
	InputInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~InputInject();

// �Ի�������
	enum { IDD = IDD_DIALOG9 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	CString m_strDllPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedInject();
};
