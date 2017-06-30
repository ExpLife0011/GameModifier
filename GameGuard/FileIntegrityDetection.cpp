#include "FileIntegrityDetection.h"
#include "ThirdParty/Decoder/crc32.h"
#include "Framework.h"
FileIntegrityDetector::FileIntegrityDetector()
{
}

FileIntegrityDetector::~FileIntegrityDetector()
{
}
void FileIntegrityDetector::CheckFileModified()
{
	AddCrc322File();
	assert(IsFileModified());
}

bool FileIntegrityDetector::CheckFileSum()
{
	FILE* pFile = fopen("InjectDll.dll", "r");
	False_Return(pFile != nullptr);
	unsigned long crc32;
	False_Return(Crc32_ComputeFile(pFile, &crc32) == 0);
	fclose(pFile);

	pFile = fopen("InjectDll2.dll", "r");
	False_Return(pFile != nullptr);
	unsigned long crc322;
	False_Return(Crc32_ComputeFile(pFile, &crc322) == 0);
	fclose(pFile);

	assert(crc322 == crc32);
	return true;

}

void FileIntegrityDetector::Detect()
{
//	CheckFileSum();
	CheckFileModified();
}
bool FileIntegrityDetector::AddCrc322File()
{
	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
	PIMAGE_NT_HEADERS       pNtHeader=NULL;
	PIMAGE_SECTION_HEADER   pSecHeader=NULL;


	TCHAR szFileName[MAX_PATH ] ;
	static OPENFILENAME ofn ;
	HANDLE hFile;	
	DWORD szTemp,fileSize,szCRC32;
	TCHAR  *pBuffer ;

	static TCHAR szFilter[] =TEXT ("*.dll");

	szFileName[0] = '\0';                         
	ZeroMemory(&ofn, sizeof(ofn));                             // ��ʼ��OPENFILENAME�ṹ
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrFile         = szFileName ;         
	ofn.nMaxFile          = MAX_PATH ;   
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;           

	if(GetOpenFileName (&ofn))
	{

		hFile = CreateFile(
			szFileName,
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ, 
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);


		if( hFile != INVALID_HANDLE_VALUE )
		{
			fileSize=GetFileSize(hFile,&szTemp);  // ��ȡ�ļ���С
			if (fileSize == 0xFFFFFFFF) return FALSE;
			pBuffer = new TCHAR [fileSize];     // �����ڴ�
			if(ReadFile(hFile, pBuffer, fileSize, &szTemp, NULL)==NULL) return FALSE;// ��ȡ����
		}
		else
		{
			printf("I can't access file!");
			return 0;
		}

	}
	else
		return 0;


	pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;	 	 
	pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);

	fileSize=fileSize-DWORD(pDosHeader->e_lfanew);//��PE�ļ�ͷǰ�ǲ�������ȥ��

	szCRC32=Crc32_ComputeBuf((BYTE*)(pBuffer+pDosHeader->e_lfanew),fileSize);

	//��3ch����PE�ļ�ͷָ�룺
	DWORD Writeadd=DWORD(pDosHeader->e_lfanew-4);//��λ��PE�ļ�ͷ�����ִ���PE\0\0������ǰ4���ֽڴ���׼������дCRC-32ֵ��

	SetFilePointer(hFile,Writeadd,NULL,FILE_BEGIN);//�����ļ�ָ��

	if(!WriteFile(hFile,&szCRC32,4,&szTemp,NULL))
	{
		printf("Error while patching !");
		CloseHandle(hFile);
		return 0;
	}


	CloseHandle(hFile);
	printf("Patch successfull !");
}

bool FileIntegrityDetector::IsFileModified()
{

	PIMAGE_DOS_HEADER	    pDosHeader=NULL;
	PIMAGE_NT_HEADERS       pNtHeader=NULL;
	PIMAGE_SECTION_HEADER   pSecHeader=NULL;

	DWORD fileSize,OriginalCRC32,NumberOfBytesRW;
	//TCHAR  *pBuffer ,szFileName[MAX_PATH]; 
	TCHAR* szFileName = L"InjectedDll.dll";

	//����ļ���
	//	GetModuleFileName(NULL,szFileName,MAX_PATH);
	//���ļ�
	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if ( hFile == INVALID_HANDLE_VALUE ) return FALSE;


	//����ļ����� :
	fileSize = GetFileSize(hFile,NULL);
	if (fileSize == 0xFFFFFFFF) return FALSE;

	TCHAR* pBuffer = new TCHAR [fileSize];     // �����ڴ�,Ҳ����VirtualAlloc�Ⱥ��������ڴ�
	ReadFile(hFile,pBuffer, fileSize, &NumberOfBytesRW, NULL);//��ȡ�ļ�����
	CloseHandle(hFile);  //�ر��ļ�


	pDosHeader=(PIMAGE_DOS_HEADER)pBuffer;
	pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
	//��λ��PE�ļ�ͷ�����ִ���PE\0\0������ǰ4���ֽڴ��������������������CRC-32ֵ��
	OriginalCRC32 =*((DWORD *)((DWORD)pNtHeader-4));	 	 

	fileSize=fileSize-DWORD(pDosHeader->e_lfanew);//��PE�ļ�ͷǰ�ǲ�������ȥ��
	unsigned long iCurCrc = Crc32_ComputeBuf((BYTE*)(pBuffer+pDosHeader->e_lfanew),fileSize);
	if (iCurCrc == OriginalCRC32 )
		return TRUE;
	else
		return FALSE;
}