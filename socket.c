#include "socket.h"

//#define IP_TCP "61.131.1.153"
//#define TCP_PORT 10011

#define DEFAULT_OVERTIME 10
#define RESTART_TIME 5
#define HEART_TIME 100

#define UDPDATA_OVERTIME 10

static void ReleaseObj(void ** ppObj)
{
	if (*ppObj) {
		(void)IBASE_Release(((IBase *)*ppObj));
		*ppObj = NULL;
	}
}

#define SmsAlert_YYXF(pMe) {	\
	ISHELL_Beep(pMe->a.m_pIShell, BEEP_MSG, TRUE);		\
	ISHELL_Beep(pMe->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);	\
	ISHELL_SetTimer(pMe->a.m_pIShell, 3 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)pMe );	\
}

void Socket_ConnectCB_TCP(void* p,int error)
{
	newgimis *pMe = (newgimis*)p;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ConnectCB_TCP error = %d ",error);

	//Check error code
	switch(error)
	{
	case AEE_NET_ETIMEDOUT:
		// Connection timed out
		break;
	case AEE_NET_SUCCESS:

		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ConnectCB_TCP  AEE_NET_SUCCESS");

		pMe->TCP_Status_yyxf = 1;

		ISHELL_SetTimer(pMe->a.m_pIShell, HEART_TIME*1000, (PFNNOTIFY)Socket_Listen_Heart, (uint32*) pMe);
		// Send Some data
		Socket_WriteCB((void*)pMe);

		break;
	default:
		// Some other Network error
		break;
	}
	
	if (error)
	{
		Socket_Clean_TCP(pMe);
	
		return;
	}
	

}

void Scoket_init_TCP(newgimis *pMe)
{
	int nErr = 0;
	INAddr nodeINAddr;
	uint16 nPort;

	pMe->m_nRecvSize = 0;

	if (!pMe->pINetMgr_TCP_yyxf)
	{
		//ʵ��
		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_NET, (void **)&(pMe->pINetMgr_TCP_yyxf) );
		if (SUCCESS != nErr) 
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "scoket_init_TCP ISHELL_CreateInstance err=%d", nErr);
			goto ExitHere;
		}
		
		//��socket�õ��׽���
		pMe->pISocket_TCP_yyxf = INETMGR_OpenSocket(pMe->pINetMgr_TCP_yyxf,AEE_SOCK_STREAM);
		if ( !pMe->pISocket_TCP_yyxf) //AEE_SOCK_DGRAM
		{
			nErr = INETMGR_GetLastError(pMe->pINetMgr_TCP_yyxf);
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"INETMGR_OpenSocket fail nErr = %d",nErr);
			goto ExitHere;
		}
		
		//����
		INET_ATON(pMe->sTCPIP_YYXF,&nodeINAddr);
		nPort = HTONS(pMe->iTCPPORT_YYXF);
		ISOCKET_Connect(pMe->pISocket_TCP_yyxf,nodeINAddr,nPort,(PFNCONNECTCB)Socket_ConnectCB_TCP,pMe);
	}


ExitHere:
	if (SUCCESS != nErr)
	{
		Socket_Clean_TCP(pMe);
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"scoket_init fail nErr = %d",nErr);
	}
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Scoket_init_TCP connect start ");
	return;
}



void Socket_WriteCB(void *p)
{
	newgimis *pMe = (newgimis*)p;
	int nWrite;
	int sendLen;
	JulianType dDate;
	char sendBuf[1024] ;
	char sdataBuf[128] ;

	TCP_HEAD s_head;
	MEMSET(&s_head,0x00,sizeof(TCP_HEAD));
	MEMCPY(s_head.heater,PACKET_TCP_ID_HEAD,4);
	s_head.type[0] = '1';
	
	MEMSET(sendBuf,0x00,sizeof(sendBuf));
	MEMSET(sdataBuf,0x00,sizeof(sdataBuf));

	GETJULIANDATE(0,&dDate);
	SPRINTF(sdataBuf,"%04d%02d%02d%02d%02d%02d%s",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond,pMe->MyMobileNo);

	s_head.Length = sizeof(TCP_HEAD) + STRLEN(sdataBuf);

	MEMCPY(sendBuf,&s_head,sizeof(TCP_HEAD));
	MEMCPY(sendBuf + sizeof(TCP_HEAD),sdataBuf,STRLEN(sdataBuf));

	sendLen = s_head.Length;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_WriteCB =====send pack=====");

	if (pMe->pISocket_TCP_yyxf == NULL)
	{
		Socket_Clean_TCP(pMe);
		return;
	}

	nWrite = ISOCKET_Write(pMe->pISocket_TCP_yyxf, (byte*)sendBuf, sendLen);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Socket_WriteCB nWrite = %d",nWrite);
	
	if (nWrite == AEE_NET_WOULDBLOCK)
	{
		ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_WriteCB, (void*)pMe);
		return;
	}
	else if (nWrite == AEE_NET_ERROR) 
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_WriteCB Failed: Error %d\n", ISOCKET_GetLastError(pMe->pISocket_TCP_yyxf));
		Socket_Clean_TCP(pMe);
		return;
	}
	else if (nWrite > 0) // The actual data that was read
	{
		pMe->m_Listen_Heart = GETTIMEMS();	

		if (nWrite == sendLen)	//�����Ƿ����ֲ���������ȣ�����������û���ڷ������������رա�
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_WriteCB send %d success",sendLen);
			Socket_ReadCB((void*)pMe);
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_WriteCB  ISOCKET_Readable");
			ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_WriteCB, (void*)pMe);
		}
		return;
	}
	else // rv == 0 No more data
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_WriteCB  No more data");

		//��Ҫ����
		Socket_Clean_TCP(pMe);
	}
	
	return;
}

void Socket_ReadCB(void *p)
{
	newgimis *pMe = (newgimis*)p;

	char *sBuf_T = NULL; //���ݰ����ݴ��
	int si_Ret = 0; //�������ݽ������

	int m_HeadFlag = 0;

	int nRecvSize = 0;
	int nPacketSize = 0;
	TCP_HEAD* pHead  = NULL;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ReadCB =====read pack=====");

	if (pMe->pISocket_TCP_yyxf == NULL)
	{
		Socket_Clean_TCP(pMe);
		return;
	}

	nRecvSize = ISOCKET_Read(pMe->pISocket_TCP_yyxf, (byte*)&pMe->m_readMsg_tcp + pMe->m_nRecvSize, 5120 - pMe->m_nRecvSize);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Socket_ReadCB Read = %d",nRecvSize);

	if (nRecvSize == AEE_NET_WOULDBLOCK)
	{
		ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_ReadCB, (void*)pMe);
		return;
	}
	else if (nRecvSize == AEE_NET_ERROR) 
	{
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ReadCB Failed: Error %d\n", ISOCKET_GetLastError(pMe->pISocket_TCP_yyxf));
      Socket_Clean_TCP(pMe);
	  return;
	}
	else if (nRecvSize > 0) // The actual data that was read
	{
		pMe->m_Listen_Heart = GETTIMEMS();
		// �����Ѿ��������ݵĴ�С
		pMe->m_nRecvSize = pMe->m_nRecvSize + nRecvSize;
		// ���յ������ݹ�����һ����ͷ�ĳ���
		while (pMe->m_nRecvSize >= sizeof(TCP_HEAD))
		{
			//for (i = 0;i <= 11;i++)
			//{
			//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB c[] = %x",pMe->m_readMsg_tcp[i]);
			//}

			// ��ȡ��ͷ
			pHead = (TCP_HEAD*)(pMe->m_readMsg_tcp);
			nPacketSize = pHead->Length;		

			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB pHead->Length = %d",nPacketSize);

			//������Ȳ���Ӧ�������� 
			if (nPacketSize <= 0 || nPacketSize >= 5120)
			{
				// �����л��������
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB ͷ���е����ݳ�������-�建���ض�");

				pMe->m_nRecvSize = 0;
				MEMSET(pMe->m_readMsg_tcp, 0, sizeof(pMe->m_readMsg_tcp));
				ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_ReadCB, (void*)pMe);
				//Socket_Clean_TCP(pMe);
				return;
			}

			// �ж��Ƿ��ѽ��յ��㹻һ��������������
			if (pMe->m_nRecvSize < nPacketSize)
			{
				//������
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB ����һ����������������");
				ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_ReadCB, (void*)pMe);
				return;
			}
			
			// ���������ݻ���
			pMe->m_DataBuf = MALLOC(nPacketSize + 1);
			MEMSET(pMe->m_DataBuf, 0, nPacketSize + 1);
			MEMCPY(pMe->m_DataBuf, pMe->m_readMsg_tcp, nPacketSize);
			
			// ����ʣ������
			pMe->m_nRecvSize = pMe->m_nRecvSize - nPacketSize;
			if (pMe->m_nRecvSize != 0){
				MEMCPY(pMe->m_readMsg_tcp, pMe->m_readMsg_tcp + nPacketSize, pMe->m_nRecvSize);
			}
			else{
				MEMSET(pMe->m_readMsg_tcp, 0, sizeof(pMe->m_readMsg_tcp));
			}
			
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Socket_ReadCB ʣ������ pMe->m_nRecvSize= %d",pMe->m_nRecvSize);
			//���
			pHead = (TCP_HEAD*)(pMe->m_DataBuf);
			
			if (STRSTR(pHead->heater, PACKET_TCP_ID_HEAD) == NULL)
			{
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ReadCB head error");

				FREE(pMe->m_DataBuf);

				continue;
			}//end if 
			
			if (pHead->type[0] == '1' )
			{
				m_HeadFlag = 1;
				//��ȷ������
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ReadCB �յ�������");

				FREE(pMe->m_DataBuf);

				continue;
			}//end if 1

			if (pHead->type[0] == '2' )
			{
				//��ȷ���ݰ�
				sBuf_T = MALLOC(pHead->Length - sizeof(TCP_HEAD) + 1);
				MEMSET(sBuf_T, 0, pHead->Length - sizeof(TCP_HEAD) + 1);
				MEMCPY(sBuf_T,pMe->m_DataBuf + sizeof(TCP_HEAD),pHead->Length - sizeof(TCP_HEAD));

				//ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB  sBuf_T= %s\n",sBuf_T);

				si_Ret = resolvePack(pMe,sBuf_T);

				FREE(sBuf_T);//�����꼴�ͷ�

				switch (si_Ret)  //˵���ɹ�
				{
				case 0:
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ReadCB resolvePack error ����������"); 
						//����������
					}
					break;	
				case 1:
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ReadCB resolvePack 1 "); 
						//�����ظ� ����Ƶ�·�
					}
					break;
				case 2:
					{
						//�����ظ� ����Ƶ�·�
						YYXF_INFO t_info;
						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ReadCB resolvePack = %d  �յ��������������� ",si_Ret); 
						STRCPY(t_info.cTel,pMe->MyMobileNo);
						STRCPY(t_info.cName,pMe->m_audioName);
						t_info.icount = pMe->m_audioCount;
						
						inQueue(pMe,t_info);

					}
					break;
				default:
					break;
				}

				FREE(pMe->m_DataBuf);//��ͷ���ԣ��ͷ���Դ��

				if (si_Ret == 2)	//���ͷ�databuf��
				{
					//����Ӧ��
					Socket_Write_Answer_CB(pMe);
				}
				continue;
			}//end if 2
			
		}//end while

		if(m_HeadFlag == 1){
			Socket_WriteCB((void*)pMe);
		}else{
			ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_ReadCB, (void*)pMe);
		}
		//������
		//
		return;
		// end if nRecvSize > 0
	}
	else // rv == 0 No more data
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_ReadCB  No more data");
		//��Ҫ����
		Socket_Clean_TCP(pMe);
	}

	return;
}

void Socket_Write_Answer_CB(void *p)
{
	newgimis *pMe = (newgimis*)p;
	int nWrite;
	int sendLen;
	JulianType dDate;
	char sendBuf[1024] ;
	char sdataBuf[128] ;

	TCP_HEAD s_head;
	MEMSET(&s_head,0x00,sizeof(TCP_HEAD));
	MEMCPY(s_head.heater,PACKET_TCP_ID_HEAD,4);
	s_head.type[0] = '3';
	
	MEMSET(sendBuf,0x00,sizeof(sendBuf));
	MEMSET(sdataBuf,0x00,sizeof(sdataBuf));

	GETJULIANDATE(0,&dDate);
	SPRINTF(sdataBuf,"%04d%02d%02d%02d%02d%02d%s",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond,pMe->MyMobileNo);
	
	s_head.Length = sizeof(TCP_HEAD) + STRLEN(sdataBuf);
	
	MEMCPY(sendBuf,&s_head,sizeof(TCP_HEAD));
	MEMCPY(sendBuf + sizeof(TCP_HEAD),sdataBuf,STRLEN(sdataBuf));
	
	sendLen = s_head.Length;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_Write_Answer_CB =====send pack=====");
	
	if (pMe->pISocket_TCP_yyxf == NULL)
	{
		Socket_Clean_TCP(pMe);
		return;
	}
	
	nWrite = ISOCKET_Write(pMe->pISocket_TCP_yyxf, (byte*)sendBuf, sendLen);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Socket_Write_Answer_CB nWrite = %d",nWrite);
	
	if (nWrite == AEE_NET_WOULDBLOCK)
	{
		ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_Write_Answer_CB, (void*)pMe);
		return;
	}
	else if (nWrite == AEE_NET_ERROR) 
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Write_Answer_CB Failed: Error %d\n", ISOCKET_GetLastError(pMe->pISocket_TCP_yyxf));
		Socket_Clean_TCP(pMe);
		return;
	}
	else if (nWrite > 0) // The actual data that was read
	{
		pMe->m_Listen_Heart = GETTIMEMS();	
		
		if (nWrite == sendLen)	//�����Ƿ����ֲ���������ȣ�����������û���ڷ������������رա�
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Write_Answer_CB send %d success",sendLen);
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_Write_Answer_CB  ISOCKET_Readable");
			ISOCKET_Readable(pMe->pISocket_TCP_yyxf, Socket_Write_Answer_CB, (void*)pMe);
		}
		return;
	}
	else // rv == 0 No more data
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Write_Answer_CB  No more data");
		
		//��Ҫ����
		Socket_Clean_TCP(pMe);
	}
	
	return;
}

void Socket_Listen_Heart(void *p)
{
	newgimis *pMe = (newgimis*)p;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_Listen_Heart time:%u",GETTIMEMS()-pMe->m_Listen_Heart );

	if (GETTIMEMS() - pMe->m_Listen_Heart >= HEART_TIME*1000)
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Listen_Heart  ����2����û���յ�����");
		//��Ҫ����
		Socket_Clean_TCP(pMe);
	}
	else
	{
		if (pMe->pISocket_TCP_yyxf != NULL)
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, HEART_TIME*1000, (PFNNOTIFY)Socket_Listen_Heart, (uint32*) pMe);
		}
	}

	return;
}

void Socket_Clean_TCP(newgimis *pMe)
{	
	int ret = 0;

	pMe->TCP_Status_yyxf = 0;

	ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)Socket_Listen_Heart, (uint32*) pMe); //ȡ����ʱ��
	
	if (pMe->pISocket_TCP_yyxf != NULL)
	{
		ret =ISOCKET_Close(pMe->pISocket_TCP_yyxf);
	}

	//ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," ISOCKET_Close ret = %d",ret);
	
	if (ret == AEE_NET_SUCCESS)
	{
		if (pMe->pISocket_TCP_yyxf) 
		{
			ISOCKET_Release(pMe->pISocket_TCP_yyxf);
			pMe->pISocket_TCP_yyxf = 0;
		}

		if (pMe->pINetMgr_TCP_yyxf) 
		{
			INETMGR_Release(pMe->pINetMgr_TCP_yyxf);
			pMe->pINetMgr_TCP_yyxf = 0;
		}
		
	    ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Clean_TCP  success");
		ISHELL_SetTimer(pMe->a.m_pIShell, RESTART_TIME*1000, (PFNNOTIFY)Scoket_init_TCP, (uint32*) pMe);
	}
	else if (ret == AEE_NET_WOULDBLOCK)
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Socket_Clean_TCP  AEE_NET_WOULDBLOCK  reclose");
		ISHELL_SetTimer(pMe->a.m_pIShell, RESTART_TIME*1000, (PFNNOTIFY)Socket_Clean_TCP, (uint32*) pMe);
	}
	return;

}

//-------��ʼ��������Ƶ�·�����----
void initModule_yyxf(newgimis *pMe)
{
	//Ҫ��ɵ���
	//1����ʼ��HTTPȥ�����Ƿ���¼���·�
	//2����ʼ������
	//3�����������ϵĴӶ�����ȡ��������UDP��

	initQueue(pMe);		//�������г�ʼ��
	SMSInit_YYXF(pMe);	//���Ź��ܳ�ʼ��

	MEMSET(pMe->sUrl_YYXF,0x00,250);
	STRCAT(pMe->sUrl_YYXF,"http://");
	STRCAT(pMe->sUrl_YYXF,pMe->sHttp_YYXF);
	STRCAT(pMe->sUrl_YYXF,"&ver=3.1.2au");
	
	if (STRLEN(pMe->MyMobileNo) > 0) 
	{
		STRCAT(pMe->sUrl_YYXF,"&tel=");
		STRCAT(pMe->sUrl_YYXF,pMe->MyMobileNo);
		STRCAT(pMe->sUrl_YYXF,"&imsi=");
		STRCAT(pMe->sUrl_YYXF,pMe->szMobileID);
	}

    ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY) coreModule_yyxf, (uint32*) pMe);	//UDP�����ļ����沥�� ��Ҫ����
	//myiweb_start_yyxf(pMe,pMe->sUrl_YYXF,0);	//HTTPͨ�Ż�ȡ
	 Scoket_init_TCP(pMe);

	return;
}

//����Ӷ���ȡ����-UDP����
void coreModule_yyxf(newgimis *pMe)
{
	if (  pMe->pISocket_UDP == NULL &&  pMe->list_head_UDP == NULL   )	//UDPû������ʱ�� �Ҵ���ļ��������ͷź�ſ�ʼ�µ�һ��UDP����
	{
		YYXF_INFO  t_httpInfo;
		MEMSET(&t_httpInfo, 0x00, sizeof(YYXF_INFO));

		if ( outQueue(pMe,&t_httpInfo) )	//����������
		{	
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"coreModule_yyxf queue exist audio !");
			//��Ƶ���Ŵ���
			pMe->gi_yyxfCount = t_httpInfo.icount;
			//��֪д���ļ�������
			MEMSET(pMe->gs_yyxfAudioName, 0x00, sizeof(pMe->gs_yyxfAudioName));
			STRCPY(pMe->gs_yyxfAudioName,t_httpInfo.cName);

			pMe->iflag_YYXF = 0;	//ȷ��ͷ��ֻ����һ��
			pMe->iCount_YYXF = 0;
			Scoket_init_UDP(pMe);	
		}	
	}

	//ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"coreModule_yyxf continue ISHELL_SetTimer ");
	
	ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY) coreModule_yyxf, (uint32*) pMe);
	
	return;
}



//��ʱ����httpͨ��
void setTimerHttp_yyxf(newgimis *pMe)
{
	myiweb_start_yyxf(pMe,pMe->sUrl_YYXF,0);
}


//-------------------HTTP----------------------

void myiweb_start_yyxf(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];

	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"myiweb_start_yyxf into ");
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests 
   pMe->pszPostData_yyxf = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData_yyxf) 
   {
      *pMe->pszPostData_yyxf = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_yyxf + 1, 
                                    STRLEN(pMe->pszPostData_yyxf + 1), 0, 0,
                                    &pMe->pipPostData_yyxf);
         ISOURCEUTIL_Release(pisu);
      }
   }
   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_yyxf);
	   CALLBACK_Init(&pMe->cb_yyxf, myiweb_GotResp_yyxf, pMe);
   }
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_yyxf)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_yyxf));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)60000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;

		awo[i].nId  = WEBOPT_FLAGS;
		awo[i].pVal = (void *)WEBRESPONSE_KEEPALIVE;
		i++;
			
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;

		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_yyxf,awo);
   }

   if ((IPeek *)0 != pMe->pipPostData_yyxf) {
      IWEB_GetResponse(pMe->m_pIWeb_yyxf,
                       (pMe->m_pIWeb_yyxf, &pMe->piWResp_yyxf, &pMe->cb_yyxf, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_yyxf, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_yyxf, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_yyxf,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_yyxf + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_yyxf,
                       (pMe->m_pIWeb_yyxf, &pMe->piWResp_yyxf, &pMe->cb_yyxf, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n ",//Connection: Keep-Alive\r\n
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_yyxf, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_yyxf, 
                        WEBOPT_END));
   }
}

static void myiweb_GotResp_yyxf(void *p)
{
	char sLogBuf[257];
	int nLen =0;
	newgimis *pMe = (newgimis *)p;
	WebRespInfo *pInfo;

	SPRINTF(sLogBuf,"myiweb_GotResp_yyxf,Got Message, time:%u",(GETTIMEMS()-pMe->iPostPhTm_yyxf));
	uf_RecLog(pMe, sLogBuf);

	pMe->iPostPhTm_yyxf = 0 ;				// ���յ�Ӧ��
	
	pInfo = IWEBRESP_GetInfo(pMe->piWResp_yyxf);

	SPRINTF(sLogBuf, "myiweb_GotResp_yyxf response, code: %d",pInfo->nCode);
	nLen = STRLEN(sLogBuf);
	uf_RecLog(pMe,sLogBuf);

	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) {  
		
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"myiweb_GotResp_yyxf response, code: %d",pInfo->nCode);
		myiweb_Stop_yyxf(pMe);
		//���ִ���ʱ����ʱ5s
		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) setTimerHttp_yyxf, (uint32*) pMe);
		return;
	}
	
	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 
		{
			myiweb_Stop_yyxf(pMe);
			//���ִ���ʱ����ʱ5s
			ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) setTimerHttp_yyxf, (uint32*) pMe);
			return; 
		}
	      
		
		pMe->pSource_yyxf = pInfo->pisMessage;
		ISOURCE_AddRef(pMe->pSource_yyxf);				// ---- ��Read Body���õ�
		// 2.6.2D2		IWEBRESP_Release(me->piWResp);
		// 2.6.2D2		me->piWResp = 0;
		{
			newgimis * pMe; 
			int n=0;			// 2.6.4D2
			uint32 nErr = IWEBRESP_Release(pMe->piWResp_yyxf);

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"myiweb_GotResp_yyxf in,clear piWResp,return=%d,n=%d",nErr,n);
			pMe->piWResp_yyxf = NULL;
		}
		
		CALLBACK_Cancel(&pMe->cb_yyxf);
		CALLBACK_Init(&pMe->cb_yyxf, myiweb_ReadBody_yyxf, pMe);
		myiweb_ReadBody_yyxf(pMe);
	}else{
		myiweb_Stop_yyxf(pMe);
		myiweb_start_yyxf(pMe,pMe->sUrl_YYXF,0);
	}
}

static void myiweb_ReadBody_yyxf(newgimis *pMe)
{
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	char *sBuf_T = NULL;
	char *sBuf1 = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"coming myiweb_ReadBody_yyxf");

	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);
	
	nRead = ISOURCE_Read(pMe->pSource_yyxf, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		ISOURCE_Readable(pMe->pSource_yyxf, &pMe->cb_yyxf);
		//stop

		return;
	}
	
	if (nRead > 0)		
	{
		SPRINTF(sc_Count,"%d",nRead);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sBuf); 
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," nRead = %d",nRead); 
		
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
			nRead = ISOURCE_Read(pMe->pSource_yyxf, sBuf1, 1023);
			STRCAT(sBuf_T,sBuf1);
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"while nRead myiweb_ReadBody_yyxf = %d",nRead); 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sBuf1); 
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sBuf_T  = %d ",STRLEN(sBuf_T)); 
		uf_RecLog(pMe,sBuf_T);
		
		//uf_fromESC(sBuf_T,sBuf_d);

		si_Ret = resolvePack(pMe,sBuf_T);

		FREEIF(sBuf_T);
		sBuf_T = NULL ;
		
		FREEIF(sBuf1);
		sBuf1 = NULL ;
		
		switch (si_Ret)  //˵���ɹ�
		{
		case 0:
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"myiweb_ReadBody_yyxf = %d ",si_Ret); 
				//����������
			}
			break;	
		case 1:
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"myiweb_ReadBody_yyxf = %d ",si_Ret); 
				//�����ظ� ����Ƶ�·�
			}
			break;
		case 2:
			{
				//�����ظ� ����Ƶ�·�
				YYXF_INFO t_info;
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"myiweb_ReadBody_yyxf = %d  ��Ƶ�·� ",si_Ret); 
				STRCPY(t_info.cTel,pMe->MyMobileNo);
				STRCPY(t_info.cName,pMe->m_audioName);
				t_info.icount = pMe->m_audioCount;

				inQueue(pMe,t_info);
			}
			break;
		default:
			break;
		}
	
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
	}
	myiweb_Stop_yyxf(pMe);

	myiweb_start_yyxf(pMe,pMe->sUrl_YYXF,0);

}

int resolvePack( newgimis *pMe,char *pString)
{

	int si_ret = 0;	//���ݷ���ֵ�жϴ������0ʧ�� 1�����ظ�û����Ƶ�·�  2����Ƶ�·�
	int si_Count = 0;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_tmp = NULL;
	char sc_one;
	char s_MobileNo[64+1];
	char *sbuf = NULL;
	
	MEMCPY(&sc_one,pString,1);
	if (sc_one == '1')
	{
		//��˵�����ճɹ���ȷ��������Ϣ
		si_ret = 1;
		
		// ���ҵ绰����
		sc_Start = STRSTR(pString,"1dectel");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start,";");
			si_Count = sc_End - sc_Start - 7;
			MEMSET(s_MobileNo,0x00,sizeof(s_MobileNo));
			STRNCPY(s_MobileNo,sc_Start+7,si_Count);
			
			if (STRCMP(s_MobileNo,pMe->MyMobileNo) != 0)	//��������뱾�����벻ƥ�������˴�http����ʧ��������
			{
				si_ret = 0;
				return;
			}
		}
		
		//�·���Ƶ�貥�ż���   m_audioCount  �·���Ƶ�貥�ż���
		sc_Start = STRSTR(pString,";CS");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_audioCount = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			
			si_ret = 2;
		}
		
		// m_audioName //��Ƶ����
		sc_Start = STRSTR(pString,";AN");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			MEMSET(pMe->m_audioName,0x00,sizeof(pMe->m_audioName));//bug
			STRNCPY(pMe->m_audioName,sc_Start+3,si_Count);

			si_ret = 2;
		}

		// m_audioName //��Ƶ����
		sc_Start = STRSTR(pString,";SM");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sbuf = (char *)MALLOC(si_Count+1);
			MEMSET(sbuf, 0x00 ,si_Count+1);
			STRNCPY(sbuf,sc_Start+3,si_Count);

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),";SM = sbuf  = %s",sbuf); 
			
			pMe->aeContent_yyxf = (AECHAR *)MALLOC(si_Count);
			UTF8_TO_WSTR((byte *)sbuf,si_Count,pMe->aeContent_yyxf,si_Count);
			SmsSave_YYXF(pMe);

			FREE(sbuf);
		}
	}
	
	return si_ret;
}

static void myiweb_Header_yyxf(void *p, const char *cpszName, GetLine *pglVal)
{
	   newgimis *pwa = (newgimis *)p;
	   
	   //  if (pMe->m_bRS) { // If response is to be displayed
	   if ((char *)0 != cpszName) {
		   // ND_Print(pMe, "%s:", cpszName);
	   }
	   //ND_Print(pMe, "%s\n", pglVal->psz);
	   // }
}

static void myiweb_Status_yyxf(void *p, WebStatus ws, void *pVal)
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
		   DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
		   break;          
	   case WEBS_CONNECT:
		   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
		   break;
	   case WEBS_SENDREQUEST:
		   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
		   
		   pMe->iPostPhTm_yyxf = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_yyxf, pMe);
		   break;
	   case WEBS_READRESPONSE:
		   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
		   DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
		   break;
	   case WEBS_GOTREDIRECT:
		   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
		   break;
	   case WEBS_CACHEHIT:
		   ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
		   DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
		   break;
		   
		   
	   default:
		   if (pMe->iPostPhTm_yyxf == 0) {			//  Add 2013-09-23
			   pMe->iPostPhTm_yyxf = GETTIMEMS();
			   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_yyxf, pMe);
		   }		// Add End
		   break;
	   }
	   
	   // show that status!
	   if ((AECHAR *)0 != szText) 
	   {
		   //    myiweb_DisplayInfo(pMe,szText);
		   DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
	   }
}


static void uf_MonitorPost_yyxf(newgimis *pMe)                        // (WebAction * pwa)		// b61 2012-02-09		
{
	if ( pMe->iPostPhTm_yyxf == 0)
		return;

	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost_yyxf in, http time out");

	 myiweb_Stop_yyxf(pMe);

	 myiweb_start_yyxf(pMe,pMe->sUrl_YYXF,0);
}

static void myiweb_Stop_yyxf(newgimis *pwa)
{
	// this cancels any pending web transaction, or readable on the 
	// response body.  if nothing is pending, this has no effect
	CALLBACK_Cancel(&pwa->cb_yyxf);
	
	// then clean up, if necessary
	if ((char *)0 != pwa->pszPostData_yyxf) {
		
		FREE(pwa->pszPostData_yyxf);
		pwa->pszPostData_yyxf = NULL;
		//		pwa->iFileRead = 0;
		
		//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
		//     pwa->pszPostData_xt = 0;
	}
	// ����  
	if (pwa->pipPostData_yyxf != NULL)
	{
		IPEEK_Release(pwa->pipPostData_yyxf);
		// Add by zjie 2011-01-30
		pwa->pipPostData_yyxf = NULL;
		
	}
	// ����  
	if (pwa->pSource_yyxf != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_yyxf);	// 2013-04-12 �� nErr
		//ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"myiweb_Stop_gps ISOURCE_Release nErr = %d",nErr);
		pwa->pSource_yyxf = NULL;
	}
	
	
	if (pwa->piWResp_yyxf)
	{
		IWEBRESP_Release(pwa->piWResp_yyxf);
		pwa->piWResp_yyxf= NULL;
	}
	
	if (pwa->m_pIWeb_yyxf) 
	{
		//		IWEB_Release(pwa->m_pIWeb_xt);
		//		pwa->m_pIWeb_xt = NULL;
		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_yyxf);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_yyxf);
		}
		
		pwa->m_pIWeb_yyxf = NULL;
	}
	
	if (pwa->m_BodyBuffer_yyxf)
	{
		FREE(pwa->m_BodyBuffer_yyxf);
		pwa->m_BodyBuffer_yyxf = NULL;
	}
	pwa->m_BodySize_yyxf = 0;
	pwa->m_BodyAllocSize_yyxf = 0;
	
}

//-------------------UDP----------------------
void Scoket_init_UDP(newgimis *pMe)
{
	int nErr = 0;
	
	if (!pMe->pINetMgr_UDP)
	{
		//ʵ��
		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_NET, (void **)&(pMe->pINetMgr_UDP) );
		if (SUCCESS != nErr) 
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "scoket_init_UDP ISHELL_CreateInstance err=%d", nErr);
			goto ExitHere;
		}
		
		//��socket�õ��׽���
		pMe->pISocket_UDP = INETMGR_OpenSocket(pMe->pINetMgr_UDP,AEE_SOCK_DGRAM);
		if ( !pMe->pISocket_UDP) //AEE_SOCK_DGRAM
		{
			nErr = INETMGR_GetLastError(pMe->pINetMgr_UDP);
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"INETMGR_OpenSocket fail nErr = %d",nErr);
			goto ExitHere;
		}

		pMe->list_head_UDP = LIST_Init(NULL);	//��Ƶ���ݰ����

		pMe->overTime_YYXF = 0;	//��ʼ����ʱ��¼
		
		//����������
		Socket_sendPack_UDP(pMe);
		//Socket_readPack_UDP(pMe);
		ISHELL_SetTimer(pMe->a.m_pIShell,100, (PFNNOTIFY)Socket_readPack_UDP, pMe);
	}
	
ExitHere:
	if (SUCCESS != nErr)
	{
		Socket_Clean_UDP(pMe);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"scoket_init fail nErr = %d",nErr);
	}
	return;
}



static void Socket_sendPack_UDP(void *p)
{
	int nErr = 0;
	INAddr nodeINAddr;
	uint16 nPort;

	JulianType dDate;
	char sLogBuf[TEXT_MAXSIZE];
	
	newgimis *pMe = (newgimis*)p;

	INET_ATON(pMe->sUDPIP_YYXF,&nodeINAddr);
	nPort = HTONS(pMe->iUDPPORT_YYXF); 

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"=====send pack===== %s",pMe->gs_yyxfAudioName);

	MEMSET(sLogBuf,0x00,sizeof(sLogBuf));

	//sLOGbuf ����+�ֻ���+������Ƶ�ļ���
	GETJULIANDATE(0,&dDate);
	SPRINTF(sLogBuf,"%04d%02d%02d%02d%02d%02d%s%s",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond,pMe->MyMobileNo,pMe->gs_yyxfAudioName);

	nErr = ISOCKET_SendTo(pMe->pISocket_UDP, (byte*)sLogBuf, STRLEN(sLogBuf), 0, 
		nodeINAddr, nPort);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_sendPack_UDP nErr = %d ",nErr);

	if (nErr == AEE_NET_WOULDBLOCK) {
		ISOCKET_Writeable(pMe->pISocket_UDP, (PFNNOTIFY)Socket_sendPack_UDP, pMe);
	} else if (nErr == AEE_NET_ERROR) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_sendPack_UDP AEE_NET_ERROR");
		Socket_Clean_UDP(pMe);
		FREE(pMe->list_head_UDP);	//��֤�´�coreModule_yyxf�ܽ��롣
		pMe->list_head_UDP = NULL;
	} else {         
		// Reset Buffer
		//��һ����ʱ������Ƿ���ճ�ʱ
		pMe->overTime_YYXF = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, DEFAULT_OVERTIME * 1000, (PFNNOTIFY)readOvertime_yyxf, pMe);

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_sendPack_UDP success");
	}

	return;
}

void readOvertime_yyxf(newgimis *pMe)
{
	if (pMe->overTime_YYXF == 0)
	{
		return;
	}
	else
	{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"readOvertime_yyxf over time:%u",GETTIMEMS()-pMe->overTime_YYXF );
		
		if (pMe->pISocket_UDP)
		{
			Socket_Clean_UDP(pMe);
			FREE(pMe->list_head_UDP);	//��֤�´�coreModule_yyxf�ܽ��롣
			pMe->list_head_UDP = NULL;
		}
	}

	return;
}

void Settimer_ReadUdpData(newgimis *pMe)
{
	if (pMe->readUdpTime_YYXF == 0)
	{
		return;
	}
	else{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Settimer_ReadUdpData over time:%u",GETTIMEMS()-pMe->readUdpTime_YYXF );

		if (pMe->pISocket_UDP)	//��Ҫ�ж����Ƿ���ͶϿ��ˣ��п��ܽ��ղ�������ʱ��ϵ����ӣ���û��ȡ����ʱ��
		{
			pMe->iCount_YYXF = 0;
			Socket_Clean_UDP(pMe);
		    write_udp_thread(pMe);
		}
	}

	return;
}

//��UDP��
static void Socket_readPack_UDP(void *p)
{
	int nRead;

	newgimis *pMe = (newgimis*)p;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"=====rev pack=====");

	MEMSET(pMe->m_readMsg,0,10240);
	nRead = ISOCKET_RecvFrom(pMe->pISocket_UDP, (byte *)pMe->m_readMsg, 
		10240,0,0,0);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP nRead = %d ",nRead);
	if (nRead == AEE_NET_ERROR) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP AEE_NET_ERROR");
		Socket_Clean_UDP(pMe);
		FREE(pMe->list_head_UDP);	//��֤�´�coreModule_yyxf�ܽ��롣
		pMe->list_head_UDP = NULL;
		return;
	} else {
		if (nRead == AEE_NET_WOULDBLOCK) {

			if ( (pMe->list_head_UDP)->pNext != NULL )		//�����������ڷ���AEE_NET_WOULDBLOCK����ʱ����д�ļ�������
			{												//��������ͷ���жϣ����Ա���udpû�յ�ǰ�յ�AEE_NET_WOULDBLOCK��֪ͨ
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP ������� Socket_Clean_UDP");
				pMe->iCount_YYXF = 0;
				Socket_Clean_UDP(pMe);
				write_udp_thread(pMe);
				return;
			}
			ISOCKET_Readable(pMe->pISocket_UDP, (PFNNOTIFY)Socket_readPack_UDP, pMe);
			return;
		} else {
			//������
// 			char temp[100] = {0};
// 			MEMCPY(temp)
			int *pIntfirst = (int *)pMe->m_readMsg;
			
			switch(*pIntfirst)
			{
			case 1:
				{
					char sc_tmp;
					STRNCPY(&sc_tmp,pMe->m_readMsg+1,1);

					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP UDP������ %d",sc_tmp);
				}
				break;
			case 2:
				{
// 					int index; //���͵�ǰ�İ�
// 					int total; //�ܰ���
// 					int bsize; //�ôη������ݳ��ȴ�С

					UDP_INFO *u_info = NULL;
					u_info = (UDP_INFO*)MALLOC(sizeof(UDP_INFO));
					MEMSET(u_info,0,sizeof(UDP_INFO));
					MEMCPY(u_info,pMe->m_readMsg,nRead);

					pMe->overTime_YYXF = 0;	//�������ݼ�û�г�ʱ��

					ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)readOvertime_yyxf, (uint32*) pMe); //(��������10s���յ��κ�һ����)
					
					pMe->iCount_YYXF++;
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"u_info->total = %d u_info->index = %d u_info->bsize = %d",u_info->total,u_info->index,u_info->bsize);
					if (pMe->iCount_YYXF == u_info->total + 1)
					{
						if (u_info->index != 1 )
						{
							LIST_Add(pMe->list_head_UDP,u_info);
						}

						pMe->readUdpTime_YYXF = 0;
						ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY)Settimer_ReadUdpData, (uint32*) pMe);//�涨ʱ���ڽ�������رն�ʱ��
						
						pMe->iCount_YYXF = 0;
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP Socket_Clean_UDP");
						Socket_Clean_UDP(pMe);
						write_udp_thread(pMe);
						return;
					}

					if ( u_info->index == 1  )
					{
						if (pMe->iflag_YYXF == 0)
						{
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_readPack_UDP audio index 1 ");
							LIST_Add_front(pMe->list_head_UDP,u_info);
							pMe->iflag_YYXF = 1;

							pMe->readUdpTime_YYXF = GETTIMEMS();
							//�յ���һ�����ݰ� ��ʼ����ʱ 10S�ڹرմ˴����� UDPDATA_OVERTIME
							ISHELL_SetTimer(pMe->a.m_pIShell, UDPDATA_OVERTIME * 1000, (PFNNOTIFY)Settimer_ReadUdpData, pMe);
						}

						ISOCKET_Readable(pMe->pISocket_UDP, (PFNNOTIFY)Socket_readPack_UDP, pMe);
						return;
					}

					LIST_Add(pMe->list_head_UDP,u_info);
				}
				break;
			default:
				break;
			}
			
			ISOCKET_Readable(pMe->pISocket_UDP, (PFNNOTIFY)Socket_readPack_UDP, pMe);
		}
	}
	return;
}

void Socket_Clean_UDP(newgimis *pMe)
{	
	if (pMe->pISocket_UDP) 
	{
		ISOCKET_Release(pMe->pISocket_UDP);
		pMe->pISocket_UDP = NULL;
	}
	
	if (pMe->pINetMgr_UDP) 
	{
		INETMGR_Release(pMe->pINetMgr_UDP);
		pMe->pINetMgr_UDP = NULL;
	}
	return;
}

//��udp�յ��İ�,д���ļ���
void write_udp_thread(newgimis *pMe)
{
	pList pTmp = NULL;

	char file_name[256] = {0};
	char *sc_Start = NULL;
	char *sc_tmp = NULL;
	int si_Count = 0;


	//ȥ��Ŀ¼\֮ǰ�Ķ���
	sc_tmp = (char *)MALLOC(100);
	MEMSET(sc_tmp,0x00,100);
	si_Count = ud_rDistToChar(pMe->gs_yyxfAudioName,'\\');
	if (si_Count >= 0)
	{
		STRCPY(sc_tmp,pMe->gs_yyxfAudioName+si_Count+1);
		MEMSET(pMe->gs_yyxfAudioName, 0x00, sizeof(pMe->gs_yyxfAudioName));
		STRCPY(pMe->gs_yyxfAudioName,sc_tmp);
	}
	FREE(sc_tmp);

	//����Ƶ��׺��Ϊqcp
	sc_Start = STRSTR(pMe->gs_yyxfAudioName,".");
	if(sc_Start != NULL)
	{
		si_Count = sc_Start - pMe->gs_yyxfAudioName;
		MEMSET(file_name,0x00,sizeof(file_name));
		STRNCPY(file_name,pMe->gs_yyxfAudioName,si_Count);
		SPRINTF(pMe->gs_yyxfAudioName,"%s.qcp",file_name);
	}
	else{
		return;
	}

	pTmp = pMe->list_head_UDP;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," write_udp_thread start name =%s ",pMe->gs_yyxfAudioName);
	while (pTmp->pNext != NULL)
	{
		pTmp = pTmp->pNext;
		write_UDP_Date(pMe,((UDP_INFO*)(pTmp->pData))->buffer,((UDP_INFO*)(pTmp->pData))->bsize);
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_udp_thread end");

	SoundStatus_YYXF(pMe);
// 
// 	LIST_Free(pMe->list_head_UDP);
// 	pMe->list_head_UDP = NULL;
	

// 	//�µ��ļ������������������Ƶ����
// 	if (pMe->pIPlayer != NULL)
// 	{
// 		ISOUNDPLAYER_Stop(pMe->pIPlayer);//��ֹͣ�����벥��
// 		//ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY) SoundPlay_YYXF, (uint32*) pMe);
// 	}
// 	SoundPlay_YYXF(pMe);
//	SoundStatus_YYXF(pMe);
//	ISHELL_SetTimer(pMe->a.m_pIShell,100, (PFNNOTIFY) SoundStatus_YYXF, (uint32*) pMe);
	
//	ISHELL_SetTimer(pMe->a.m_pIShell,100, (PFNNOTIFY) SoundPlay_YYXF, (uint32*) pMe);
	return;
}

void write_UDP_Date(newgimis *pMe, char *strData,int ilen)
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
	char file_name[256] = {0};

	SPRINTF(file_name,"%s",pMe->gs_yyxfAudioName);	//m_senMsg�����ڼӱ���
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_UDP_Date file_name is %s",file_name);
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
		int si_Count = 0;
		FileInfo fiInfo;
		
		if ( IFILE_GetInfo( pTempFile, &fiInfo ) == SUCCESS) 
		{
			IFILE_Seek(pTempFile,_SEEK_END,0);

			si_Count = IFILE_Write(pTempFile, strData, ilen); 
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_GPSData_Day IFILE_Write data is %d",si_Count);
		}
	}
	
	if (pTempFile != NULL)
		IFILE_Release( pTempFile );
	
	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );
	
	return ;
}

//////////////////////////////////////////
void SoundPlay_YYXF(newgimis* pMe)
{
	AEESoundPlayerInfo info;
	info.eInput = SDT_FILE;
	//info.pData ="cari.mp3";/*"Voice_001.mp3"*/
	MEMSET(pMe->gs_yyxfPlayName, 0x00, sizeof(pMe->gs_yyxfPlayName));	//���ź�ɾ��ʱ���õ��� ��Ƶ����
	if (pMe->gs_yyxfAudioName != NULL)
	{
		STRCPY(pMe->gs_yyxfPlayName, pMe->gs_yyxfAudioName);
		info.pData = pMe->gs_yyxfPlayName;
	}else{
		return;
	}
	
	//����SOUNDPLAYʵ��
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOUNDPLAYER,(void **)&pMe->pIPlayer) != SUCCESS )
	{
		IAPPLET_Release((IApplet*)pMe);
		return ;
	}
	//ע��ص�
	ISOUNDPLAYER_RegisterNotify(pMe->pIPlayer,CBSoundPlay_YYXF,(void *)pMe);
	//��������Ϊ���
	ISOUNDPLAYER_SetVolume(pMe->pIPlayer, AEE_MAX_VOLUME);
	//����
	if (ISOUNDPLAYER_SetInfo(pMe->pIPlayer,&info)  == SUCCESS)
	{
		if (pMe->pIPlayer != NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"SoundPlay_YYXF ISOUNDPLAYER_Play");
			ISOUNDPLAYER_Play(pMe->pIPlayer);
		}
	}
}

void SoundStatus_YYXF(newgimis* pMe)
{
	int iStatus  = 0;

	if (pMe->pIPlayer)
	{
		//ISoundPlayer �Ƿ���æ�����Ե��ã� int ISOUNDPLAYER_GetInfo(pISoundPlayer, NULL); ��� ISoundPlayer ���У��򷵻� EIDLE�������æ���򷵻� EITEMBUSY�� 
		iStatus = ISOUNDPLAYER_GetInfo(pMe->pIPlayer, NULL);
		switch( iStatus )
		{
		case EIDLE:
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SoundStatus_YYXF EIDLE");
				//������Ӧ�ò����� ��Ϊ������Ͼ��ͷ�pMe->pIPlayer  
				ISOUNDPLAYER_Stop(pMe->pIPlayer);//��ֹͣ�����벥��
				ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY) SoundPlay_YYXF, (uint32*) pMe);
			}
			break;
		case EITEMBUSY:
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SoundStatus_YYXF EITEMBUSY");
				ISOUNDPLAYER_Stop(pMe->pIPlayer);//��ֹͣ�����벥��
				ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY) SoundPlay_YYXF, (uint32*) pMe);
			}
			break;
		default:
			break;
		}
	}else{
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"SoundStatus_YYXF ׼��");
		SoundPlay_YYXF(pMe);
	}
	return;
}

void CBSoundPlay_YYXF(void *p,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam)
{
    newgimis* pMe = (newgimis*)p;
	AEESoundPlayerCmdData * pData = (AEESoundPlayerCmdData *) dwParam;

	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CBSoundPlay_YYXF into eCBType = %d eSPStatus = %d");

	switch(eCBType)
	{
	case AEE_SOUNDPLAYER_PLAY_CB:
		switch(eSPStatus)
		{
		case AEE_SOUNDPLAYER_SUCCESS://��������
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_SUCCESS");
			break;
		case AEE_SOUNDPLAYER_DONE://������ִ�����
			
			ISOUNDPLAYER_RegisterNotify(pMe->pIPlayer, NULL, NULL); //�رջص�
			ReleaseObj((void **)&pMe->pIPlayer);//�ͷ�

			removeFile_YYXF(pMe,pMe->gs_yyxfPlayName);	//ֹͣ�Ͳ��Ž�����ɾ���ļ���

			LIST_Free(pMe->list_head_UDP);
			pMe->list_head_UDP = NULL;

			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_DONE");
			break;
		case AEE_SOUNDPLAYER_ABORTED:	//������ֹ
			
			ReleaseObj((void **)&pMe->pIPlayer);//�ͷ�
			
			removeFile_YYXF(pMe,pMe->gs_yyxfPlayName);	//ֹͣ�Ͳ��Ž�����ɾ���ļ���
			
			LIST_Free(pMe->list_head_UDP);
			pMe->list_head_UDP = NULL;
			
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_ABORTED");
			
			break;
		case AEE_SOUNDPLAYER_FAILURE://�������
			
			ReleaseObj((void **)&pMe->pIPlayer);//�ͷ�
			
			removeFile_YYXF(pMe,pMe->gs_yyxfPlayName);	//ֹͣ�Ͳ��Ž�����ɾ���ļ���
			
			LIST_Free(pMe->list_head_UDP);
			pMe->list_head_UDP = NULL;
			
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_FAILURE");
			
			break;
		default:
			break;
		}
		break;
		
	case AEE_SOUNDPLAYER_STATUS_CB:
		switch(eSPStatus)
		{
		case AEE_SOUNDPLAYER_SUCCESS://��������
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_SUCCESS");
			break;
		case AEE_SOUNDPLAYER_FAILURE://�������
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CBSoundPlay_YYXF AEE_SOUNDPLAYER_FAILURE");
			break;
		default:
			break;
		}
		break;
		
		default:
			break;
	}
	
}

void removeFile_YYXF(newgimis *pMe,char *filename)
{
	IFileMgr* pFileMgr = NULL;
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		return;
	}
	if ( IFILEMGR_Remove(pFileMgr, filename) == SUCCESS ) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"removeFile  %s success",filename);
	}
	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
	return;
}
///////////////////////////////////////////////////////


/* 
 *��ʼ��������ʵ�� 
 */  
void initQueue(newgimis *pMe)  
{  
    //�����ڴ�  
    pMe->pQueue.pBase = (YYXF_INFO *)MALLOC(sizeof(YYXF_INFO) * 20);          //����10����ռ�Ŀռ�  

	while(pMe->pQueue.pBase == NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"initQueue malloc fail");
		pMe->pQueue.pBase = (YYXF_INFO *)MALLOC(sizeof(YYXF_INFO) * 20); 
	}
	
    pMe->pQueue.front = 0;       
    pMe->pQueue.rear = 0;  
    return;  
}  
/* 
 *��Ӻ�����ʵ�� 
 */  
int inQueue(newgimis *pMe, YYXF_INFO t_info)  
{  
    if(isFull(pMe))  
    {  
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"outQueue isFull ");   
        return 0;  
    }  
    else  
    {  
        //������������Ԫ��  
        pMe->pQueue.pBase[pMe->pQueue.rear] = t_info;  
        //��rear�����µĺ��ʵ�ֵ  
        pMe->pQueue.rear = (pMe->pQueue.rear+1) % 20;  
        return 1;  
    }  
}  
/* 
 *���Ӻ�����ʵ�� 
 */  
int outQueue(newgimis *pMe, YYXF_INFO *t_info)  
{  
    //�������Ϊ��,�򷵻�false  
    if(isEmpty(pMe))  
    {  
        return 0;  
    }  
    else  
    {  
        *t_info = pMe->pQueue.pBase[pMe->pQueue.front];       //�Ƚ��ȳ�  
        pMe->pQueue.front = (pMe->pQueue.front+1) % 20;      //�Ƶ���һλ��  
        return 1;  
    }  
}  

/* 
 *�ж϶����Ƿ����ĺ�����ʵ�� 
 */  
int isFull(newgimis *pMe)  
{  
    if((pMe->pQueue.rear+1) % 20 == pMe->pQueue.front)     //������  
        return 1;  
    else  
        return 0;  
}  
/* 
 *�ж϶����Ƿ�Ϊ�պ�����ʵ�� 
 */  
int isEmpty(newgimis *pMe)  
{  
    if(pMe->pQueue.front == pMe->pQueue.rear)  
        return 1;  
    else  
        return 0;  
}

// ʵ�������Žӿ�
// ����ֵ��
//		�ɹ� ��SUCCESS, ʧ�� EFAILED
int SMSInit_YYXF(newgimis *pMe)
{
	int nErr = 0;
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSSTORAGE, (void**)&pMe->pISmsStorage_yyxf);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMSSTORAGE,nErr=%d",nErr);
		pMe->pISmsStorage_yyxf = NULL;
		nErr = EFAILED;
		goto bail;
	}	
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSMSG, (void**)&pMe->pISmsMsg_yyxf);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMSMSG,nErr=%d",nErr);
		pMe->pISmsMsg_yyxf = NULL;
		nErr = EFAILED;
		goto bail;
	}	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMS, (void**)&pMe->pISMS_yyxf);
    if (nErr != SUCCESS)
    {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMS,nErr=%d",nErr);
		pMe->pISMS_yyxf = NULL;
		nErr = EFAILED;
		goto bail;
    }
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SMSInit_YYXF ISHELL_CreateInstance,AEECLSID_SMSMSG and AEECLSID_SMSSTORAGE ok");
	
bail:
	return nErr;
}

void SmsSave_YYXF(newgimis * pMe)
{
	JulianType pDate;
	int nHours;	
	uint32 dwCreationDate;
	uint32 uErr = 0;
	AEESMSStorageType st;
	uint32 nIndex =0;
	SMSMsgOpt apwoz[10];
	char sCALLNO[65] = "88888";
	
	if ( (pMe->pISmsMsg_yyxf == NULL) || (pMe->pISmsStorage_yyxf == NULL) ) 
	{
		FREEIF(pMe->aeContent_yyxf);
		return;
	}
	
	nHours = pMe->iAddHour % 100;
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt nHours=%d",nHours);
	dwCreationDate = GETTIMESECONDS();
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt 1 time = %04d%02d%02d%02d%02d%02d",pDate.wYear,pDate.wMonth,pDate.wDay,pDate.wHour,pDate.wMinute,pDate.wSecond);
	dwCreationDate += (nHours*3600); 
	GETJULIANDATE(dwCreationDate,&pDate);		//	ȡ������	
	
	
	{
		int i = 0;
		//		me->m_SMSList.nCurId = idx;
		
		apwoz[i].nId  = MSGOPT_TO_DEVICE_SZ ;
		apwoz[i].pVal = (void *)(sCALLNO); 		//DESTINATION_NUMBER;
		i++;
		
		/* unicode text to be send */
		apwoz[i].nId  = MSGOPT_PAYLOAD_WSZ ;
		apwoz[i].pVal = (void *)(pMe->aeContent_yyxf);
		i++;
		
		/* encoding */
		apwoz[i].nId  = MSGOPT_PAYLOAD_ENCODING;
		apwoz[i].pVal = (void *)AEE_ENC_UNICODE ;
		i++;
		
		apwoz[i].nId  = MSGOPT_MOSMS_ENCODING;
		apwoz[i].pVal = (void *)AEESMS_ENC_UNICODE;
		i++;
		
		apwoz[i].nId =  MSGOPT_LOOPBACK_MSG;				// �ռ���
		apwoz[i].pVal = (void *)TRUE;
		i++;
		
		apwoz[i].nId = MSGOPT_TAG;							// δ��
		apwoz[i].pVal = (void *)AEESMS_TAG_MT_NOT_READ;
		i++;
		
		apwoz[i].nId = MSGOPT_TIMESTAMP;					// д��ʱ��
		apwoz[i].pVal = (void *)&pDate;
		i++;
		
		apwoz[i].nId = MSGOPT_END;
		ISMSMSG_AddOpt(pMe->pISmsMsg_yyxf,apwoz);
		
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"===============================================================");
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,sCALLNO=%s",sCALLNO);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok time = %04d%02d%02d%02d%02d%02d",pDate.wYear,pDate.wMonth,pDate.wDay,pDate.wHour,pDate.wMinute,pDate.wSecond);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,nContentLen=%d",me->m_SMSList.pSmsInfo[idx].nContentLen);
		
	}	
	
	st = AEESMS_NV_CDMA;
	CALLBACK_Init(&pMe->cbMsg_yyxf, SMSCB_YYXF, pMe);
	
	ISMSSTORAGE_StoreMsg(pMe->pISmsStorage_yyxf,st,pMe->pISmsMsg_yyxf,&pMe->cbMsg_yyxf,&nIndex,&uErr);
	//ISMSSTORAGE2_StoreMsg(me->pISmsStorage,st,me->pISmsMsg,&me->cbMsg,&nIndex,&uErr);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSSTORAGE_StoreMsg return=%d,index=%d",uErr,nIndex);

		
	return;
}

// ������ŵĻص�����
void SMSCB_YYXF(newgimis * pMe)
{
	FREEIF(pMe->aeContent_yyxf);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SMSCB_YYXF in");
	SmsAlert_YYXF(pMe);
	return;
}





// void Scoket_init_TCP(newgimis *pMe)
// {
// 	int nErr = 0;
// 	INAddr nodeINAddr;
// 	uint16 nPort;
// 
// 	if (!pMe->pINetMgr_TCP)
// 	{
// 		//ʵ��
// 		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_NET, (void **)&(pMe->pINetMgr_TCP) );
// 		if (SUCCESS != nErr) 
// 		{
// 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "scoket_init_TCP ISHELL_CreateInstance err=%d", nErr);
// 			goto ExitHere;
// 		}
// 		
// 		//��socket�õ��׽���
// 		pMe->pISocket_TCP = INETMGR_OpenSocket(pMe->pINetMgr_TCP,AEE_SOCK_STREAM);
// 		if ( !pMe->pISocket_TCP) //AEE_SOCK_DGRAM
// 		{
// 			nErr = INETMGR_GetLastError(pMe->pINetMgr_TCP);
// 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"INETMGR_OpenSocket fail nErr = %d",nErr);
// 			goto ExitHere;
// 		}
// 		
// 		//����
// 		INET_ATON(IP_TCP,&nodeINAddr);
// 		nPort = HTONS(TCP_PORT);
// 		ISOCKET_Connect(pMe->pISocket_TCP,nodeINAddr,nPort,(PFNCONNECTCB)Socket_ConnectCB_TCP,pMe);
// 	}
// 
// 
// ExitHere:
// 	if (SUCCESS != nErr)
// 	{
// 		Socket_Clean_TCP(pMe);
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"scoket_init fail nErr = %d",nErr);
// 	}
// 	
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Scoket_init_TCP success connect start ");
// 	return;
// }
// 
// static void Socket_ConnectCB_TCP(void* p,int error)
// {
// 	newgimis *pMe = (newgimis*)p;
// 
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ConnectCB_TCP error = %d ",error);
// 	
// 	if (error)
// 	{
// 		Socket_Clean_TCP(pMe);
// 		return;
// 	}
// 
// 	//Check error code
// 	switch(error)
// 	{
// 	case AEE_NET_ETIMEDOUT:
// 		// Connection timed out
// 		break;
// 	case AEE_NET_SUCCESS:
// 		// Send Some data
// 		Socket_ReadCB((void*)pMe);
// 		break;
// 	default:
// 		// Some other Network error
// 		break;
//    }
// }
// 
// static void Socket_WriteCB(void *p)
// {
// 	newgimis *pMe = (newgimis*)p;
// 	
// 	int nWrite;
// 	char szBuf[50];
// 	
// 	TCP_HEADER *t_tcphead =  (TCP_HEADER *)MALLOC(sizeof(TCP_HEADER ));
// 	MEMSET(t_tcphead, 0, sizeof(TCP_HEADER));
// 	
// 	nWrite = ISOCKET_Read(pMe->pISocket_TCP, (byte*)&pMe->m_pszMsg, 1023);
// 	
// 	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_WriteCB Read = %d",nWrite);
// 	
// 	if (nWrite == AEE_NET_WOULDBLOCK)
// 	{
// 		ISOCKET_Readable(pMe->pISocket_TCP, Socket_WriteCB, (void*)pMe);
// 		return;
// 	}
// 	else if (nWrite == AEE_NET_ERROR) 
// 	{
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_WriteCB Failed: Error %d\n", ISOCKET_GetLastError(pMe->pISocket_TCP));
// 		Socket_Clean_TCP(pMe);
// 		return;
// 	}
// 	else if (nWrite > 0) // The actual data that was read
// 	{
// 		ISOCKET_Readable(pMe->pISocket_TCP, Socket_WriteCB, (void*)pMe);
// 		return;
// 	}
// 	else // rv == 0 No more data
// 	{
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_WriteCB  No more data");
// 	}
// 	
// 	return;
// }
// 
// static void Socket_ReadCB(void *p)
// {
// 	newgimis *pMe = (newgimis*)p;
// 
// 	int nRead;
// 	char szBuf[50];
// 
// 	TCP_HEADER *t_tcphead =  (TCP_HEADER *)MALLOC(sizeof(TCP_HEADER ));
// 	MEMSET(t_tcphead, 0, sizeof(TCP_HEADER));
// 
// 	nRead = ISOCKET_Read(pMe->pISocket_TCP, (byte*)&pMe->m_pszMsg, 1023);
// 
// 	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB Read = %d",nRead);
// 
// 	if (nRead == AEE_NET_WOULDBLOCK)
// 	{
// 		ISOCKET_Readable(pMe->pISocket_TCP, Socket_ReadCB, (void*)pMe);
// 		return;
// 	}
// 	else if (nRead == AEE_NET_ERROR) 
// 	{
// 	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ReadCB Failed: Error %d\n", ISOCKET_GetLastError(pMe->pISocket_TCP));
//       Socket_Clean_TCP(pMe);
// 	  return;
// 	}
// 	else if (nRead > 0) // The actual data that was read
// 	{
// 		//ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd)," Socket_ReadCB pMe->m_pszMsg = %s",pMe->m_pszMsg);
// 		SOCKET_INFO *s_info = NULL;
// 		s_info = (SOCKET_INFO*)MALLOC(sizeof(SOCKET_INFO));
// 		MEMSET(s_info,0,sizeof(SOCKET_INFO));
// 		s_info = (SOCKET_INFO*)(pMe->m_pszMsg);
// 
// 		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"s_info->iCount  = %d",s_info->iCount);
// 		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"s_info->ctitle  = %s",s_info->ctitle);
// 		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"s_info->cCRC  = %s",s_info->ctime);
// 		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"s_info->cCRC  = %s",s_info->cCRC);
// 
// 		MEMSET(pMe->m_senMsg, 0, STRLEN(pMe->m_senMsg) );
// 		MEMCPY(pMe->m_senMsg,s_info->ctitle,sizeof(s_info->ctitle));//�յ���Ϣ�����m_senMsg��udp����ʹ�á�
// 
// 		Scoket_init_UDP(pMe);
// 
// 		FREE(s_info);
// 		MEMSET(pMe->m_pszMsg,0,STRLEN(pMe->m_pszMsg));//��ջ���
// 
// 		ISOCKET_Readable(pMe->pISocket_TCP, Socket_ReadCB, (void*)pMe);
// 		return;
// 	}
// 	else // rv == 0 No more data
// 	{
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Socket_ReadCB  No more data");
// 	}
// 
// 	return;
// }
// 
// void Socket_Clean_TCP(newgimis *pMe)
// {	
// 	if (pMe->pISocket_TCP) 
// 	{
// 		ISOCKET_Release(pMe->pISocket_TCP);
// 		pMe->pISocket_TCP = 0;
// 	}
// 	
// 	if (pMe->pINetMgr_TCP) 
// 	{
// 		INETMGR_Release(pMe->pINetMgr_TCP);
// 		pMe->pINetMgr_TCP = 0;
//    }
// 	return;
// }