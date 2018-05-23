#ifndef GM_FILEUPLOAD_H
#define GM_FILEUPLOAD_H

#define FLDUPD_EVT_START     EVT_USER + 1050		// �����ļ��ϴ�
#define FLDUPD_EVT_STOP		 EVT_USER + 1051		// �˳��ļ��ϴ�
#define FLDUPD_EVT_UPLOAD	 EVT_USER + 1052		// ��ʼ�ϴ��ļ�
#define FLDUPD_EVT_SEND	     EVT_USER + 1053		// �����ϴ��ļ�

#define MAX_FILE_BYTES_POST		512000			//  1024*500 = 500K  Ҫ���͵��ļ���һ����෢�͵��ֽ���


typedef struct _FileUpload FileUpload;
typedef struct _newgimis newgimis;

// �ϴ�¼���ļ��ֶ��б�   2014-04-10
typedef struct  
 {
 	char	sRecName[MAX_FILE_NAME+1];					// ¼���ļ���(��·��)
 	char    cSendStatus;								// ����״̬ 0 : δ����, 1 �򿪷����ļ� ��10 �ѷ���
} IAudRecFieldA;
 
// �ϴ�¼���ļ���¼�б�
typedef struct  
{
 	uint16 			nRecordCount;								// ��¼������
 	uint16 			nSelRec;									// ѡ�е������, nSelRec >= nRecordCount δѡ��
	char            cStatus;									// 0: û���յ��µ��ϴ�ָ��, 1: �յ��µ��ϴ�ָ��
	char            sDate1[11];									// ��ʼʱ��  yymmddhhmm,������ʱ��
	char            sDate2[11];									// ����ʱ��  yymmddhhmm,������ʱ��
 	IAudRecFieldA	*pAudRecFieldA;								// һ����¼
} IAudRecRecordsA;
// Add End 2014-04-10

struct _FileUpload					
{
	AEEApplet    a;
	newgimis		*m_pParent;

	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   piWResp;				// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	ISourceUtil* pisu;
	uint32		 dwPostOvertime;		// ��¼�����ļ������Ӧ���ʱ�䣬��λ�����ʱ�����涨ʱ�䣬�ر����ӣ�����ʾʧ��
	
	char		 pURL[256];
	int          nBytes;					// count of bytes in response
	char         sBodyBuf[10];				// ֻҪӦ��ĵ�0���ַ�	
	char         cSendSts;					// ���͵�״̬
											//	'1':  ���ν��ճɹ�������������һ�Σ����ֻ��˻����������һ�Σ���������һ�β��ҷ��͵�����Ƶ�ļ������ֻ��˻�ɾ����Ƶ�ļ�����ʾ�ֻ��û������ͳɹ�����
											//	'0':  ���ν���ʧ�ܣ����ֻ��˻��ط�����
											//	'A': ��ʾ���ճ����쳣���ֻ�����ʾ������ʧ�ܡ���Ϣ���ֻ��û���������͵�����Ƶ�ļ����򲻻�ɾ����Ƶ�ļ�
	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	int32		 iFileRead;					// �����ļ����ֽ���  
	uint8		 nPartNum;					// ���ڴ��ڼ��Σ�1 ��1�Σ�2 ��2�Σ�����������  
	uint8		 nPartsTotal;				// �ܶ���

	IFileMgr*    pFileMgr;
	IFile*		 pFileUpload;				// ���ϴ����ļ����
    FileInfo	 oFileInfo;					// �ļ���Ϣ
	int			 nAddHours;					// 
	uint32       nHasRead;					// �Ѿ��ϴ���ȡ���ֽ���

	AECHAR		pText[TEXT_MAXSIZE+1];		// ���ϴ��ļ��йص�����,���100��˫�ֽ��ַ�

	char		 cType;						// �ļ������� '0' �Ѿ���ʼ��  '1' ��Ƶ�ļ��� '2' �����ļ� config.txt, '3' ��־ rec_info.log
											//  '4' ��Ƭ  
											//  '5' ¼���ļ�  Add by ----  2014-04-10
	char         sFile[MAX_FILE_NAME+1];	// �ļ�������·����	
	uint8		 nState;					// ״̬
											// 0(SUCCESS)    �ɹ�
											// 1(EFAILED)    ʧ�� 

	IAudRecRecordsA *pAudRecRecordsA;		// �ϴ�¼���ļ���¼				// Add by ---- 2014-04-10

};

boolean FleUpd_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// ������Ƭ�����Ķ�
void uf_SendFile(newgimis * pMe);



#endif /* #ifndef GM_FILEUPLOAD_H */


// // �ϴ��ļ�  49 ��Ƶ�� 50 �����ļ��� 51 ��־
// if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,49,dwParam) )	
// {
// 	//						uf_RecLog(pMe,"PostEvent FLDUPD_EVT_UPLOAD");
// 	ISHELL_PostEvent(pMe->theApp.m_pIShell,pMe->theApp.clsID,FLDUPD_EVT_UPLOAD,0,0);
// }
