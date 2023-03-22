/* ===========================================================================
* @file NetraDrvMsg.c
*
* @path $(IPNCPATH)/util/
*
* @desc Message driver for Netra_interface
* .
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include <NetraDrvMsg.h>
#include "sem_util.h"

static int gProcId = MSG_TYPE_MSG1;
static int qid;
static SemHandl_t hndlNetraDrvSem = NULL;

#define PROC_MSG_ID	gProcId

#define ENABLE_CMEM	(0)

/**
 * @brief set the message type id of the process
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 *
 *
 */
void NetraDrv_SetProcId(int proc_id)
{
	if(proc_id < MSG_TYPE_MSG1 || proc_id >= MSG_TYPE_END){
		gProcId = MSG_TYPE_MSG1;
		return;
	} else
		gProcId = proc_id;
}

/**
 * @brief set the message queue id of the process
 *
 *
 * @param   iqid    message queue id of the proceess
 *
 *
 *
 */
void NetraDrv_SaveQid(int iqid)
{
	qid = iqid;
}

int Testflag = 0;
/**
 * @brief Initiliztion of the message driver for Netra interface
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int NetraDrvInit(int proc_id)
{
	if(proc_id < MSG_TYPE_MSG1 || proc_id > 22){
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	gProcId = proc_id;
	fprintf(stderr, "%s: %d\n", __func__, proc_id);
#if ENABLE_CMEM
	/* CMEM only one init is allowed in each process */
	if(Testflag==0)
	{
		if(CMEM_init() < 0){
			gProcId = MSG_TYPE_MSG1;
			return -1;
		}
	}
#endif
	Testflag = 1;;
	if(hndlNetraDrvSem == NULL)
		hndlNetraDrvSem = MakeSem();
	if(hndlNetraDrvSem == NULL){
#if ENABLE_CMEM
		CMEM_exit();
#endif
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	if((qid=Msg_Init(MSG_KEY)) < 0){
		DestroySem(hndlNetraDrvSem);
		hndlNetraDrvSem = NULL;
#if ENABLE_CMEM
		CMEM_exit();
#endif
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	return 0;
}
/**
 * @brief Resource releasing of the message driver for Netra interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int NetraDrvExit()
{
	fprintf(stderr, "%s: %d\n", __func__, gProcId);
	gProcId = MSG_TYPE_MSG1;
	DestroySem(hndlNetraDrvSem);
	hndlNetraDrvSem = NULL;
#if ENABLE_CMEM

	if( Testflag != 0 )
	{
	 	CMEM_exit();
	}
	Testflag = 0;;
	return 0;
#else
	return 0;
#endif
}

/**
 * @brief  Message driver for getting current frame information
 *
 *
* @param   fmt    stucture of frame information, defined at Msg_def.h
 *
 *
 *
 */
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_CUR_FRAME;
	msgbuf.frame_info.format = fmt;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -1;
	return msgbuf.frame_info;
}

/**
 * @brief  Message driver for waiting newframe (Do not use now)
 *
 *
* @param   fmt    stucture pointer of frame information, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
FrameInfo_t WaitNewFrame(FrameFormat_t fmt)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_NEW_FRAME;
	msgbuf.frame_info.format = fmt;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -1;
	return msgbuf.frame_info;
}
/**
 * @brief  Message driver for getting memory information
 *
 *
 * @return MSG_MEM
 *
 *
 */
MSG_MEM GetPhyMem()
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MEM;
	SemWait(hndlNetraDrvSem);
//	printf("========GetPhyMem()_snd qid[%d] size[%d]\n",qid, msgbuf.mem_info.size);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
//	printf("========GetPhyMem()_rcv qid[%d] size[%d]\n",qid, msgbuf.mem_info.size);
	
	if(msgbuf.ret != 0)
		msgbuf.mem_info.addr = 0;
	return msgbuf.mem_info;
}

/**
 * @brief  Message driver for sending leave message (Do not use now)
 *
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void SendQuitCmd()
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_QUIT;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	SemRelease(hndlNetraDrvSem);

}
/**
 * @brief  Message driver for unLock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void UnlockFrame(FrameInfo_t *pFrame)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_UNLOCK_FRAME;
	msgbuf.frame_info.format = pFrame->format;
	msgbuf.frame_info.serial_no = pFrame->serial_no;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	SemRelease(hndlNetraDrvSem);
	return ;
}
/**
 * @brief  Message driver for Lock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockFrame(FrameInfo_t *pFrame)
{
	int cnt = 0;
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_LOCK_FRAME;
	msgbuf.frame_info.format = pFrame->format;
	msgbuf.frame_info.serial_no = pFrame->serial_no;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/

	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);

	SemRelease(hndlNetraDrvSem);
	pFrame->size = msgbuf.frame_info.size;
	pFrame->width = msgbuf.frame_info.width;
	pFrame->height = msgbuf.frame_info.height;
	pFrame->offset = msgbuf.frame_info.offset;
	pFrame->quality = msgbuf.frame_info.quality;
	pFrame->flags = msgbuf.frame_info.flags;
	pFrame->frameType = msgbuf.frame_info.frameType;
	pFrame->timestamp = msgbuf.frame_info.timestamp;
	for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
	{
		pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
	}

	return msgbuf.ret;
}
/**
 * @brief  Message driver for getting MPEG4 VOL data
 *
 *
 * @param   pVolBuf   ouput buffer for getting MPEG4 VOL data
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int GetVolInfo(FrameInfo_t *pFrame, int fmt_type)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VOL;
	msgbuf.frame_info.format = fmt_type;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);

	pFrame->offset 	= msgbuf.frame_info.offset;
	pFrame->size 	= msgbuf.frame_info.size;

	return msgbuf.frame_info.size;
}
/**
 * @brief  Message driver for Lock MPEG4 I-frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type)
{
	int cnt = 0;
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_AND_LOCK_IFRAME;
	msgbuf.frame_info.format = fmt_type;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	if(msgbuf.ret == -1)
		msgbuf.frame_info.serial_no = -1;
	else if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -2;
	pFrame->serial_no = msgbuf.frame_info.serial_no;
	pFrame->size = msgbuf.frame_info.size;
	pFrame->width = msgbuf.frame_info.width;
	pFrame->height = msgbuf.frame_info.height;
	pFrame->offset = msgbuf.frame_info.offset;
	pFrame->quality = msgbuf.frame_info.quality;
	pFrame->flags = msgbuf.frame_info.flags;
	pFrame->frameType = msgbuf.frame_info.frameType;
	pFrame->timestamp = msgbuf.frame_info.timestamp;
	for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
	{
		pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
	}
	return msgbuf.frame_info.serial_no;
}

/**
 * @brief  day / night mode setting for UI
 *
 *
 * @param   nDayNight    0 : night mode 	1: day mode
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetDayNight(unsigned char nDayNight)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DAY_NIGHT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nDayNight;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Motion Enable mode setting for UI
 *
 *
 * @param   enableVal  0: Enable		1:Disable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int NetraSetMotion(NetraMotionPrm* pMotionPrm)
{
	MSG_BUF msgbuf;
	void* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOTION;
	ptr = (void*)&msgbuf.mem_info;
	memcpy(ptr, pMotionPrm, sizeof(NetraMotionPrm));
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  indoor / outdoor mode setting for UI
 *
 *
 * @param   nValue    0 : indoor	1: outdoor	2: Auto
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetWhiteBalance(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, MSG_NOERROR);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  back light setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBacklight(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BACKLIGHT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  brightness setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBrightness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BRIGHTNESS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  contrast setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetContrast(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CONTRAST;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Saturation setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSaturation(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SATURATION;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  Sharpness setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSharpness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHARPNESS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Bitrate setting for UI
 *
 *
 * @param   stream    	0: stream1 720P \n
*				1: stream2 SIF \n
*
 * @param   nValue    	64~8000
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetEncBitrate(unsigned char stream, int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_BITRATE1;
	else
		msgbuf.cmd = MSG_CMD_SET_BITRATE2;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  frame rate setting for UI
 *
 *
 * @param   stream    	0: stream1 MPEG4 720P \n
*				1: stream2 MPEG4 SIF \n
*				2: stream3 JPG VGA
*
 * @param   nValue    	7500~30000 ( 7.5fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetEncFramerate(unsigned char stream, int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_M41_FRAMERATE;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_M42_FRAMERATE;
	else
		msgbuf.cmd = MSG_CMD_SET_JPG_FRAMERATE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  frame rate setting for UI
 *
 *
 * @param   nValue    	7500~30000 ( 7.5fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetJpgQuality(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_JPEG_QUALITY;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetRot(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ROTATION;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetMirr(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MIRROR;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetOSDWindow(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSDWINDOW;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  2A enable/disable setting for UI
 *
 *
 * @param   nValue    	0: disable \n
 *				1: enable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetImage2AType(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_TYPE_2A;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  TV setting for UI
 *
 *
 * @param   nValue    	0: NTSC \n
 *				1: PAL
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTvSystem(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TV_SYSTEM;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  PTZ  setting  for UI
 *
 *
 * @param   nValue    		0: PTZ_ZOOM_IN \n
*					1: PTZ_ZOOM_OUT \n
*					2: PTZ_PAN_UP \n
*					3: PTZ_PAN_DOWN \n
*					4: PTZ_PAN_LEFT \n
*					5: PTZ_PAN_RIGHT \n
*					6: PTZ_INIT_4X\n
*					7: PTZ_ZOOM_EMPTY \n
*					8: PTZ_ZOOM_RESET
 *
 *@note This function only work at sensor output is VGA
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetPtz(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_PTZ;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  BinningSkip  setting  for UI
 *
 *
 * @param   nValue    		0: Binning \n
*					1: Skipping \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBinningSkip(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BINNING_SKIP;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  time stamp  setting  for UI
 *
 *
 * @param   nValue    		0: timestamp open \n
*					1: timestamp close \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTStamp(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TSTAMP;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}


unsigned int EnMaskGen(unsigned char value)
{
	unsigned int tMask = 0;
	fprintf(stderr, "\nFeatureMask: ");
	if(value == 0) {
		fprintf(stderr, "NONE.\n");
		return 0; }
	if ((value & FFLAG_VS) > 0) {
		fprintf(stderr, "VS.");
		tMask |= (1<<ADV_FEATURE_VS); }
	if ((value & FFLAG_LDC) > 0) {
		fprintf(stderr, "LDC.");
		tMask |= (1<<ADV_FEATURE_LDC); }
	if ((value & FFLAG_SNF) > 0) {
		fprintf(stderr, "SNF.");
		tMask |= (1<<ADV_FEATURE_SNF); }
	if ((value & FFLAG_TNF) > 0) {
		fprintf(stderr, "TNF.");
		tMask |= (1<<ADV_FEATURE_TNF); }
	fprintf(stderr, "\n");
	return tMask;
}

int SetStreamAdvFeature(unsigned char stream, unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	unsigned int EnMask = EnMaskGen(nValue);

	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_M41_ADV_FEATURE;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_M42_ADV_FEATURE;
	else
		msgbuf.cmd = MSG_CMD_SET_JPG_ADV_FEATURE;

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = EnMask;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetROIConfig(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ROICFG;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetAvFaceDetect(unsigned char nValue)
{
	MSG_BUF msgbuf;
	FACE_PARM* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_FDETECT;

	ptr = (FACE_PARM*)&msgbuf.mem_info;
	switch (nValue)
	{
		case 0:
		{
			ptr->type = FACE_NO_DETECT;
			break;
		}
		case 1:
		{
			ptr->type = FACE_DETECT;
			break;
		}
		case 2:
		{
			ptr->type = FACE_MASK;
			break;
		}
		case 3:
		{
			ptr->type = FACE_RECOGIZE;
			break;
		}
		case 4:
		{
			ptr->type = FACE_REGUSER;  //ANR
			break;
		}
		case 5:
		{
			ptr->type = FACE_DELUSER;  //ANR
			break;
		}
	}
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetAvOsdTextEnable(int enable)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDTEXT_EN;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = enable;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetHistEnable(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_HIST_EN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int EnableGBCE(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_GBCE_EN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetEvalVersionMsg(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EVALMSG;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetAvOsdLogoEnable(int enable)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDLOGO_EN;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = enable;
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

int SetAvOsdText(char* strText, int nLength)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDTEXT;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = nLength;
	memcpy(++ptr, strText, nLength);
	SemWait(hndlNetraDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlNetraDrvSem);
	return msgbuf.ret;
}

