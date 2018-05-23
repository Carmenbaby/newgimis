#include "newgimis.bid"
#include "newgimis.h"

#include "gm_FileUpload.h"  // Add 2012-09-21
#include "gmTools.h"

#define SEGMENT_LEN		1			// �������εĳ��ȡ���Ҫ���ֽ���
#define EPAGESUM		5			//ÿһҳ��ʾ�������Ŀ
#define MAINMENU_ITEM_FPAGE   1	//��һҳ
#define MAINMENU_ITEM_BPAGE   2	//��һҳ

//��ʼ�� ��ȡ��ҳ��page �� ��ǰӦ����ʾ����count
static void Record_Page(Record_information *me);
//¼���ϴ���ʼ��׼��
static void RecordUpload_Init(Record_information *me);
//¼�����͹��̺ͽ��ܹ��̺���
static void Record_Send(Record_information *me);
static void Record_SetData(Record_information *me);
static void Record_SetData01(Record_information* me);
static void Record_Start(Record_information *me);
static void Record_GotResp(void *p);
static void Record_ReadBody(void *p);
static void Record_Status(void *p, WebStatus ws, void *pVal);
void Record_Afterpost(newgimis *pMe);
void Record_MonitorSendFile(Record_information* me);
void Record_SendFileResult(Record_information *me);	
void Record_SendFileResult_error(Record_information *me);

void newgimis_Send_Record(newgimis* pMe);
static void Record_removeFile(newgimis *pMe,char *filename);

//�����ر�
void Record_Close(newgimis *pMe);

boolean RecordUpload_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

void uf_AEPrint_waitting(newgimis * pMe,boolean bCallBack);


// uf_AEPrint_Sending  ��ʾ����������
void uf_AEPrint_waitting(newgimis * pMe,boolean bCallBack)
{   //���������� \u6c47\u603b\u6570\u636e\u4e2d
	AECHAR szText[] = {0x6c47,0x603b,0x6570,0x636e,0x4e2d,0x0000}; // L"������";
	
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

void newgimis_Send_Record(newgimis* pMe)
{
	Record_information *me;//��������¼������

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_Send_Record in");

	pMe->precord = NULL;
	me->m_pParent = NULL;	//������������me�Ŀռ�
	FREEIF(me);

	//���ݴ���
	me = (Record_information *)MALLOC(sizeof(Record_information));
	MEMSET(me,0,sizeof(Record_information));
	me->m_pParent = pMe;
	me->a = pMe->a;
	pMe->precord = (void *)me;

	//¼����ע��Ϣ
	ZEROAT(me->pText);
	STRTOWSTR(pMe->rData->recordInfo,me->pText,sizeof(me->pText));
    //ѡ�е�¼���ļ����ܶ���
	me->nPartsTotal = pMe->rData->nPartsTotal;
	//��¼���ļ������ַŵ�file
	ZEROAT(me->sFile);
	MEMCPY(me->sFile, pMe->rData->recordName, AEE_MAX_FILE_NAME);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_Send_Record  File = %s", me->sFile);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_Send_Record  info = %s", pMe->rData->recordInfo);

	me->cSendSts = '1';

	//FREEIF(rData);

	RecordUpload_HandleEvent(pMe,FLDUPD_EVT_START,0,0);
}

void newgimis_GetRecordInfo(newgimis* pMe)
{
	FileInfo pFileInfo ;	
	IFileMgr* pFileMgr = NULL;
	IFile	*pTempFile = NULL;

	int idx = 0;
	int flag = 0;

	char* sc_temp = NULL;
	char* sc_Start = NULL;
	char* sc_End = NULL;

	char sc_buf[1998] = {0};
	char strRoot[512] = {0};
	char lStr_Log[LOG_STR_LEN];

	char temp_info[2000];

	char *file_buf = NULL;
	file_buf = (char *) MALLOC (Record_Info_Size);
	while (file_buf == NULL)
	{	
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo file_buf malloc fail");
		file_buf = (char *) MALLOC (Record_Info_Size);
	}
	MEMSET( file_buf ,0x00,Record_Info_Size);

	sc_temp = (char *) MALLOC(25);
	MEMSET( sc_temp ,0x00,25);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecordInfo  in");

	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecordInfo in, ISHELL_CreateInstance err");
		return;
	}

	if (IFILEMGR_Test(pFileMgr, pMe->szinfotxtPath) == SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"info.txt exist");
		
		pTempFile = IFILEMGR_OpenFile(pFileMgr, pMe->szinfotxtPath, _OFM_READ);
		
		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &pFileInfo ) ) 
			{
				if (pFileInfo.dwSize > 0 )  // ˵�������Լ�����������
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pFileInfo.dwSize= %d",pFileInfo.dwSize);
					if (SUCCESS != IFILE_Seek(pTempFile,_SEEK_START,0)  )
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo IFILE_Seek fail return");
						goto ExitHere;
					}
				  //IFILE_Seek(pTempFile,_SEEK_START,0);
					if (0 == IFILE_Read( pTempFile, file_buf, pFileInfo.dwSize ) )
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo IFILE_Read fail return");
						goto ExitHere;
					}
			
					idx = ud_rDistToChar(pMe->rData->recordName,'/');
					STRCPY(sc_temp, pMe->rData->recordName + idx + 1);

					sc_Start = STRSTR(file_buf,sc_temp);	
					if (sc_Start == NULL)	//���info���Ҳ�����ȥinfo2Ѱ��
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo sc_Start fail");

						if (1 == newgimis_GetRecordInfo_2(pMe))
						{
							goto ExitHere; //info2Ҳû���ҵ���Ϣ
						}
					}
					else
					{
						sc_End = STRSTR(sc_Start,"\r\n");
						if (sc_End == NULL)
						{
							ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo sc_Start fail return");
							goto ExitHere;
						}

				    	MEMCPY(sc_buf, sc_Start+19, sc_End-sc_Start-19);
						MEMSET(pMe->rData->recordInfo,0x00,sizeof(pMe->rData->recordInfo));
						STRCPY(pMe->rData->recordInfo, sc_buf);
						
						if (STRLEN(pMe->rData->recordInfo) > 190)	//��ע�ַ�����200.���������ϴ�������
						{
							MEMSET(temp_info,0x00,sizeof(temp_info));
							MEMCPY(temp_info,pMe->rData->recordInfo,STRLEN(pMe->rData->recordInfo));
							MEMSET(pMe->rData->recordInfo,0x00,sizeof(pMe->rData->recordInfo));
							MEMCPY(pMe->rData->recordInfo,temp_info,187);
							STRCAT(pMe->rData->recordInfo,"...");
						}
					}

					flag = 1;
				}	
				else
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pFileInfo.dwSize = %d",pFileInfo.dwSize);
				}
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILE_GetInfo fail");	
			}
		}
	}


ExitHere:

	FREEIF(file_buf);
	FREEIF(sc_temp);

	if (pTempFile != NULL)
	{
		IFILE_Release( pTempFile );
		pTempFile = NULL;
	}
	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}
	
	if (flag == 1)
	{
		newgimis_Send_Record(pMe);
	}
	else
	{
		pMe->m_flag_Audio = 0;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����˳�¼���ϴ����� exit record upload");
		FREEIF(pMe->rData);
	}
}

int newgimis_GetRecordInfo_2(newgimis* pMe)
{
	FileInfo pFileInfo ;	
	IFileMgr* pFileMgr = NULL;
	IFile	*pTempFile = NULL;

	int idx = 0;
    int flag = 0;

	char* sc_temp = NULL;
	char* sc_Start = NULL;
	char* sc_End = NULL;

	char sc_buf[1998] = {0};
	char strRoot[512] = {0};
	char lStr_Log[LOG_STR_LEN];

	char temp_info[2000];

	char *file_buf = NULL;
	file_buf = (char *) MALLOC (Record_Info_Size);
	while (file_buf == NULL)
	{	
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo_2 file_buf malloc fail");
		file_buf = (char *) MALLOC (Record_Info_Size);
	}
	MEMSET( file_buf ,0x00,Record_Info_Size);

	sc_temp = (char *) MALLOC(25);
	MEMSET( sc_temp ,0x00,25);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecordInfo_2  in");

	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecordInfo_2 in, ISHELL_CreateInstance err");
		return 1;
	}

	if (IFILEMGR_Test(pFileMgr, pMe->szinfotxtPath_2) == SUCCESS  /*|| IFILEMGR_Test(pFileMgr, Record_Info_Name_NoDel) == SUCCESS*/)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"info2.txt exist");
		
		pTempFile = IFILEMGR_OpenFile(pFileMgr, pMe->szinfotxtPath_2, _OFM_READ);
		
		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &pFileInfo ) ) 
			{
				if (pFileInfo.dwSize > 0 )  // ˵�������Լ�����������
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pFileInfo.dwSize= %d",pFileInfo.dwSize);
					if (SUCCESS != IFILE_Seek(pTempFile,_SEEK_START,0)  )
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo_2 IFILE_Seek fail return");
						goto ExitHere;
					}
				  //IFILE_Seek(pTempFile,_SEEK_START,0);
					if (0 == IFILE_Read( pTempFile, file_buf, pFileInfo.dwSize ) )
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo_2 IFILE_Read fail return");
						goto ExitHere;
					}
			
					idx = ud_rDistToChar(pMe->rData->recordName,'/');
					STRCPY(sc_temp, pMe->rData->recordName + idx + 1);

					sc_Start = STRSTR(file_buf,sc_temp);	
					if (sc_Start == NULL)	//info2�����Ҳ��������ɾ����
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo_2 sc_Start fail");

						if (NULL != STRSTR(pMe->rData->recordName, ".qcp"))//������ļ���info��û���ҵ���Ӧ����Ϣ��ѡ��ɾ��
						{
							Record_removeFile(pMe,pMe->rData->recordName);
						}	

						goto ExitHere;
					}
					else
					{
						sc_End = STRSTR(sc_Start,"\r\n");
						if (sc_End == NULL)
						{
							ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecordInfo sc_Start fail return");
							goto ExitHere;
						}
				    	MEMCPY(sc_buf, sc_Start+19, sc_End-sc_Start-19);
					}


					MEMSET(pMe->rData->recordInfo,0x00,sizeof(pMe->rData->recordInfo));
					STRCPY(pMe->rData->recordInfo, sc_buf);

					if (STRLEN(pMe->rData->recordInfo) > 190)	//��ע�ַ�����200.���������ϴ�������
					{
						MEMSET(temp_info,0x00,sizeof(temp_info));
						MEMCPY(temp_info,pMe->rData->recordInfo,STRLEN(pMe->rData->recordInfo));
						MEMSET(pMe->rData->recordInfo,0x00,sizeof(pMe->rData->recordInfo));
						MEMCPY(pMe->rData->recordInfo,temp_info,187);
						STRCAT(pMe->rData->recordInfo,"...");
					}

					flag = 1;
				}	
				else
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pFileInfo.dwSize = %d",pFileInfo.dwSize);
				}
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILE_GetInfo fail");	
			}
		}
	}
	else
	{
		//info2Ҳû���ҵ���Ϣ��ɾ��¼��
		if (NULL != STRSTR(pMe->rData->recordName, ".qcp"))//������ļ���info��û���ҵ���Ӧ����Ϣ��ѡ��ɾ��
		{
			Record_removeFile(pMe,pMe->rData->recordName);
		}
	}


ExitHere:

	FREEIF(file_buf);
	FREEIF(sc_temp);

	if (pTempFile != NULL)
	{
		IFILE_Release( pTempFile );
		pTempFile = NULL;
	}
	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}

	if (flag == 1)
	{
		return 0;	//˵����info2����ȡ����ȷ��Ϣ�������Ѿ��浽pMe->rData->recordInfo
	}
	else{
		return 1;	//˵��û����ȡ����ȷ��Ϣ��������ʾ�����˳�������֮ǰ�Ѿ�ɾ��¼���ļ�
	}
}



//�ж��Ƿ���¼���ļ�����
//֮������������Ƿ����ֵ��ȷ��
//����д��ڵĻ�����¼�ļ������ֺʹ�С�������ܶ��������ں���ֶη���ʹ��
void newgimis_GetRecord(newgimis* pMe)
{
	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;

	int ret = 0;	
	char strDateTime[15];
	int flag = 0;
	char strRoot[512] = {0};
	char lStr_Log[LOG_STR_LEN];

	pMe->m_flag_Audio = 1;

	if (pMe->sAudRecPath == NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������¼��·��");
		return;
	}

	MEMSET(pMe->szinfotxtPath,0x00, sizeof(pMe->szinfotxtPath));
	SPRINTF(pMe->szinfotxtPath,AEEFS_ROOT_DIR"%sinfo.txt",pMe->sAudRecPath);

	MEMSET(pMe->szinfotxtPath_2,0x00, sizeof(pMe->szinfotxtPath_2));
	SPRINTF(pMe->szinfotxtPath_2,AEEFS_ROOT_DIR"%sinfo2.txt",pMe->sAudRecPath);

	SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sAudRecPath);

	FREEIF(pMe->rData);
	pMe->rData = (UploadRecord *)MALLOC(sizeof(UploadRecord));//���ռ�
	MEMSET(pMe->rData, 0x00, sizeof(UploadRecord));
	
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecord in, AUDIO RECORD PATH=%s",strRoot);
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecord in, AUDIO RECORD infotxt PATH=%s",pMe->szinfotxtPath);
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_GetRecord in, AUDIO RECORD info2txt PATH=%s",pMe->szinfotxtPath_2);
	
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecord in, ISHELL_CreateInstance err");
		return;
	}
	
	if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )  
	{
		int iAccess;
		iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	}
	
	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
	MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
	if( pFileInfo )   
	{	
		while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
		{  // while -- 1
			FileInfo * pInfo;	
			pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
			
			if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )  
			{
				if (pInfo->dwCreationDate != 0)
				{
					if (pMe->m_del_Audio == 0)
					{
						if (STRCMP(pInfo->szName, pMe->szinfotxtPath) == 0  || STRCMP(pInfo->szName, pMe->szinfotxtPath_2) == 0)
						{
							FREEIF( pInfo );
							continue;
						}
					}
					else
					{
						//����Ҫɾ��¼����ʱ��,��Ҫ�ж�info�ͷ��ͺ���ļ������ý���
						if (STRCMP(pInfo->szName, pMe->szinfotxtPath) == 0  || STRCMP(pInfo->szName, pMe->szinfotxtPath_2) == 0 || STRSTR(pInfo->szName, "x.") != NULL) 
						{
							FREEIF( pInfo );
							continue;
						}
					}

					pMe->rData->recordSize = pInfo->dwSize;		//��С
	
					STRCPY(pMe->rData->recordName, pInfo->szName/*sc_tmp*/);	//¼���ļ���

					pMe->rData->nPartsTotal = (uint8)(pInfo->dwSize / MAX_FILE_BYTES_POST);				// �ܶ���

					if (pInfo->dwSize == 0)	//�ڴ���T35����¼���ļ�Ϊ�յ����
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"¼���ļ���СΪ�գ�Ĭ��ɾ��");

						if (pInfo){
							FREEIF( pInfo );
						}
						if (pFileInfo){
							FREEIF( pFileInfo );
						}
						if (pFileMgr != NULL){
							IFILEMGR_Release( pFileMgr );
							pFileMgr = NULL;
						}

						Record_removeFile(pMe,pMe->rData->recordName);

						FREEIF(pMe->rData);	//¼���ļ��������rdata�У�ʹ�������ͷ�

						pMe->m_flag_Audio = 0;	//�����´��������ٴν���
						return;
					}

					if ( (pInfo->dwSize % MAX_FILE_BYTES_POST) != 0 )
					{
						pMe->rData->nPartsTotal += 1;
					}

					if ( STRSTR(pMe->rData->recordName, ".qcp") == NULL )
					{
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����¼���ļ�qcp�������쳣");
						if (pInfo){
							FREEIF( pInfo );
						}
						if (pFileInfo){
							FREEIF( pFileInfo );
						}
						if (pFileMgr != NULL){
							IFILEMGR_Release( pFileMgr );
							pFileMgr = NULL;
						}
						FREEIF(pMe->rData);
						pMe->m_flag_Audio = 0;	//�����´��������ٴν���
						return;
					}

					flag = 1;
				}
			}
			FREEIF( pInfo );
			break;//��������
		}  // while -- 1
		//FREEIF(sc_tmp);
		FREEIF( pFileInfo );	
	}	// if -- pFileInfo	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}

	if (flag == 1)
	{
		newgimis_GetRecordInfo(pMe);//�ó���Ϣ�浽
	}
	else
	{
		pMe->m_flag_Audio = 0;	//�����´��������ٴν���
		FREEIF(pMe->rData);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_GetRecord never audio file");
	}

	return;
}




static void RecordUpload_Init(Record_information *me)
{
	//1����ʼ�����͵�ַURL 2����ʼ��file 3����ʼ��web + source
	int idx;
	char sFormat[250];
	int nErr = SUCCESS;

	newgimis *pMe = me->m_pParent;

	MEMSET(sFormat,0,sizeof(sFormat));
    idx = ud_DistToChar(pMe->strCenterIp,'/');  
	if ( idx >= 0 )
	{
		MEMCPY(sFormat, pMe->strCenterIp, idx); 
	}
	else
	{
		STRCPY(sFormat, pMe->strCenterIp);
	}
	SPRINTF(me->pURL,"http://%s/FileUploadServlet",sFormat);
	if (STRLEN(pMe->MyMobileNo) > 0) 
	{
		STRCAT(me->pURL,"?tel=");
		STRCAT(me->pURL,pMe->MyMobileNo);
		STRCAT(me->pURL,"&imsi=");
		STRCAT(me->pURL,pMe->szMobileID);
	}

	me->nPartNum = 0;					// ���ڴ��ڼ��Σ�1 ��1�Σ�2 ��2�Σ�����������  

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"RecordUpload_Init in url=%s",me->pURL);
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&me->pFileMgr);
	if(SUCCESS != nErr)  	
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FileUpload AEECLSID_FILEMGR error = %d",nErr);
	}

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);			// ---- Web�ӿ�
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "RecordUpload_Init IWeb err=%d", nErr);
	}

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&me->pisu);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"RecordUpload_Init ISourceUtil err=%d", nErr);
	}

	//////////////////////////////////////////////////////////////////////////
	if (SUCCESS != IFILEMGR_Test(me->pFileMgr, me->sFile))
	{
		uf_RecLog(me->m_pParent, "RecordUpload_Init IFILEMGR_Test faile");
		return;
	}
	
	me->pFileUpload = IFILEMGR_OpenFile(me->pFileMgr, me->sFile, _OFM_READ);
	if (0 == me->pFileUpload)
	{
		uf_RecLog(me->m_pParent, "RecordUpload_Init IFILEMGR_OpenFile faile");
		return;
	}
	
    if ( SUCCESS != IFILE_GetInfo( me->pFileUpload, &me->oFileInfo ) ) 
	{
		uf_RecLog(me->m_pParent, "RecordUpload_Init IFILE_GetInfo faile");
		return;
	}
	
	uf_RecLog(pMe,"RecordUpload_Init SEND FASONGZHONG");
	//uf_AEPrint_Sending(pMe, FALSE);
	//���͹��̲����л����棬 ������ɺ�����쳣��Ҫ�ͷſ��ƽ���
	//pMe->m_WitchPag = 100;//100Ϊ���ͽ��档

	//���͡�����
	Record_Send(me);
}

static void Record_Send(Record_information *me)
{
	// ���÷�������
	Record_SetData(me);
	if (me->nState == '0')
	{

		return;
	}

	// ��ʼWeb����
	Record_Start(me);
}

static void Record_SetData(Record_information *me)
{
	int32 iLen = 0;
	char sLogBuf[256];
	char sFormat[50];
	
	SPRINTF(sLogBuf,"Record_SetData in,me->cSendSts=%d[%c]",me->cSendSts,me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	// ��һ�η���ʧ�ܣ��ط�
	if (me->cSendSts == '0') 
	{		
		me->nState = SUCCESS;
		uf_RecLog(me->m_pParent,"Record_SetData shang yi duan fail");
		goto ExitHere;
	}
	
	// ���ͳɹ���������һ��
	if (me->cSendSts == '1')
	{ // if -- 1
		
		FREEIF(me->pszPostData);
		me->iFileRead = 0;
		
		// ���õ�1,2��������
		Record_SetData01(me);
		
		//  ���õ�3��������
		// Allocate enough memory to read the full text into memory
		if ( (uint32)(me->nHasRead+MAX_FILE_BYTES_POST) > me->oFileInfo.dwSize )
			iLen = me->oFileInfo.dwSize - me->nHasRead;
		else
			iLen = MAX_FILE_BYTES_POST;
		me->nPartNum += 1;
		me->pszPostData = REALLOC(me->pszPostData,(iLen+SEGMENT_LEN+SEGMENT_LEN)+me->iFileRead+1 );

		SPRINTF(sLogBuf,"---------me->nPartNum = %d",me->nPartNum);
		uf_RecLog(me->m_pParent,sLogBuf);
		
		MEMSET(me->pszPostData+me->iFileRead,0,(iLen+SEGMENT_LEN+SEGMENT_LEN+1));
		MEMSET(sLogBuf,0,sizeof(sLogBuf));
		{
			char sTmp[5];
			SPRINTF(sTmp,"%c",me->pszPostData[18]);		// ��18Ϊ�ε��ֽ���
			STRCPY(sFormat,"%0");
			STRCAT(sFormat,sTmp);
			STRCAT(sFormat,"d%0");
			STRCAT(sFormat,sTmp);
			STRCAT(sFormat,"d");
		}
		SPRINTF(sLogBuf,sFormat,me->nPartNum,me->nPartsTotal);
        MEMCPY(me->pszPostData+me->iFileRead,sLogBuf,STRLEN(sLogBuf));
		me->iFileRead += STRLEN(sLogBuf);
		// test by ---- 2012-09-21
		// 		uf_RecLog(me->m_pParent,"test=");
		// 		uf_RecLog(me->m_pParent,me->pszPostData);
		// test end
		IFILE_Seek(me->pFileUpload,_SEEK_CURRENT ,0);
		iLen = IFILE_Read( me->pFileUpload, me->pszPostData+me->iFileRead, iLen );
		
		SPRINTF(sLogBuf,"FleUpd_SetData in, File Len=%d,Has Read=%d,IFILE_Read return=%d",
			me->oFileInfo.dwSize,me->nHasRead,iLen);
		uf_RecLog(me->m_pParent,sLogBuf);
		
		if ( iLen <= 0 ) 
		{
			FREEIF( me->pszPostData );
			me->iFileRead = 0;
			//888
			goto ExitHere;
		}
		
		me->iFileRead += iLen;
		me->nHasRead += iLen;
		
		me->nState = SUCCESS;
		goto ExitHere;
	}	// if -- 1
	
	me->nState = EFAILED;
	
ExitHere:
	return;
}

// ===================================================== 
// �����ϴ��ĵ�1,2 ������
//   ��1��������
//       �ļ�����ʱ��(14)+Uunicode�ַ�������(3)+�ļ������ֽ���(1)+�ε��ֽ���(1) 
//   ��2���֣��ַ�������+�ļ�����(�ļ���չ��) ,�ַ���������Uunicode�ַ�    
//       �ַ���������Uunicode�ַ�����
//  ����������֮һ��2012091109121300531����ABCmp4
//  ���� �ļ�����ʱ�� 2012-09-11 09:12:13��5��Uunicode�ַ�������ABC�����ļ�����ռ3���ֽڣ�mp4��ʽ������Ϣռ1���ֽ�
//  ����������֮����2012091109121300031mp4
//  ���� �ļ�����ʱ�� 2012-09-11 09:12:13��0��Uunicode�ַ����ļ�����ռ3���ֽڣ�mp4��ʽ������Ϣռ1���ֽ�
// ===================================================== 
static void Record_SetData01(Record_information* me)
{
	char sLogBuf[/*128+TEXT_MAXSIZE*2*/4096];
	char sDateTime[15];						// �ļ��Ĵ���ʱ��
	char sExpName[10];						// �ļ�����չ��	
	char sFormat[50];
	int  nPartLen = 0;						// �ε��ֽ���
	int  nTextLen = 0;						// Uunicode�ַ����ֽ���
	int idx;
	
	SPRINTF(sLogBuf,"Record_SetData01 in filename=%s",me->oFileInfo.szName);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	// �ļ�����ʱ��
	// ¼���ļ�
	uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'1');

	idx = ud_rDistToChar(me->oFileInfo.szName,'.');
	if (idx == -1) 
	{
		SPRINTF(sLogBuf,"ud_rDistToChar return -1, File[%s]",me->oFileInfo.szName);
		uf_RecLog(me->m_pParent,sLogBuf);
		me->nState = EFAILED;
		return;
	}
	// �ļ���չ��
	MEMSET(sExpName,0,sizeof(sExpName));
	SPRINTF(sExpName,"%s",(me->oFileInfo.szName+idx+1));
	
	// �ε��ֽ���
	SPRINTF(sLogBuf,"%d",me->nPartsTotal);
	nPartLen = STRLEN(sLogBuf);
	
	// Uunicode�ַ����ֽ���
	nTextLen = WSTRLEN(me->pText) * sizeof(AECHAR);
	
	// ��1��������
	MEMSET(sLogBuf,0,sizeof(sLogBuf));
	MEMSET(sFormat,0,sizeof(sFormat));
	SPRINTF(sLogBuf,"%d",SEGMENT_LEN);
	STRCPY(sFormat,"%s%03d%01d%0"); // STRCPY(sFormat,"%s000%01d%0");
	STRCAT(sFormat,sLogBuf);
	STRCAT(sFormat,"d");
	SPRINTF(sLogBuf,sFormat,sDateTime,nTextLen,STRLEN(sExpName),nPartLen);		//	SPRINTF(sLogBuf,"%s000%01d%0"SEGMENT_LENd"%s;",sDateTime,STRLEN(sExpName),nPartLen,sExpName);
	me->iFileRead = STRLEN(sLogBuf);
	uf_RecLog(me->m_pParent, sLogBuf);
	
	// ��2��������
	MEMSET(sLogBuf+me->iFileRead,0,(nTextLen+1));
	MEMCPY(sLogBuf+me->iFileRead,me->pText,nTextLen);
	me->iFileRead += nTextLen;
	
    idx = STRLEN(sExpName);
	MEMSET(sLogBuf+me->iFileRead,0,(idx+1));
	MEMCPY(sLogBuf+me->iFileRead,sExpName,idx);
	me->iFileRead += idx;
	
	
	me->pszPostData = MALLOC( me->iFileRead + 1 );
	MEMSET(me->pszPostData,0,me->iFileRead+1);
	MEMCPY(me->pszPostData,sLogBuf,me->iFileRead);
	
	
	nTextLen = me->iFileRead;
// 	{
// 		char sBuf[500];
// 		MEMSET(sBuf,0,sizeof(sBuf));
// 		SPRINTF(sBuf,"len=%d,  text AECHAR=",nTextLen);
// 		{
// 			int idx;
// 			for(idx=0;idx<nTextLen;idx++)
// 			{
// 				char cTmp = me->pszPostData[idx];
// 				char sTmp[5];
// 				SPRINTF(sTmp,"%d,",cTmp);
// 				STRCAT(sBuf,sTmp);
// 			}
// 		}
// 		uf_RecLog(me->m_pParent,sBuf);
// 	}
}

static void Record_Start(Record_information *me)
{
	WebOpt awo[4]; 
	
	//	cgimis * pMe = pwa->m_pParent;
	int iLen = 0;
	
    // Add Options
	awo[0].nId  = WEBOPT_CONNECTTIMEOUT;
    awo[0].pVal = (void *)20000;			// 2013-03-27  5000 
	// 2.6.2D2   awo[1].nId  = WEBOPT_FLAGS;
	// 2.6.2D2   awo[1].pVal = (void *)WEBREQUEST_NOWAITCONN;
    awo[1].nId  = WEBOPT_PROXYSPEC;
    awo[1].pVal = (void *)"http:///http://10.0.0.200:80";
    awo[2].nId  = WEBOPT_END;
	IWEB_AddOpt(me->pWeb, awo);
	
	me->dwPostOvertime = 0;
	
	if (me->pipPostData != NULL) {
		IPEEK_Release(me->pipPostData);
		me->pipPostData = NULL;
	}
	
	// if there's post data, construct a stream for IWeb to consume
	if ((char *)0 != me->pszPostData) {
		iLen = me->iFileRead;
		ISOURCEUTIL_PeekFromMemory(me->pisu, me->pszPostData, iLen, 0, 0,&me->pipPostData);
	}
	
	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if ((IPeek *)0 != me->pipPostData) {
		
		uf_RecLog(me->m_pParent,"Record_Start send file begin");
		
		// initialize the callback, where I'll be called when the request  completes
		CALLBACK_Init(&me->webcb, Record_GotResp, me);
		
		IWEB_GetResponse(me->pWeb,
			(me->pWeb, &me->piWResp, &me->webcb, me->pURL, 
			WEBOPT_HANDLERDATA, me,
			WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, NULL,   
			WEBOPT_STATUSHANDLER, Record_Status,  
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, me->pipPostData,
			WEBOPT_CONTENTLENGTH, iLen,		
			WEBOPT_END));
		
	}
	
}

static void Record_Status(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	Record_information  *me;
	
	if (p == NULL)
		return;
	
	me = (Record_information *)p;
	//    pMe = pwa->m_pParent;
	
	MEMSET(lStr_Sts,0,sizeof(lStr_Sts));
	switch (ws) 
	{
	case WEBS_CANCELLED:
		STRCPY(lStr_Sts, "** cancelled...");
		break;
	case WEBS_GETHOSTBYNAME:
		STRCPY(lStr_Sts, "** finding host...");
		break;          
	case WEBS_CONNECT:
		STRCPY(lStr_Sts, "** connecting...");
		break;
	case WEBS_SENDREQUEST:
		me->dwPostOvertime = GETTIMEMS();
		ISHELL_SetTimer(me->a.m_pIShell, 60 * 1000, (PFNNOTIFY)Record_MonitorSendFile, me);
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->dwPostOvertime == 0)
		{			//  Add 2013-09-23
			me->dwPostOvertime = GETTIMEMS();														
			ISHELL_SetTimer(me->a.m_pIShell, 60 * 1000, (PFNNOTIFY)Record_MonitorSendFile, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}
	
	uf_RecLog(me->m_pParent,lStr_Sts);
}


// ********************************************************************************************
// ����ļ������Ƿ�ʱ
// **********************************************************************************************/
void Record_MonitorSendFile(Record_information* me)
{
	char sLogBuf[128];
	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��
	
	SPRINTF(sLogBuf,"Record_MonitorSendFile in,dwPostOvertime=%d",me->dwPostOvertime);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	me->m_pParent->m_flag_Audio = 0;

	if ( me->dwPostOvertime == 0)
	{
		return;
	}
	
	ISHELL_SetTimer(me->a.m_pIShell, 100, (PFNNOTIFY)Record_Afterpost, me->m_pParent);
}



//static void FleUpd_Close(FileUpload* me)
void Record_Close(newgimis *pMe)
{
	Record_information* me;
	me = (Record_information*)pMe->precord;
	
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close in");
	
	if (me->pFileUpload != NULL) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close in,pFileUpload!=NULL");
		IFILE_Release(me->pFileUpload);
		me->pFileUpload = NULL;
	}
	
	if (me->pFileMgr != NULL) {
		IFILEMGR_Release(me->pFileMgr);
		me->pFileMgr = NULL;
	}
	
	if (me->pszPostData != NULL) {
		FREE( me->pszPostData );
		me->pszPostData = NULL;
		me->iFileRead = 0;
	}
	
	if (me->pipPostData != NULL)
	{
		IPEEK_Release(me->pipPostData);
		me->pipPostData = NULL;
	}
	
	if (me->pSource != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close in,pSource != NULL");
		ISOURCE_Release(me->pSource);
		me->pSource = NULL;
	}
	
	if (me->piWResp != NULL)
	{
		uint32 nErr = IWEBRESP_Release(me->piWResp);		// 2.6.4D2
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close in,clear piWResp,return=%d,n=%d",nErr);
		me->piWResp = NULL;
	}
	
	CALLBACK_Cancel(&me->webcb);
	
	if (me->pisu != NULL) {
		ISOURCEUTIL_Release(me->pisu);
		me->pisu = NULL;
	}
	
	if(me->pWeb)	{
		int i,n;			
		n = 0;
		i=IWEB_Release(me->pWeb);
		while (i>0) {
			n++;
			i=IWEB_Release(me->pWeb);
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close, IWEB_Release=%d,n=%d",i,n);
		me->pWeb = 0;
	}
	
	me->nState = SUCCESS;
	
/*	pMe->precord = 0;*/			
//  	me->m_pParent = 0;
//  	FREEIF(me);
	
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_Close out");
	
	return;
}

static void Record_GotResp(void *p)
{
	char sLogBuf[257];
	int nLen =0;
	Record_information *me = (Record_information *)p;

	WebRespInfo *pInfo;
	uf_RecLog(me->m_pParent,"Record_GotResp in");

	pInfo = IWEBRESP_GetInfo(me->piWResp);
	
	SPRINTF(sLogBuf, "Record_GotResp response, code: %d",pInfo->nCode);
	nLen = STRLEN(sLogBuf);
	uf_RecLog(me->m_pParent,sLogBuf);
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
	{  
		//Record_SendFileResult_error(me); // ��ʾʧ�ܵĽ���
		ISHELL_SetTimer(me->a.m_pIShell, 100, (PFNNOTIFY)Record_Afterpost, me->m_pParent);//////////////////////////////////////////////////////////////////////////
		return;
	}
	
	if(0 != pInfo->pisMessage) 
	{
		SPRINTF(sLogBuf,"Record_GotResp,Got Message, time:%u",(GETTIMEMS()-me->dwPostOvertime));
		uf_RecLog(me->m_pParent, sLogBuf);
		if(0 == pInfo->lContentLength) 
		{
			return; 
		}
	      
		me->dwPostOvertime = 0;				// ���յ�Ӧ��
		
		me->pSource = pInfo->pisMessage;
		ISOURCE_AddRef(me->pSource);				// ---- ��Read Body���õ�
		// 2.6.2D2		IWEBRESP_Release(me->piWResp);
		// 2.6.2D2		me->piWResp = 0;
		{
			newgimis * pMe; 
			int n=0;			// 2.6.4D2
			uint32 nErr = IWEBRESP_Release(me->piWResp);
			pMe=me->m_pParent;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in,clear piWResp,return=%d,n=%d",nErr,n);
			me->piWResp = NULL;
		}
		
		CALLBACK_Cancel(&me->webcb);
		CALLBACK_Init(&me->webcb, Record_ReadBody, me);	
		Record_ReadBody(me);
	}
}


// ==========================================================
// ��ȡ���Ķ˷��ص���Ϣ
// ==========================================================
static void Record_ReadBody(void *p)
{
	Record_information* me = (Record_information* )p;
	int32 nRead;
	char sBodybuf[10];			// ���Ķ�ֻӦ��1������0���� ��Ҫ̫��
	char sLogBuf[256];

	uf_RecLog(me->m_pParent,"Record_ReadBody in");
	
	MEMSET(sBodybuf,0,sizeof(sBodybuf));
	nRead = ISOURCE_Read(me->pSource, sBodybuf, sizeof(sBodybuf)-1 );
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	if (nRead>0)		// ---- ��������
	{
		int iLen = (nRead>sizeof(me->sBodyBuf))? sizeof(me->sBodyBuf) : nRead;
		if (me->nBytes == 0)
		{
			MEMSET(me->sBodyBuf,0,sizeof(me->sBodyBuf));
			MEMCPY(me->sBodyBuf,sBodybuf,iLen);
		}
// ֻҪ��1���ַ� 		else 
//			   MEMCPY(me->sBodyBuf+me->nBytes,sBodybuf,iLen);
//   FleUpd_ReadBody Data Reading:1,sBodybuf = 0
		me->nBytes += nRead;
		SPRINTF(sLogBuf, "Record_ReadBody Data Reading:%d,sBodybuf = %s", me->nBytes,sBodybuf);
		uf_RecLog(me->m_pParent, sLogBuf);
	}
	
	if (!ISOURCE_Exhausted(nRead))
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	ISOURCE_Release(me->pSource);
	me->pSource = NULL;
	CALLBACK_Cancel(&me->webcb);

	SPRINTF(sLogBuf,"Record_ReadBody, me->sBodyBuf=%s",me->sBodyBuf);
	uf_RecLog(me->m_pParent,sLogBuf);

	me->cSendSts =  ( STRLEN(me->sBodyBuf) > 1 ) ? 'E' : me->sBodyBuf[0];		// 'E' ���������
	me->nBytes = 0;

	SPRINTF(sLogBuf,"Record_ReadBody, me->cSendSts=%c",me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);

	// ���ݷ��صĽ������
	switch (me->cSendSts)
	{
	case '1':
		
		if (me->nPartNum == me->nPartsTotal)
		{	// �Ѿ����
			uf_RecLog(me->m_pParent,"Record_ReadBody Send file finish");
			
			Record_SendFileResult(me);	
		}
		else
		{
			me->cSendSts = '1';			// ����һ��
			uf_RecLog(me->m_pParent,"Record_ReadBody send next leg");
	//		ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, FLDUPD_EVT_SEND,0, 0);  //�����¼�û��������
			Record_Send(me);
		}
		break;
	case '0':
		me->cSendSts = '0';			// �ط�
		Record_Send(me);
		break;
	default:
// 		SPRINTF(sLogBuf,"Record_ReadBody Send file error, receive=%s",me->sBodyBuf);
// 		uf_RecLog(me->m_pParent,sLogBuf);
// 		me->cSendSts = 'A';	
// 
// 		Record_SendFileResult(me);
		Record_Close(me->m_pParent);//20170116
		break;
	}

}
// ==========================================================
// ��ʾ�ϴ��ļ��Ľ��  ͨ�Ź��ϵĽ��
// ==========================================================
void Record_SendFileResult_error(Record_information *me)
{
	// ��ʾ���ͽ����Ϣ 
	// \u53d1\u9001\u5931\u8d25\uff0c\u624b\u673a\u6b20\u8d39\u6216\u6545\u969c  ����ʧ�ܣ��ֻ�Ƿ�ѻ����
	AECHAR szFailure[] = {0x53d1,0x9001,0x5931,0x8d25,0xff0c,0x624b,0x673a,0x6b20,0x8d39,0x6216,0x6545,0x969c,0x0000};	// ����ʧ��
	//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6
	
				// ��ʾ����Ƶ�ļ��ɹ� 
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);
	
	return;
}
// ==========================================================
// ��ʾ�ϴ��ļ��Ľ��
// ==========================================================
void Record_SendFileResult(Record_information *me)
{
	//ɾ���ļ�
	IFileMgr* pFileMgr = NULL;
	newgimis *pMe = me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Record_SendFileResult in");
	
	Record_Close(pMe);

	if (me->cSendSts == '1')
	{
		if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
		{
			return;
		}
		
		if (pMe->m_del_Audio == 0)
		{
			if ( IFILEMGR_Remove(pFileMgr, me->sFile) == SUCCESS ) 
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete audio %s",me->sFile);
			}
		}
		else
		{
			char s_Name[MAX_FILE_NAME + 1] = {0};
			char s_buf[MAX_FILE_NAME + 1] = {0};

			int idx = ud_rDistToChar(me->sFile,'.');
			STRNCPY(s_buf, me->sFile, idx-1);
			SPRINTF(s_Name,"%sx%s", s_buf, me->sFile+idx);
			
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Rename %s ",s_buf); 
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Rename %s ",s_Name); 
			
			if ( IFILEMGR_Rename(pFileMgr, me->sFile, s_Name) == SUCCESS ) 
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Rename %s success",s_Name); 
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Rename %s fail",me->sFile);
			}
		}


		if (pFileMgr)
		{
			IFILEMGR_Release(pFileMgr);
			pFileMgr = NULL;
		}
	}

	return;
}


/********************************************************************************************
// ������ɺ�Ķ���
// �������
  FileUpload * pMe	: 
********************************************************************************************/
void Record_Afterpost(newgimis *pMe)
{
	Record_Close(pMe);	//�ر���HTTP����Դ ���ڽ�����ת���ͷ�������	
	return;
}



boolean RecordUpload_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	Record_information *me = pMe->precord;

	switch (eCode)
	{
    case FLDUPD_EVT_START:
		{
			RecordUpload_Init(me);			
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_HandleEvent in FLDUPD_EVT_START, FleUpdApp is null");
			return TRUE;
		}
		
		break;
    default:
		break;
    }
	
     return(FALSE);
}

static void Record_removeFile(newgimis *pMe,char *filename)
{
	IFileMgr* pFileMgr = NULL;
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		return;
	}

	if ( IFILEMGR_Remove(pFileMgr, filename) == SUCCESS ) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete audio %s",filename);
	}
	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
	return;
}