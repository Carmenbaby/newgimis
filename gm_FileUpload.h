#ifndef GM_FILEUPLOAD_H
#define GM_FILEUPLOAD_H

#define FLDUPD_EVT_START     EVT_USER + 1050		// 启动文件上传
#define FLDUPD_EVT_STOP		 EVT_USER + 1051		// 退出文件上传
#define FLDUPD_EVT_UPLOAD	 EVT_USER + 1052		// 开始上传文件
#define FLDUPD_EVT_SEND	     EVT_USER + 1053		// 正在上传文件

#define MAX_FILE_BYTES_POST		512000			//  1024*500 = 500K  要发送的文件，一次最多发送的字节数


typedef struct _FileUpload FileUpload;
typedef struct _newgimis newgimis;

// 上传录音文件字段列表   2014-04-10
typedef struct  
 {
 	char	sRecName[MAX_FILE_NAME+1];					// 录音文件名(含路径)
 	char    cSendStatus;								// 发送状态 0 : 未发送, 1 打开发送文件 ，10 已发送
} IAudRecFieldA;
 
// 上传录音文件记录列表
typedef struct  
{
 	uint16 			nRecordCount;								// 记录的总数
 	uint16 			nSelRec;									// 选中的任务号, nSelRec >= nRecordCount 未选中
	char            cStatus;									// 0: 没有收到新的上传指令, 1: 收到新的上传指令
	char            sDate1[11];									// 开始时间  yymmddhhmm,年月日时分
	char            sDate2[11];									// 结束时间  yymmddhhmm,年月日时分
 	IAudRecFieldA	*pAudRecFieldA;								// 一条记录
} IAudRecRecordsA;
// Add End 2014-04-10

struct _FileUpload					
{
	AEEApplet    a;
	newgimis		*m_pParent;

	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   piWResp;				// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	ISourceUtil* pisu;
	uint32		 dwPostOvertime;		// 记录发送文件后接收应答的时间，单位妙，接收时超过规定时间，关闭链接，并提示失败
	
	char		 pURL[256];
	int          nBytes;					// count of bytes in response
	char         sBodyBuf[10];				// 只要应答的第0个字符	
	char         cSendSts;					// 发送的状态
											//	'1':  本段接收成功，如果不是最后一段，则手机端会继续发送下一段；如果是最后一段并且发送的是视频文件，则手机端会删除视频文件，提示手机用户“发送成功”。
											//	'0':  本段接收失败，则手机端会重发本段
											//	'A': 表示接收出现异常，手机端显示“发送失败”信息给手机用户。如果发送的是视频文件，则不会删除视频文件
	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	int32		 iFileRead;					// 读入文件的字节数  
	uint8		 nPartNum;					// 正在传第几段，1 第1段，2 第2段，。。。。。  
	uint8		 nPartsTotal;				// 总段数

	IFileMgr*    pFileMgr;
	IFile*		 pFileUpload;				// 待上传的文件句柄
    FileInfo	 oFileInfo;					// 文件信息
	int			 nAddHours;					// 
	uint32       nHasRead;					// 已经上传读取的字节数

	AECHAR		pText[TEXT_MAXSIZE+1];		// 与上传文件有关的内容,最多100个双字节字符

	char		 cType;						// 文件的类型 '0' 已经初始化  '1' 视频文件， '2' 配置文件 config.txt, '3' 日志 rec_info.log
											//  '4' 照片  
											//  '5' 录音文件  Add by ----  2014-04-10
	char         sFile[MAX_FILE_NAME+1];	// 文件名（含路径）	
	uint8		 nState;					// 状态
											// 0(SUCCESS)    成功
											// 1(EFAILED)    失败 

	IAudRecRecordsA *pAudRecRecordsA;		// 上传录音文件记录				// Add by ---- 2014-04-10

};

boolean FleUpd_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// 发送照片到中心端
void uf_SendFile(newgimis * pMe);



#endif /* #ifndef GM_FILEUPLOAD_H */


// // 上传文件  49 视频， 50 配置文件， 51 日志
// if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,49,dwParam) )	
// {
// 	//						uf_RecLog(pMe,"PostEvent FLDUPD_EVT_UPLOAD");
// 	ISHELL_PostEvent(pMe->theApp.m_pIShell,pMe->theApp.clsID,FLDUPD_EVT_UPLOAD,0,0);
// }
