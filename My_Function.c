#include "My_Function.h"

#include "gm_FileUpload.h"
#include "gmTools.h"

/****************************************************************************************
����GPS���ݵı���Ͷ�ȡ
	��󱣴���Ŀ��m_RofflineData_Num
	��˳��1~maxѭ��д��
	��ȡ��ʱ�򰴱���ʱ�� ˳��
	��ʼ��ȡ
*****************************************************************************************/
void File_Write_OfflineData_Gps(newgimis *pMe, char* strData)
{
	boolean flg = 0; //�ļ������ڣ������������ļ����1 

	int lInt_Len;	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;

	int nId_X = 0;	//д���x��
	int nId_Y = 0;	//��ȡ��y��
	
	char chID_X[4]; //����999��
	char chID_Y[4];  

	ZEROAT(chID_X);
	ZEROAT(chID_Y);

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		//My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"open success");
		if (IFILEMGR_Test(pFileMgr, FILE_NAME) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, FILE_NAME, _OFM_READWRITE);
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, FILE_NAME, _OFM_CREATE);
			flg  = 1;
		}
	}
	else
	{
	//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"create offline file fail");
		return;
	}

	if (pTempFile != NULL)
	{
		if (1 == flg)//��һ�δ����ļ��򿪣������һ������
		{
			
			nId_X = 1;
			nId_Y = 1;

			SPRINTF(chID_X, "%d", nId_X);
			SPRINTF(chID_Y, "%d", nId_Y);
			
			
			//д�루1, 1��
			IFILE_Seek(pTempFile,_SEEK_START,0);	
			IFILE_Write(pTempFile, chID_X, sizeof(chID_X));
			IFILE_Write(pTempFile, chID_Y, sizeof(chID_Y));
		//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write:  x = %s  y = %s", chID_X, chID_Y);

			IFILE_Write(pTempFile, strData, MAX_MESSAGE_LEN); //д���һ������ 
		//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write: %s", strData);
			
		}
		else
		{	
			//�����ļ��Ѵ��ڣ���ȡ�ļ�ͷ��nID_X��nID_Y��
			IFILE_Seek(pTempFile,_SEEK_START,0);
			lInt_Len = IFILE_Read( pTempFile, chID_X, sizeof(chID_X) );
			lInt_Len = IFILE_Read( pTempFile, chID_Y, sizeof(chID_Y) );


			nId_X = My_StrTOint(chID_X);
			nId_Y = My_StrTOint(chID_Y);
//			My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Read:  x = %d  y = %d", nId_X, nId_Y);

			if (0 == nId_X, 0 == nId_Y) //�����ļ�ȫ���������ˣ����¿�ʼ�����һ��
			{
				nId_X = 1;
				nId_Y = 1;
				
				SPRINTF(chID_X, "%d", nId_X);
				SPRINTF(chID_Y, "%d", nId_Y);
				
				
				//д�루1, 1��
				IFILE_Seek(pTempFile,_SEEK_START,0);	
				IFILE_Write(pTempFile, chID_X, sizeof(chID_X));
				IFILE_Write(pTempFile, chID_Y, sizeof(chID_Y));
			//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write:  x = %s  y = %s", chID_X, chID_Y);

				//д���һ������ 
				IFILE_Write(pTempFile, strData, MAX_MESSAGE_LEN); 
			//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write: %s", strData);
			}
			else
			{//�Ѵ��������ļ���׷��or����
				nId_X++;
				if(nId_X > pMe->m_RofflineData_Num) //һ�ִ����ˣ����ǵ�һ�����¿�ʼ��
				{
					nId_X = 1;
				}

				if (nId_X == nId_Y)
				{
					nId_Y++;
					if (nId_Y > pMe->m_RofflineData_Num)//һ�ֶ�ȡ���ˣ��ӵ�һ�����¿�ʼ��ȡ
					{
						nId_Y = 1;
					}	
				}				
				//д�루x, y��
				SPRINTF(chID_X, "%d", nId_X);
				SPRINTF(chID_Y, "%d", nId_Y);

				IFILE_Seek(pTempFile,_SEEK_START,0);	
				IFILE_Write(pTempFile, chID_X, sizeof(chID_X));
				IFILE_Write(pTempFile, chID_Y, sizeof(chID_Y));
			//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write:  x = %s  y = %s", chID_X, chID_Y);

				// ����д�뵽��xλ��
				IFILE_Seek(pTempFile,_SEEK_START, sizeof(chID_X) + sizeof(chID_Y) + MAX_MESSAGE_LEN * (nId_X - 1));
				IFILE_Write(pTempFile, strData, MAX_MESSAGE_LEN); 
			//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write: %s", strData);
			}
		}
	}

   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );
}



//��ȡ��������Offline data
int File_Get_OfflineData_Gps(newgimis *pMe, char **strData)  //��ȡ�������� �ɹ� ���� > 0  ����ڼ����ļ�  ʧ�ܣ����������ݣ����� 0
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
    FileInfo fiInfo;		// �ļ���Ϣ
	int lInt_Len = 0;

	int nId_X = 0;	//д���x��
	int nId_Y = 0;	//��ȡ��y��
	
	char chID_X[4]; //����999��
	char chID_Y[4];  
	
	ZEROAT(chID_X);
	ZEROAT(chID_Y);

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{

		if (IFILEMGR_Test(pFileMgr, FILE_NAME) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, FILE_NAME, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
		}
		else
		{
			//��������������
			return 0;
		}

		if (pTempFile != NULL)
		{

			IFILE_Seek(pTempFile,_SEEK_START,0);
			lInt_Len = IFILE_Read( pTempFile, chID_X, sizeof(chID_X) );
			
			lInt_Len = IFILE_Read( pTempFile, chID_Y, sizeof(chID_Y) );

			nId_X = My_StrTOint(chID_X);
			nId_Y = My_StrTOint(chID_Y);
		//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Read:  x = %d  y = %d", nId_X, nId_Y);

			if ( 0 == nId_X && 0 == nId_Y)
			{
				//û�� ��Ҫ���͵�����
				//uf(pMe->a.m_pIShell,'1', &(pMe->nInd),"no data...");
				return 0;
			}else{
				
				//��ȡ��nId_Y������
				IFILE_Seek(pTempFile,_SEEK_START, sizeof(chID_X) + sizeof(chID_Y) + MAX_MESSAGE_LEN * (nId_Y - 1));
				IFILE_Read(pTempFile, *strData, MAX_MESSAGE_LEN); 
			//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Read: %s", *strData);

			
				if (nId_X == nId_Y) //ȫ����ȡ��������
				{
					nId_X = 0;
					nId_Y = 0;
					
					SPRINTF(chID_X, "%d", nId_X);
					SPRINTF(chID_Y, "%d", nId_Y);
					
					
					//д�루0, 0��
					IFILE_Seek(pTempFile,_SEEK_START,0);	
					IFILE_Write(pTempFile, chID_X, sizeof(chID_X));
					IFILE_Write(pTempFile, chID_Y, sizeof(chID_Y));
				//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write:  x = %s  y = %s", chID_X, chID_Y);
				}else{	
					nId_Y++;
					if (nId_Y >pMe->m_RofflineData_Num)
					{
						nId_Y = 1;
					}

					//д�루nId_X, nId_Y��
					SPRINTF(chID_X, "%d", nId_X);
					SPRINTF(chID_Y, "%d", nId_Y);

					IFILE_Seek(pTempFile,_SEEK_START,0);	
					IFILE_Write(pTempFile, chID_X, sizeof(chID_X));
					IFILE_Write(pTempFile, chID_Y, sizeof(chID_Y));
			//		My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"IFILE_Write:  x = %s  y = %s", chID_X, chID_Y);
				}
			}

		}
		
	}


	if (pTempFile != NULL)
		IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );

	pFileMgr = NULL;
	pTempFile = NULL;
	return 1; //��������
}





////////////////////////��������//////////////////////////////////////
uint16 Leap_Year(uint16 m_Year)
{
	return  ( ( ( m_Year % 400 == 0) || ( ( m_Year % 4 == 0) && ( m_Year != 0) ) ) ? 1 : 0 );
}

uint16 Judge(calDate d1)
{
	uint16 a_Month[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	return     ((d1.year > 0 && d1.year <= 9999) && (d1.month > 0 && d1.month <= 12) &&(d1.day > 0 &&  (((d1.month == 2) && (d1.day < a_Month[d1.month - 1] + Leap_Year(d1.year)))||((d1.month != 2) && (d1.day < a_Month[d1.month - 1]))))) ? 1 : 0;
}

void Now_To_Fronter(char *file_name,JulianType m_Date, int gap)
{
	calDate d1 = {0};
	int sum, sum_bak;
	int temp;

	uint16 a_Month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	
	d1.year  = m_Date.wYear;
	d1.month = m_Date.wMonth;
	d1.day   = m_Date.wDay;
	sum = gap;
	
	if( (sum < 0) && (sum > -3649270))
	{
		while(sum < -365)
		{
			if(d1.month>=3)
			{
				sum=sum+365+Leap_Year(d1.year);
				d1.year--;
			}
			else
			{
				d1.year--;
				sum=sum + 365 + Leap_Year(d1.year);
			}
		}
		
		sum_bak = sum;
		
		while(sum < 0)
		{
			temp =-d1.day;
			sum = sum - temp;
			if (sum  <= 0)
			{
				d1.month--;
				if (d1.month < 1)
				{
					d1.month = 12;
					d1.year--;
				}
				if (d1.month == 2)
					d1.day  = a_Month[d1.month - 1] + Leap_Year(d1.year);
				else
					d1.day = a_Month[d1.month - 1];
				
				sum_bak = -sum;
			}
		}
		if (sum_bak < 0)
		{
			d1.day = d1.day + sum_bak;
		}else
		{
			
			if (d1.month == 2)
				d1.day = a_Month[d1.month - 1] + Leap_Year(d1.year) - sum_bak;
			else
				d1.day = a_Month[d1.month - 1] - sum_bak;
		}
	}
	
	
	SPRINTF(file_name,AEEFS_GPSDATA_DIR"%04d-%02d-%02d.txt",d1.year,d1.month,d1.day); 
}

//ɾ�����ڵ���������
void delete_overGpsData(newgimis* pMe)
{
	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	int i = 0 ;
	
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData in, ISHELL_CreateInstance err");
		return;
	}
	
	if ( IFILEMGR_EnumInit( pFileMgr, AEEFS_GPSDATA_DIR, FALSE ) )  
	{
		int iAccess;
		iAccess = IFILEMGR_CheckPathAccess(pFileMgr,AEEFS_GPSDATA_DIR,AEEFP_DELETE,NULL);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData IFILEMGR_CheckPathAccess iAccess = %d",iAccess);
	}
	
	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
	MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
	if( pFileInfo )   
	{	
		//����Ŀ¼���ļ�
		while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
		{  // while -- 1
			
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData pFileInfo->szName = %s",pFileInfo->szName);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData pFileInfo->szName = %s",pFileInfo->dwSize);
	
			//����nKeepDays�趨�Ļ����ļ��������ж��ļ��Ƿ�Χ�������ļ������򷵻ر���Ŀ¼����һ���ļ���
			for (i = 0; i <= pMe->nKeepDays ; i++)
			{
				if ( STRCMP(pFileInfo->szName, pMe->pfile_Name[i]) == 0 )
				{
					MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
					break;
				}
			}
			
			//��������������û��break��˵��û�в��Ƿ�Χ�������ļ���ɾ����
			if ( pMe->nKeepDays + 1 == i)
			{
				//ɾ�������ļ�
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData pFileInfo->szName = %s",pFileInfo->szName);
				Delete_file(pMe,pFileInfo->szName);
			}

		}  // while -- 1
		
		FREEIF( pFileInfo );	
	}	// if -- pFileInfo	
	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}

	//�ͷ���ָ��
	FREEIF(pMe->pfile_Name);
	
	return;
}

//��ʼ��ʱ�����ļ�
void init_check_gpsData(newgimis *pMe)
{
	int i = 0;
	int j = 0;
	JulianType dDate;												
	GETJULIANDATE(0,&dDate);

	//ֻ�����ź�֮��ſ�ʼģ�顣
	if (pMe->m_enable_evdo != 1)	
	{
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"testgps_StartSendXT evdo init continue....");
		ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) init_check_gpsData, (uint32*) pMe);
		return;
	}

	if (pMe->iUDPPORT_YYXF != 0  && pMe->sUDPIP_YYXF !=NULL && pMe->iTCPPORT_YYXF != 0  && pMe->sTCPIP_YYXF !=NULL)
	{
		//ƽ��ҽԺ�����������ϴ����ܡ�
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"ƽ��ҽԺ-�ر������ϴ�����");
		return;
	}


	//��ƽ̨���������ļ��ϴ�����Ϊ0ʱ�� (;pd )�رչ��ܡ�//12 18�����һ��
	if ( (0 != pMe->nKeepDays) && ( (12 == dDate.wHour && 0 == dDate.wMinute) || (18 == dDate.wHour && 0 == dDate.wMinute) )  )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"init delete_overGpsData start"); 
		
		//��ָ���ʼ��(nKeepDays = 3 ˵������3��ǰ���ݣ�������������Ҫ��4���ռ�)
		pMe->pfile_Name = (char(*)[64])MALLOC( sizeof(char) * (pMe->nKeepDays + 1) * 64 );  
		MEMSET(pMe->pfile_Name, 0, sizeof(char) * (pMe->nKeepDays + 1) * 64 );

		//���ݱ��������������ڱ��浽��ָ�루i=0�ǵ��죩
		for (i = 0; i >= (-1 * pMe->nKeepDays); i--)
		{
			Now_To_Fronter(pMe->pfile_Name[j],dDate, i);
			j++;
		}

// 		for (i = 0; i <= pMe->nKeepDays; i++)
// 		{
// 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"delete_overGpsData pfile_Name[%d] = %s",i,pMe->pfile_Name[i]);
// 		}

		//����gpsData�ļ�Ŀ¼�ȶ��ļ�
		delete_overGpsData(pMe);
	}

	//��ƽ̨���������ļ��ϴ�����Ϊ0ʱ�� (;pd )�رչ��ܡ�//ÿ��10����һ��
	if ( (0 != pMe->nKeepDays) && (0 == dDate.wMinute % 10) )
	{
		///////////////////////GPS�������ݰ�/////////////////////////
		if (pMe->mux_7Day == 0)
		{
			send_GPSData_7Day(pMe);
			if (pMe->nState_7Day == 1)
			{
				pMe->nState_7Day = 0;
				if (Upload_Init_7Day(pMe) == 0){
					Upload_Start_7Day(pMe);
				}
				else{
					Upload_Stop_7Day(pMe);
				}
			}
		}
		/////////////////////////////////////////////////////////////
	}

	ISHELL_SetTimer(pMe->a.m_pIShell, 60000, (PFNNOTIFY) init_check_gpsData, (uint32*) pMe);
}


////////////////////////7���������ݻ���д��ģ��////////////////////////
void write_GPSData_7Day(newgimis *pMe, char *strData)
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
	char file_name[50] = {0};

	JulianType dDate;												
	GETJULIANDATE(0,&dDate);

	SPRINTF(file_name,AEEFS_GPSDATA_DIR"%04d-%02d-%02d.txt",dDate.wYear,dDate.wMonth,dDate.wDay);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_GPSData_Day file_name is %s",file_name);

	pMe->mux_7Day = 1;

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, file_name) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, file_name, _OFM_READWRITE);
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, file_name, _OFM_CREATE);
		}
	}
	else
	{
		return;
	}

	if (pTempFile != NULL)
	{
		char *sc_Start = NULL;
	    char *sc_tmp = NULL;
		int si_Count = 0;
		FileInfo fiInfo;

		if ( IFILE_GetInfo( pTempFile, &fiInfo ) == SUCCESS) 
		{
			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			
			IFILE_Seek(pTempFile,_SEEK_END,0);
			if ( fiInfo.dwSize != 0 ) 
			{
				STRCPY(sc_tmp,":");
				IFILE_Write(pTempFile, sc_tmp, 1); 
			}
			else
			{
				STRCPY(sc_tmp,"&pp=");
				IFILE_Write(pTempFile, sc_tmp, 4); 
			}
			FREEIF(sc_tmp);
		}

		sc_Start = STRSTR(strData,"&pp=");
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_GPSData_Day sc_Start = %s",sc_Start);
		if(sc_Start != NULL)
		{
			sc_tmp = (char *)MALLOC(1000);
			MEMSET(sc_tmp,0x00,1000);
			STRCPY(sc_tmp,sc_Start + 4);
			
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_GPSData_Day data is %s len is %d",sc_tmp,STRLEN(sc_tmp));

			IFILE_Seek(pTempFile,_SEEK_END,0);
			si_Count = IFILE_Write(pTempFile, sc_tmp, STRLEN(sc_tmp)); //д���һ������

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_GPSData_Day IFILE_Write data is %d",si_Count);

			FREE(sc_tmp);
			sc_tmp = NULL;
		}
		 
	}
	   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
		IFILE_Release( pTempFile );
	
	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );

	pMe->mux_7Day = 0;

	return ;
}

void send_GPSData_7Day(newgimis *pMe)
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
	FileInfo fiInfo;
	//int nResult = 0;
	int ilen = 0;
	//char sLogBuf[MAX_FILE_BYTES_POST];

	int i = -1 * pMe->nKeepDays;

	JulianType dTemp;
	//JulianType dDate;
	GETJULIANDATE(0,&dTemp);

	pMe->mux_7Day = 1;

	while (i <= 0)
	{
		MEMSET(pMe->file_Name_7Day, 0, sizeof(pMe->file_Name_7Day));

		Now_To_Fronter(pMe->file_Name_7Day,dTemp, i);	//�����ڼ�ȥi,�õ������ڴ浽pMe->file_Name_7Day

		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"send_GPSData_7Day file_name is %s",pMe->file_Name_7Day);

		if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
		{
			if (IFILEMGR_Test(pFileMgr, pMe->file_Name_7Day) == SUCCESS)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"send_GPSData_7Day file_name is %s",pMe->file_Name_7Day);
				pTempFile = IFILEMGR_OpenFile(pFileMgr, pMe->file_Name_7Day, _OFM_READWRITE);
				if ( !pTempFile )
				{
					uf_RecLog(pMe,"send_GPSData_7Day IFILEMGR_OpenFile fail");
					pMe->nState_7Day = 0;
				}
				else 
				{
					if ( SUCCESS != IFILE_GetInfo( pTempFile, &fiInfo ) ) 
					{
						pMe->nState_7Day = 0;
					}
					
					if ( fiInfo.dwSize == 0 ) 
					{
						pMe->nState_7Day = 0;
					}
					else
					{

						pMe->nState_7Day = 1;
						//�ļ������������ݣ�����pszPostData_7Day �Ա��ȥpost������
						if (fiInfo.dwSize > MAX_FILE_BYTES_POST)
						{
							ilen = MAX_FILE_BYTES_POST;
							ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"send_GPSData_Day ilen > 500K");
						}else{
							ilen = fiInfo.dwSize;
						}

						while (pMe->pszPostData_7Day == NULL)
						{
							pMe->pszPostData_7Day = MALLOC(ilen + 1);
						}

						MEMSET(pMe->pszPostData_7Day,0,ilen + 1);
						
						IFILE_Seek(pTempFile,_SEEK_START ,0);
						pMe->fileRead_7Day = IFILE_Read( pTempFile, pMe->pszPostData_7Day, ilen + 1 );
			
						if ( pMe->fileRead_7Day <= 0 ) {
							pMe->nState_7Day = 0;
							FREE(pMe->pszPostData_7Day);
						}

						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"send_GPSData_Day pMe->pszPostData_7Day len is %d",STRLEN(pMe->pszPostData_7Day));
					}
				}
			}
			else
			{
				pMe->nState_7Day = 0;
			}
		}
		else
		{
			pMe->nState_7Day = 0;
		}


		if (pTempFile)
		{
			IFILE_Release( pTempFile );
			pTempFile = NULL;
		}
		
		if (pFileMgr)
		{
			IFILEMGR_Release( pFileMgr );
			pFileMgr = NULL;
		}

	

		if (pMe->nState_7Day == 1)
		{
			break;
		}

		i++;
	}


	pMe->mux_7Day = 0;
}


int Upload_Init_7Day(newgimis *pMe)
{
	int nErr = SUCCESS;

	int nState_7Day = EFAILED;

	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_Init_7Day in");
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&pMe->m_pIWeb_7Day);			// ---- Web�ӿ�
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "Upload_Init_7Day IWeb err=%d", nErr);
		goto ExitHere;
	}
	
	ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pMe->pisu_7Day);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_Init_7Day ISourceUtil err=%d", nErr);
		goto ExitHere;
	}

	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		SPRINTF(pMe->pURL_7Day,"http://%s&ver=3.1.2au&tel=%s&imsi=%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID);
		//SPRINTF(pMe->pURL_7Day,"http://61.131.1.153:9810/partition/brewGPS/BrewServlet?act=httpdec&ver=3.0.5std");
		//http://cn.richtalk.cn:80/brewGPS/BrewServlet?act=httpdec&ver=3.1.2au&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM0;TT20160720112129&tel=13348552109&imsi=460030948567805
	}
	else
	{
		SPRINTF(pMe->pURL_7Day,"http://%s&ver=3.1.2au&imsi=%s",pMe->strCenterIp,pMe->szMobileID);
	}
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_Init_7Day in url=%s",pMe->pURL_7Day);

	nState_7Day = SUCCESS;
	
ExitHere:
	if (SUCCESS != nErr)
		nState_7Day = EFAILED;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_Init_7Day  out nState=%d",nState_7Day);
	
	return nState_7Day;
}

// ��post������������
void Upload_Start_7Day(newgimis* pMe)
{
    WebOpt awo[4]; 
	//	cgimis * pMe = pwa->m_pParent;
	int iLen = 0;
	
	int m_temp = 0;
    // Add Options
	awo[0].nId  = WEBOPT_CONNECTTIMEOUT;
    awo[0].pVal = (void *)20000;			// 2013-03-27  5000 
	// 2.6.2D2   awo[1].nId  = WEBOPT_FLAGS;
	// 2.6.2D2   awo[1].pVal = (void *)WEBREQUEST_NOWAITCONN;
    awo[1].nId  = WEBOPT_PROXYSPEC;
    awo[1].pVal = (void *)"http:///http://10.0.0.200:80";
    awo[2].nId  = WEBOPT_END;
	IWEB_AddOpt(pMe->m_pIWeb_7Day, awo);
	
	pMe->PostOvertime_7Day = 0;
	
	if (pMe->pipPostData_7Day != NULL) {
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Upload_Start_7Day pipPostData_7Day clear");
		IPEEK_Release(pMe->pipPostData_7Day);
		pMe->pipPostData_7Day = NULL;
	}
	
	// if there's post data, construct a stream for IWeb to consume
	if ((char *)0 != pMe->pszPostData_7Day) {
		iLen = pMe->fileRead_7Day;
		m_temp = ISOURCEUTIL_PeekFromMemory(pMe->pisu_7Day, pMe->pszPostData_7Day, iLen, 0, 0,&pMe->pipPostData_7Day);
		//ISOURCEUTIL_Release(pMe->pisu_7Day);
	}

	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Upload_Start_7Day iLen %d m_temp = %d",iLen,m_temp);

	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"coming Upload_Start_7Day");
	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if ((IPeek *)0 != pMe->pipPostData_7Day) {
		
		// initialize the callback, where I'll be called when the request  completes
		CALLBACK_Init(&pMe->cb_7Day, Upload_GotResp_7Day, pMe);
		
		IWEB_GetResponse(pMe->m_pIWeb_7Day,
			(pMe->m_pIWeb_7Day, &pMe->piWResp_7Day, &pMe->cb_7Day, pMe->pURL_7Day, 
			WEBOPT_HANDLERDATA, pMe,
			WEBOPT_HEADER, "X-Method: POST\r\n  Content-Type: application/x-www-form-urlencoded\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, Upload_Header_7Day,   
			WEBOPT_STATUSHANDLER, Upload_Status_7Day,  
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, pMe->pipPostData_7Day,
			WEBOPT_CONTENTLENGTH, iLen,		
			WEBOPT_END));
	}
}

void Upload_Header_7Day(void *p, const char *cpszName, GetLine *pglVal)
{
	newgimis *pwa = (newgimis *)p;
	if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
	}
}

void Upload_GotResp_7Day(void *p)	
{
	char sLogBuf[257];
	int nLen =0;
	newgimis *pMe = (newgimis *)p;
	//	cgimis * pMe = pwa->m_pParent; 
	WebRespInfo *pInfo;
	
	uf_RecLog(pMe,"Upload_GotResp_7Day in");

	
	
	//   char szBuf[500];
	pInfo = IWEBRESP_GetInfo(pMe->piWResp_7Day);

	SPRINTF(sLogBuf,"Upload_GotResp_7Day,Got Message, time:%u",(GETTIMEMS()-pMe->PostOvertime_7Day));
	uf_RecLog(pMe, sLogBuf);

	pMe->PostOvertime_7Day = 0 ;
	
	SPRINTF(sLogBuf, "Upload_GotResp_7Day response, code: %d",pInfo->nCode);
	nLen = STRLEN(sLogBuf);
	uf_RecLog(pMe,sLogBuf);
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) {  
		
		//ISHELL_SetTimer(me->a.m_pIShell, 3000, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
		Upload_Stop_7Day(pMe);
		return;
	}
	
	
	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 
			return; 	      
		
		pMe->PostOvertime_7Day = 0;				// ���յ�Ӧ��
		
		pMe->pSource_7Day = pInfo->pisMessage;
		ISOURCE_AddRef(pMe->pSource_7Day);				// ---- ��Read Body���õ�
		// 2.6.2D2		IWEBRESP_Release(me->piWResp);
		// 2.6.2D2		me->piWResp = 0;
		{
			newgimis * pMe; 
			int n=0;			// 2.6.4D2
			uint32 nErr = IWEBRESP_Release(pMe->piWResp_7Day);

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_GotResp_7Day in,clear piWResp,return=%d,n=%d",nErr,n);
			pMe->piWResp_7Day = NULL;
		}
		
		CALLBACK_Cancel(&pMe->cb_7Day);
		CALLBACK_Init(&pMe->cb_7Day, Upload_ReadBody_7Day, pMe);
		Upload_ReadBody_7Day(pMe);
	}
}	

void Upload_ReadBody_7Day(newgimis *pMe)
{
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	char *sBuf_T = NULL;
	char *sBuf1 = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"coming Upload_ReadBody_7Day");

	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);
	
	nRead = ISOURCE_Read(pMe->pSource_7Day, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		ISOURCE_Readable(pMe->pSource_7Day, &pMe->cb_7Day);
		return;
	}
	
	if (nRead > 0)		
	{
		SPRINTF(sc_Count,"%d",nRead);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sBuf); 
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," nRead  GPS = %d",nRead); 
		
		if (nRead > 100)
		{
			sBuf1 = (char *)MALLOC(1024);
			sBuf_T = (char *)MALLOC(102400);
			
			while (!sBuf_T)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sBuf_T GPS Failure to apply for space 1"); 
				sBuf_T = (char *)MALLOC(102400);
			}
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,sBuf);
		}
		else
		{
			sBuf_T = (char *)MALLOC(1024);
			while(!sBuf_T)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sBuf_T GPS Failure to apply for space 2"); 
				sBuf_T = (char *)MALLOC(1024);
			}
			MEMSET(sBuf_T,0x00,1024);
			STRCPY(sBuf_T,sBuf);
		}
		
		while (nRead > 100)
		{
			
			MEMSET(sBuf1,0x00,1024);
			nRead = ISOURCE_Read(pMe->pSource_7Day, sBuf1, 1023);
			STRCAT(sBuf_T,sBuf1);
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"while nRead GPS = %d",nRead); 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sBuf1); 
		}

		if (STRSTR(sBuf_T,"1") != NULL)
		{
			pMe->mux_7Day = 1;
			Delete_file(pMe,pMe->file_Name_7Day);
			pMe->mux_7Day = 0;
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sBuf_T gps = %d ",STRLEN(sBuf_T)); 
		uf_RecLog(pMe,sBuf_T);
		
		uf_fromESC(sBuf_T,sBuf_d);

		si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pMe,sBuf_d);

		FREEIF(sBuf_T);
		sBuf_T = NULL ;
		
		FREEIF(sBuf1);
		sBuf1 = NULL ;
		
		switch (si_Ret)  //˵���ɹ�
		{
		case 0:
			{

			}
			break;	
		case 1:
			{
				//gimis_WriteGPSSettings(pwa,);
				pMe->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pMe); 					
			}
			break;
		}
	
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		}
		Upload_Stop_7Day(pMe);
}

void Upload_Status_7Day(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   (void)pVal;
   
   switch (ws) {
   case WEBS_CANCELLED:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
	   break;
   case WEBS_GETHOSTBYNAME:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	   break;          
   case WEBS_CONNECT:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
	   break;
   case WEBS_SENDREQUEST:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
	   
	   pMe->PostOvertime_7Day = GETTIMEMS();
	   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_7Day, pMe);
	   break;
   case WEBS_READRESPONSE:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	   break;
   case WEBS_GOTREDIRECT:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
	   break;
   case WEBS_CACHEHIT:
	   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	   break;
	   
   default:
	   if (pMe->PostOvertime_7Day == 0) {			//  Add 2013-09-23
		   pMe->PostOvertime_7Day = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_7Day, pMe);
	   }		// Add End
	   break;
   }
}

void uf_MonitorPost_7Day(newgimis *pMe)
{
	if ( pMe->PostOvertime_7Day == 0)
		return;
	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost_7Day in , http time out");

	 Upload_Stop_7Day(pMe);
}

void Delete_file(newgimis *pMe,char* file_name)
{
	IFileMgr* pFileMgr = NULL; 

	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS )
	{
		if ( IFILEMGR_Remove(pFileMgr, file_name) == SUCCESS ) 
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Remove %s success",file_name); 
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IFILEMGR_Remove %s fail",file_name); 
		}
	}

	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
}

void Upload_Stop_7Day(newgimis *pMe)
{
	CALLBACK_Cancel(&pMe->cb_7Day);
	// then clean up, if necessary
	if ((char *)0 != pMe->pszPostData_7Day) {
		
		FREE(pMe->pszPostData_7Day);
		pMe->pszPostData_7Day = NULL;
	}
	
	if (pMe->pipPostData_7Day != NULL)
	{
		IPEEK_Release(pMe->pipPostData_7Day);
		pMe->pipPostData_7Day = NULL;
		
	}
	
	if (pMe->pisu_7Day != NULL) {
		ISOURCEUTIL_Release(pMe->pisu_7Day);
		pMe->pisu_7Day = NULL;
	}
	
	if (pMe->pSource_7Day != NULL) {
		uint32 nErr = ISOURCE_Release(pMe->pSource_7Day);	// 2013-04-12 �� nErr
		pMe->pSource_7Day = NULL;
	}

	if (pMe->piWResp_7Day)
	{
	   IWEBRESP_Release(pMe->piWResp_7Day);
	   pMe->piWResp_7Day= NULL;
	}

	if (pMe->m_pIWeb_7Day) 
	{
	   int i,n;			// 2.6.4D2
	   n = 0;
	   i=IWEB_Release(pMe->m_pIWeb_7Day);
	   while (i>0) {
		   n++;
		   i=IWEB_Release(pMe->m_pIWeb_7Day);
	   }
	   pMe->m_pIWeb_7Day = NULL;
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Upload_Stop_7Day succeed"); 
}


//�����������gps���ݵ�Ŀ¼
void init_gpsDir(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL; 
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS )
	{
		if ( IFILEMGR_MkDir(pFileMgr, AEEFS_GPSDATA_DIR) == SUCCESS ) 
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"init_gpsDir success"); 
		}else
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"init_gpsDir fail"); 
		}
	}

	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
}

//������Ŷ������ݵ�Ŀ¼
void init_smsDir(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL; 

	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS )
	{
		if ( IFILEMGR_MkDir(pFileMgr, AEEFS_SMSDATA_DIR) == SUCCESS ) 
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"init_smsDir success"); 
		}else
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"init_smsDir fail"); 
		}
	}
	
	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
}


/********************************************************************************************
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  uint16 *pIndex  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/


void My_Log(IShell *pShell,char cWrtLog, uint16 *pIndex, const char *szFormat, ... )
{
	char sDateTime[20];
	int lInt_Len;
	char    lStr_msg[MAX_MESSAGE_LEN+sizeof(sDateTime)+4+5];
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// �ļ���Ϣ

	va_list args;

// ��������DBGPRINTF֮�� ��������Ϣ�����ڱ� 	if ( cWrtLog == '0' ) return;		// 2013-04-27 ���ڴ˴�

   // ��¼д��־��ʱ��
   GETJULIANDATE(0,&dDate);		//	ȡ������	
   MEMSET(sDateTime,0,sizeof(sDateTime));
   // yyyy-mm-dd hh:MM:ss    ��19���ֽ�
   SPRINTF(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
   
   MEMSET(lStr_msg,0,sizeof(lStr_msg));
   SPRINTF(lStr_msg,"%d[%s]:",*pIndex,sDateTime);
   lInt_Len = STRLEN(lStr_msg);

	va_start( args, szFormat );
	(void)VSNPRINTF( lStr_msg+lInt_Len, MAX_MESSAGE_LEN, szFormat, args );
	va_end( args );

	DBGPRINTF("----=%s",lStr_msg);

	if ( cWrtLog == '0' ) return;		// 2013-10-09  ��������Ϣ�����ڱ�

	 STRCAT(lStr_msg,"\r\n");

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, "My_log.log") == SUCCESS)
			pTempFile = IFILEMGR_OpenFile(pFileMgr, "My_log.log", _OFM_READWRITE);
		else
			pTempFile = IFILEMGR_OpenFile(pFileMgr, "My_log.log", _OFM_CREATE);
	}
	else
		return;

	if (pTempFile != NULL)
	{
		if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) {
// Change by ---- 2013-01-01			if ( fiInfo.dwSize > 1024*500 ) {
			if ( (fiInfo.dwSize > 1024*600) || (cWrtLog == '4') ) {
				IFILE_Truncate(pTempFile, 0);
			}
		}

	   IFILE_Seek(pTempFile,_SEEK_END,0);
	   lInt_Len = STRLEN(lStr_msg);
	   IFILE_Write(pTempFile, lStr_msg, lInt_Len);
	   *pIndex = ( *pIndex > 50000 ) ? 1 : (*pIndex)+1;
	}


   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );
}




int My_StrTOint(const char *str)
{
	int sum = 0;
	while(*str)
	{
		sum = sum*10 + (*str - '0');
		str++;
	}
	return sum;
}

/*===========================================================================
   This function adds one item to the specified IMenuCtl.
===========================================================================*/
boolean MP_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData)
{
   CtlAddItem  ai;

   // Fill in the CtlAddItem structure values
   ai.pText = pText;
   ai.pImage = NULL;
   ai.pszResImage = NEWGIMIS_RES_FILE;
   ai.pszResText = NEWGIMIS_RES_FILE;
   ai.wText = wTextID;
   ai.wFont = AEE_FONT_NORMAL;
   ai.wImage = wImageID;
   ai.wItemID = wItemID;
   ai.dwData = dwData;

   // Add the item to the menu control
   return IMENUCTL_AddItemEx( pMenu, &ai );   //��� pai->pImage Ϊ NULL ����ָ���� pai->wImage ���������Դ�ָ����Դ����ͼ�����ͼ�����ʧ�ܺ��������� FALSE
}


boolean MP_AddMenuItemEx(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData)
{
	CtlAddItem  ai;
	
	// Fill in the CtlAddItem structure values
	ai.pText = pText;
	ai.pImage = NULL;
	ai.pszResImage = NEWGIMIS_RES_FILE;
	ai.pszResText = NULL;
	ai.wText = wTextID;
	ai.wFont = AEE_FONT_NORMAL;
	ai.wImage = wImageID;
	ai.wItemID = wItemID;
	ai.dwData = dwData;
	
	// Add the item to the menu control
	return IMENUCTL_AddItemEx( pMenu, &ai );   //��� pai->pImage Ϊ NULL ����ָ���� pai->wImage ���������Դ�ָ����Դ����ͼ�����ͼ�����ʧ�ܺ��������� FALSE
}



// =====================================================================
// ��ʾ��Ļ����ʾ��ǰ���뷨��Ϣ
// =====================================================================
void yb_ShowInputMode(newgimis * pMe, AEETextInputMode eInputMode, int iMode)
{
	
	char  szBuf[10];
	AECHAR szText[10];

	cTaskApp *me = NULL;
	if(pMe->pTaskApp)
	{
		me = pMe->pTaskApp;
	}
	
	if ( (STRCMP(pMe->sMobileType,"33") != 0) )
		return;
	
	switch(eInputMode)
	{
	case AEE_TM_PINYIN:
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_PINYIN,szText,sizeof(szText));
		break;
	case AEE_TM_NUMBERS:
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_NUMBER,szText,sizeof(szText));
		break;
	case AEE_TM_LETTERS:
	case AEE_TM_EZTEXT:
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_LETTERS,szText,sizeof(szText));
		break;
	case AEE_TM_STROKE:
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_STROKE,szText,sizeof(szText));
		break;
		
	default:
		SPRINTF(szBuf,"%d",eInputMode);
        STRTOWSTR((char*)szBuf, szText, sizeof(szText));
		break;
	}

	if (1 == iMode) //��ʾ���뷨ģʽ(��Ƭ)
	{	
		yb_WPrintf(pMe,13,1,AEE_FONT_NORMAL,IDF_ALIGN_LEFT | IDF_RECT_FILL,szText);	
	}
	else if (2 == iMode)//��ʾ���뷨ģʽ(����)
	{
		if (me->pTaskRecB->bFlow)
		{
			yb_WPrintf(pMe,8,1,AEE_FONT_NORMAL,IDF_ALIGN_LEFT | IDF_RECT_FILL,szText);
		}
		else
		{
			yb_WPrintf(pMe,9,1,AEE_FONT_NORMAL,IDF_ALIGN_LEFT | IDF_RECT_FILL,szText);// ԭ����8-1 [3/3/2016 Administrator]
		}
		
		
	}
	
//	yb_WPrintf(pMe,14,2,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szText);
	
}

/*===========================================================================
// ����Ļ����ʾ˫�ֽ���Ϣ
===========================================================================*/
void yb_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText)
{
	RGBVAL iColor;
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;

	int nAEChars;
	int nCPixel;		// ÿ���ַ�����ռ������

	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
	nMaxLines = (di.cyScreen / LINEHEIGHT) - 2;
	if (nMaxLines < 1)		nMaxLines = 1;

	nCPixel = di.cxScreen / MAXCOLUMNS;
	rc.x = nCol * nCPixel;

//	rc.dx = di.cxScreen - nCol * nCPixel;  //һ����
	rc.dx = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText,-1,-1,NULL);
	rc.y = nLine * LINEHEIGHT;

//	SETAEERECT( , nCol * di.cxScreen / MAXCOLUMNS, nLine * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+2);

   if( dwFlags & IDF_ALIGNVERT_MASK ) {
	   rc.dy = di.cyScreen - rc.y;
   }
   else {
      rc.dy = LINEHEIGHT+2;
   }

   nAEChars = WSTRLEN(szText);


   iColor = IDISPLAY_SetColor(pMe->a.m_pIDisplay,CLR_USER_BACKGROUND, MAKE_RGB(255,182,193));

   IDISPLAY_DrawText(pMe->a.m_pIDisplay, fnt, szText, nAEChars, rc.x, rc.y, &rc, dwFlags);
   IDISPLAY_SetColor(pMe->a.m_pIDisplay,CLR_USER_BACKGROUND,iColor);

   IDISPLAY_Update(pMe->a.m_pIDisplay);
   
}










// Helper Functions

/**********************************************
*��������FILE *FILE_Open(void *pData)
*�������ܣ�ʵ������һ�������ڵ��ļ����½��ļ�������һ���Ѿ����ڵ��ļ��Ͳ����´�����
*����������pData:����ļ���
*��������ֵ��fp:�ɹ�  NULL:ʧ��


  û����
**********************************************/
IFile *FILE_Open(IShell *m_pIShell, IFileMgr *pFileMgr, char *pFileName)
{
	IFile * pTempFile = NULL;
	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_1) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, pFileName, _OFM_READWRITE);
			//			uf_RecLog(pMe,"�ж���û���������� 5");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, pFileName, _OFM_CREATE);
			//			uf_RecLog(pMe,"�ж���û���������� 4");
		}
	}
	else
	{
		//		uf_RecLog(pMe,"���������ļ�ʧ��");
	//	uf_RecLog(pMe,"create offline file fail");
		return	pTempFile;
	}
}




/*
void Save_GPSData_All(newgimis *me)
{
	int i_Ret = 0;
	uf_RecLog(me,"judge offline data 1");
	
	// ������ʵ�ֱ�����������
	i_Ret = gimis_judgedata(me);
	switch(i_Ret)
	{
	case 0:  //  ˵����û�������ļ�����
		//		ud_RecLog(me->a.m_pIShell,me->cWrtLog,&(me->nInd),"����GPS����");
		//		uf_RecLog(me,"�ж���û���������� 2");
		Save_GPSData(me);
		break;
	default: //˵�����ļ����� 
		Save_GPSData_had( me );
		break;
	}
	me->m_OffData = 1;
	
	//	uf_RecLog(me,"�ж���û���������� 3");
	return ;
}
*/