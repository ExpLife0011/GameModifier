#pragma once


// RegInject �Ի���

class RegInject : public CDialogEx
{
	DECLARE_DYNAMIC(RegInject)

public:
	RegInject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~RegInject();

// �Ի�������
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDllPath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
