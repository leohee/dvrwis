/* ===========================================================================
* @file NetraDrvMsg.h
*
* @path $(IPNCPATH)/include/
*
* @desc Message driver for Netra_interface
* .
*
* =========================================================================== */

#ifndef _NETRADRVMSG_
#define _NETRADRVMSG_

#if defined (__cplusplus)
extern "C" {
#endif

/**
  \file NetraDrvMsg.h

  \brief Message driver for Netra_interface
*/

#define FFLAG_VS	0x01
#define FFLAG_LDC	0x02
#define FFLAG_SNF	0x04
#define FFLAG_TNF	0x08


#include <Msg_Def.h>
typedef struct _NetraMotionPrm{
	int bMotionEnable;
	int bMotionCEnale;
	int MotionLevel;
	int MotionCValue;
	int MotionBlock;
}NetraMotionPrm;

/**
  \defgroup AV_MSG_DRV Netra interface message driver datatypes, data structures, functions

  The section defines some common datatypes, data structures, callbacks which are used by all parts of the Netra interface framework.
  System init APIs are also defined in this section.

  \section AV_MSG_DRV_COMMON_DIR_STRUCT Directory Structure

  \subsection AV_MSG_DRV_COMMON_INTERFACE Interface

  \code
  NetraDrvMsg.h
  \endcode

  \subsection AV_MSG_DRV_COMMON_SOURCE Source

  \code
  NetraDrvMsg.c
  \endcode


  @{
*/
/* This function should be called at process innitial !!! */
void NetraDrv_SetProcId(int proc_id);
void NetraDrv_SaveQid(int iqid);
/* Only one NetraDrvInit is allowed in each process */
int NetraDrvInit(int proc_id);
int NetraDrvExit();
/* API */
MSG_MEM GetPhyMem();
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt);
FrameInfo_t WaitNewFrame(FrameFormat_t fmt);
void SendQuitCmd();
int LockFrame(FrameInfo_t *pFrame);
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type);
void UnlockFrame(FrameInfo_t *pFrame);
int GetVolInfo(FrameInfo_t *pFrame,int fmt_type);
int SetDayNight(unsigned char nDayNight);
int SetWhiteBalance(unsigned char nValue);
int SetBacklight(unsigned char nValue);
int SetBrightness(unsigned char nValue);
int SetContrast(unsigned char nValue);
int SetSaturation(unsigned char nValue);
int SetSharpness(unsigned char nValue);
int SetEncBitrate(unsigned char stream, int nValue);
int SetEncFramerate(unsigned char stream, int nValue);
int SetJpgQuality(int nValue);
int SetRot(int nValue);
int SetMirr(int nValue);
int SetOSDWindow(int nValue);
int SetImage2AType(unsigned char nValue);
int SetTvSystem(unsigned char nValue);
int SetBinningSkip(unsigned char nValue);
int SetTStamp(unsigned char nValue);
int SetPtz(int nValue);
int NetraSetMotion(NetraMotionPrm* pMotionPrm);
int SetStreamAdvFeature(unsigned char stream, unsigned char nValue);
int SetROIConfig(unsigned char nValue);
int SetAvFaceDetect(unsigned char nValue);
int SetAvOsdTextEnable(int enable);
int SetHistEnable(int enable);
int EnableGBCE(int enable);
int SetEvalVersionMsg(int enable);
int SetAvOsdLogoEnable(int enable);
int SetAvOsdText(char* strText, int nLength);
/* @} */

#if defined (__cplusplus)
}
#endif

#endif

