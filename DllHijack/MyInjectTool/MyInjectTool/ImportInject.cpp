// ImportInject.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyInjectTool.h"
#include "ImportInject.h"
#include "afxdialogex.h"
#include "PEFuncs.h"
#include <IMAGEHLP.H>
// ImportInject �Ի���

IMPLEMENT_DYNAMIC(ImportInject, CDialogEx)

ImportInject::ImportInject(CWnd* pParent /*=NULL*/)
	: CDialogEx(ImportInject::IDD, pParent)
	, m_strFile(_T(""))
	, m_strDll(_T(""))
	, m_strFun(_T(""))
{

}

ImportInject::~ImportInject()
{
}

void ImportInject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFile);
	DDX_Text(pDX, IDC_EDIT2, m_strDll);
	DDX_Text(pDX, IDC_EDIT3, m_strFun);
	DDX_Control(pDX, IDC_LIST1, m_strFunList);
}


BEGIN_MESSAGE_MAP(ImportInject, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ImportInject::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ImportInject::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &ImportInject::OnBnClickedButton3)
END_MESSAGE_MAP()


// ImportInject ��Ϣ�������

/*******************************************************
*��������:�����ڴ��������ļ������Ĵ�С
*��������:����1��ʵ�ʴ�С������2������ֵ
*��������:DWORD
*ע������:��
*����޸�ʱ��:2017/5/13
*******************************************************/
DWORD ClacAlignment(DWORD dwSize, DWORD dwAlign)
{
	if (dwSize % dwAlign != 0)
	{
		return (dwSize / dwAlign + 1)*dwAlign;
	}
	else
	{
		return dwSize;
	}
}

void ImportInject::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL bRet = FALSE;
	// TODO: Add your control notification handler code here
	char szFilter[] = "��ִ���ļ�|*.exe";
	CFileDialog fileDlg(TRUE, "exe", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	char szExePath[MAX_PATH] = { 0 };
	char *szExe = "Temp.exe";
	GetModuleFileName(NULL, szExePath, MAX_PATH);
	(strrchr(szExePath, '\\'))[1] = 0;
	strcat(szExePath, szExe);

	m_strTempPath = szExePath;
	if (fileDlg.DoModal() == IDOK)
	{
		m_strFile = fileDlg.GetPathName();
	}

	//����һ���ļ������޸ģ�Դ�ļ�������
	bRet = ::CopyFile(m_strFile.GetBuffer(0), m_strTempPath.GetBuffer(0), FALSE);

	if (bRet == 0)
	{
		MessageBox("�����ļ�ʧ��");
	}

	LoadFileR(m_strFile.GetBuffer(0), &theApp.m_stMapFile);



	if (!IsPEFile(theApp.m_stMapFile.ImageBase))
	{
		::MessageBox(m_hWnd, "������Ч��PE�ļ�", "������Ч��PE�ļ�", MB_OK);
		UnLoadFile(&theApp.m_stMapFile);
		//EnableEditCtrl(hWnd, FALSE);
		return;
	}


	UpdateData(FALSE);
}


void ImportInject::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	if (m_strFun.GetLength() == 0)
	{
		MessageBox("������DLL������");
		return;
	}
	static int nIndex = 0;
	m_strFunList.InsertItem(nIndex, m_strFun);
	m_strFun.Empty();
	nIndex++;
}


void ImportInject::OnBnClickedButton3()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(FALSE);
	// TODO: Add your control notification handler code here
	FILE* fp;
	//���һ����
	PIMAGE_SECTION_HEADER lpImgLastSection;
	//Ҫ��ӵ�����
	IMAGE_SECTION_HEADER ImgNewSection;
	//��һ����ͷ
	PIMAGE_SECTION_HEADER lpFirstSectionHeader;
	//��Դ�ļ��޸ġ�
	PIMAGE_NT_HEADERS lpNtHeader = new IMAGE_NT_HEADERS;
	PIMAGE_NT_HEADERS lpNewNtHeader = new IMAGE_NT_HEADERS;
	//�ڵ���Ŀ
	int nSectionNum = 0;
	//�½ڵ�RVA
	DWORD dwNewSectionRVA, dwNewImportRva;
	//�½ڵ��ļ�ƫ��
	DWORD dwNewFA = 0;
	//�ڶ���
	int nSectionAlignment = 0;
	//�ļ�����
	int nFileAlignment = 0;
	//DLL���Ƶĳ���
	int nDllLen = 0;
	//��Ҫд��ĺ�����Ŀ
	int nFunNum = m_strFunList.GetItemCount();
	//������½ڵ��ļ�ƫ��
	DWORD dwNewOffset = 0;
	//Ҫ��ӵĽڱ�ͷ
	//IMAGE_SECTION_HEADER ImgNewSection;
	PIMAGE_IMPORT_DESCRIPTOR lpImport, lpNewImport;
	//ԭ�������Ĵ�С�����µ����Ĵ�С
	DWORD dwImportSize, dwNewImportSize;
	//�����½�ͷ���ļ�ƫ��
	DWORD dwNewSectionOffset;


	fp = ::fopen(m_strTempPath.GetBuffer(0), "rb+");
	if (fp == NULL)
	{
		::DeleteFile(m_strTempPath.GetBuffer(0));
		MessageBox("����ʱ�ļ�ʧ�ܣ���");
		return;
	}

	lpFirstSectionHeader = GetFirstSectionHeader(theApp.m_stMapFile.ImageBase);
	lpNtHeader = GetNtHeaders(theApp.m_stMapFile.ImageBase);
	nSectionNum = lpNtHeader->FileHeader.NumberOfSections;
	nSectionAlignment = lpNtHeader->OptionalHeader.SectionAlignment;
	nFileAlignment = lpNtHeader->OptionalHeader.FileAlignment;

	//��ȡ������ָ��
	lpImport = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(theApp.m_stMapFile.ImageBase, FALSE, IMAGE_DIRECTORY_ENTRY_IMPORT, &dwImportSize);
	//�����µĵ����Ĵ�С:�ɵĵ�����С + �µĵ�����С
	dwNewImportSize = dwImportSize + sizeof(IMAGE_IMPORT_DESCRIPTOR);
	//��ȡ���һ����ͷ
	lpImgLastSection = lpFirstSectionHeader + (nSectionNum - 1);
	//��ȡ�½ڵ�RVA
	dwNewSectionRVA = lpImgLastSection->VirtualAddress
		+ ClacAlignment(lpImgLastSection->Misc.VirtualSize, nSectionAlignment);
	//�����µ��ļ�ƫ��
	dwNewFA = lpImgLastSection->PointerToRawData
		+ ClacAlignment(lpImgLastSection->SizeOfRawData, nFileAlignment);

	//1.�ڸ��Ƶ��ļ���д��DLL��
	fseek(fp, dwNewFA, SEEK_SET);
	dwNewOffset = m_strDll.GetLength() + 1;
	fwrite(m_strDll.GetBuffer(0), dwNewOffset, 1, fp);

	DWORD *arrINTRva = new DWORD[nFunNum + 1];
	memset(arrINTRva, 0, sizeof(DWORD)*(nFunNum + 1));

	//2.д�����еĵ�IMAGE_IMPORT_BY_NAME�ṹ,Ҳ����д�����к�����
	for (int i = 0; i < nFunNum; i++)
	{
		DWORD dwTempRva = 0;

		static int nFunLen = 0;
		PIMAGE_IMPORT_BY_NAME pImportFun = new IMAGE_IMPORT_BY_NAME;
		pImportFun->Hint = i;
		CString strFunName = m_strFunList.GetItemText(i, 0);
		fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
		//����IMAGE_IMPORT_BY_NAME��RVA��������
		dwTempRva = dwNewSectionRVA + dwNewOffset;
		arrINTRva[i] = dwTempRva;
		dwNewOffset = dwNewOffset + strFunName.GetLength() + 1 + sizeof(WORD);
		memcpy(pImportFun->Name, strFunName.GetBuffer(0), strFunName.GetLength() + 1);
		fwrite(pImportFun, strFunName.GetLength() + 1 + sizeof(WORD), 1, fp);


	}
	DWORD dwINTRVA = dwNewSectionRVA + dwNewOffset;

	//3.д�����еĵ�INT�ṹ
	for (int i = 0; i < nFunNum + 1; i++)
	{

		fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
		dwNewOffset += sizeof(DWORD);
		//ĩβ���0�ṹ��
		fwrite(&arrINTRva[i], sizeof(DWORD), 1, fp);
	}

	//4.�����¿ռ��žɵĵ�IID���µ�IID
	lpNewImport = (PIMAGE_IMPORT_DESCRIPTOR)malloc(dwNewImportSize);
	memset(lpNewImport, 0, dwNewImportSize);
	memcpy(lpNewImport, lpImport, dwImportSize);

	int i = 0;
	while (1)
	{
		if (lpNewImport[i].OriginalFirstThunk == 0 && lpNewImport[i].TimeDateStamp == 0 &&
			lpNewImport[i].ForwarderChain == 0 && lpNewImport[i].Name == 0 && lpNewImport[i].FirstThunk == 0)
		{
			lpNewImport[i].Name = dwNewSectionRVA;
			lpNewImport[i].TimeDateStamp = 0;
			lpNewImport[i].ForwarderChain = 0;
			lpNewImport[i].FirstThunk = dwINTRVA;
			lpNewImport[i].OriginalFirstThunk = dwINTRVA;
			break;
		}
		else i++;
	}
	//�����µĵ����RVA
	dwNewImportRva = dwNewSectionRVA + dwNewOffset;
	//д�����еĵ������
	fseek(fp, dwNewFA + dwNewOffset, SEEK_SET);
	fwrite(lpNewImport, dwNewImportSize, 1, fp);
	dwNewOffset += dwNewImportSize;

	//�����ļ�������Ҫ�����ֵ
	DWORD dwFileAlign = ClacAlignment(dwNewOffset, nFileAlignment) - dwNewOffset;

	for (size_t i = 0; i < dwFileAlign; i++)
	{
		fputc('\0', fp);
	}


	//5.���һ���½ڱ�ͷ��
	memset(&ImgNewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	//�����Ϊ.newsec���½�
	strcpy((char*)ImgNewSection.Name, ".newsec");
	ImgNewSection.VirtualAddress = dwNewSectionRVA;
	ImgNewSection.PointerToRawData = dwNewFA;
	ImgNewSection.Misc.VirtualSize = ClacAlignment(dwNewOffset, nSectionAlignment);
	ImgNewSection.SizeOfRawData = ClacAlignment(dwNewOffset, nFileAlignment);
	ImgNewSection.Characteristics = 0xC0000040;

	//�����½�ͷ���ļ�ƫ��
	dwNewSectionOffset = (DWORD)lpFirstSectionHeader -
		(DWORD)theApp.m_stMapFile.ImageBase + sizeof(IMAGE_SECTION_HEADER)*nSectionNum;
	fseek(fp, dwNewSectionOffset, 0);

	//д��ڱ�ͷ
	fwrite(&ImgNewSection, sizeof(IMAGE_SECTION_HEADER), 1, fp);
	memcpy(&ImgNewSection, lpFirstSectionHeader, sizeof(IMAGE_SECTION_HEADER));
	fseek(fp, (DWORD)lpFirstSectionHeader - (DWORD)theApp.m_stMapFile.ImageBase, SEEK_SET);
	fwrite(&ImgNewSection, sizeof(IMAGE_SECTION_HEADER), 1, fp);

	//6.����NTͷ����
	memcpy(lpNewNtHeader, lpNtHeader, sizeof(IMAGE_NT_HEADERS));
	int nNewImageSize = lpNtHeader->OptionalHeader.SizeOfImage + ClacAlignment(dwNewOffset, nSectionAlignment);
	lpNewNtHeader->OptionalHeader.SizeOfImage = nNewImageSize;
	lpNewNtHeader->OptionalHeader.DataDirectory[11].Size = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[11].VirtualAddress = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[12].Size = 0;
	lpNewNtHeader->OptionalHeader.DataDirectory[12].VirtualAddress = 0;
	lpNewNtHeader->FileHeader.NumberOfSections = nSectionNum + 1;
	lpNewNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = dwNewImportRva;
	lpNewNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = dwNewImportSize;

	//д���µ�NTͷ
	fseek(fp, (DWORD)(lpNtHeader)-(DWORD)theApp.m_stMapFile.ImageBase, SEEK_SET);
	fwrite(lpNewNtHeader, sizeof(IMAGE_NT_HEADERS), 1, fp);

	if (fp != NULL)
	{
		fclose(fp);
	}
	UnLoadFile(&theApp.m_stMapFile);

	//�ͷ�ɨβ����

	if (arrINTRva != NULL)
	{
		delete[] arrINTRva;
		arrINTRva = NULL;
	}
}
