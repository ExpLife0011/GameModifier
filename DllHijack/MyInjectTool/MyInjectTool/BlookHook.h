#pragma once


// BlookHook �Ի���

class BlookHook : public CDialogEx
{
	DECLARE_DYNAMIC(BlookHook)

public:
	BlookHook(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~BlookHook();

// �Ի�������
	enum { IDD = IDD_DIALOG11 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwPID;
	afx_msg void OnBnClickedButton1();
};
