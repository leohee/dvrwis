/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
*
* =========================================================================== */
/**
* @file Sys_Msg_Def.h
* @brief System message ID definition
* @warning Be sure that one message ID can only be used by one process.
* @see File_Msg_Def.h
*/
#ifndef _SYS_MSG_DEF_H_
#define _SYS_MSG_DEF_H_
/**
* @addtogroup SYS_MSG_DRV
* @{
*/
/* MSG_TYPE_MSG1 is reserved for server */
#define SYS_BOA_MSG		MSG_TYPE_MSG2 ///< message ID used in BOA process to communicate with system server. 
#define SYS_UPNP_MSG		MSG_TYPE_MSG3 ///< message ID used in UPNP process to communicate with system server. 
#define SYS_FTP_MSG		MSG_TYPE_MSG4 ///< message ID used in FTP client to communicate with system server. 
#define SYS_SMTP_MSG		MSG_TYPE_MSG5 ///< message ID used in SMTP client to communicate with system server. 
/**
* @}
*/
#endif
