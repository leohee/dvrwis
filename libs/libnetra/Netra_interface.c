/* ===========================================================================
* @file Netra_interface.c
*
* @path $(IPNCPATH)/util/
*
* @desc Interface for getting Audio/Video data between processes
* .
* =========================================================================== */

#include <NetraDrvMsg.h>
#include "Netra_interface.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>

void   *virptr = NULL;
static unsigned char VolInfo[128];
static unsigned char VolInfo_cif[128];
static int gMemSize = 0;
static int shmem_id;

/**
 * @brief Interface for getting Audio/Video data between processes
 *
 *
 * @param   field    The operation field, the value should be AV_OP_XXX_XXX defined at Netra_interface.h
 * @param   serial   frame serial number , used when locking frame data
 * @param   ptr    frame information data structure defined at Netra_interface.h
 *
 * @return the value should be RET_XXXXX defined at Netra_interface.h
 *
 * @pre Must have called NetraDrvInit() and func_get_mem()
 *
 */
int GetAVData( unsigned int channel, unsigned int field, int serial, AV_DATA * ptr )
{
	int ret=RET_SUCCESS;
	if(virptr == NULL)
	{
		printf("enter GetAVData() channel[%d] field[%d] virptr is NULL!!!!!!!!\n", channel, field);
		return RET_ERROR_OP;
	}
		
//	printf("enter GetAVData() channel[%d] field[%d]\n", channel, field);
	switch(field){
/*		
		case AV_OP_GET_MJPEG_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_MJPEG);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->timestamp = curframe.timestamp;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
		case AV_OP_WAIT_NEW_MJPEG_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_MJPEG);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
				}
			}
			break;
		case AV_OP_LOCK_MJPEG:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_MJPEG;
					ret = LockFrame(&frame)
					switch(ret){
						case -98:
							ret = RET_OVERWRTE;
							break;
						case -99:
							ret = RET_NO_MEM;
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->timestamp = frame.timestamp;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							break;
					}
				}
			}
			break;
		case AV_OP_UNLOCK_MJPEG:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_MJPEG;
					UnlockFrame(&frame);
				}
			}
			break;
*/			
		case AV_OP_GET_MPEG4_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(channel);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial 	= curframe.serial_no;
					ptr->size 		= curframe.size;
					ptr->width 		= curframe.width;
					ptr->height 	= curframe.height;
					ptr->flags 		= curframe.flags;
					ptr->timestamp 	= curframe.timestamp;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
/*			
		case AV_OP_GET_MPEG4_CIF_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_MPEG4_EXT);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->timestamp = curframe.timestamp;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
*/			
		case AV_OP_WAIT_NEW_MPEG4_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(channel);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size 	= curframe.size;
					ptr->width 	= curframe.width;
					ptr->height = curframe.height;
					ptr->flags 	= curframe.flags;
				}
			}
			break;
/*
		case AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_MPEG4);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
				}
			}
			break;
*/

		case AV_OP_LOCK_MP4:
			if(serial == -1)
			{
				ret = RET_INVALID_PRM;
			} 
			else 
			{
				FrameInfo_t frame;
				if(serial < 0)
				{
					ret = RET_INVALID_PRM;
				} 
				else 
				{
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = channel;//FMT_MPEG4;
					ret = LockFrame(&frame);
					switch(ret)
					{
						case -99:
							ret = RET_OVERWRTE;
							printf("RET_OVERWRTE\n");
							break;
						case -100:
							ret = RET_NO_MEM;
							printf("RET_NO_MEM\n");
							break;
						case -101:
							ret = RET_ERROR_OP;
							break;
						case -98:
							ret = RET_NO_VALID_DATA;
							//printf("RET_NO_VALID_DATA\n");
							break;
						case 0:
							ptr->serial 	= serial;
							ptr->size 		= frame.size;
							ptr->width 		= frame.width;
							ptr->height 	= frame.height;
							ptr->quality 	= frame.quality;
							ptr->flags 		= frame.flags;
							ptr->frameType 	= frame.frameType;
							ptr->timestamp 	= frame.timestamp;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							printf("RET_UNKNOWN_ERROR\n");
							break;
					}
				}
			}
			break;
/*			
			case AV_OP_LOCK_MP4_CIF:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_MPEG4_EXT;
					ret = LockFrame(&frame);
					switch(ret){
						case -99:
							ret = RET_OVERWRTE;
							printf("RET_OVERWRTE\n");
							break;
						case -100:
							ret = RET_NO_MEM;
							printf("RET_NO_MEM\n");
							break;
						case -101:
							ret = RET_ERROR_OP;
							break;
						case -98:
							ret = RET_NO_VALID_DATA;
							//printf("RET_NO_VALID_DATA\n");
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->frameType = frame.frameType;
							ptr->timestamp = frame.timestamp;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							printf("RET_UNKNOWN_ERROR\n");
							break;
					}
				}
			}
			break;
*/
		case AV_OP_UNLOCK_MP4:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format 	= channel;//FMT_MPEG4;
					UnlockFrame(&frame);
				}
			}
			break;
/*
		case AV_OP_UNLOCK_MP4_CIF:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_MPEG4_EXT;
					UnlockFrame(&frame);
				}
			}
			break;
*/			
		case AV_OP_LOCK_MP4_VOL:
		{
			FrameInfo_t frame;
			ret = GetVolInfo(&frame,channel);
			if( ret > 0 )
			{
				memcpy( VolInfo,((unsigned char *)virptr + frame.offset),ret);
			}
			ptr->ptr 	= VolInfo;
			ptr->size 	= ret;
			ret 		= RET_SUCCESS;
			break;
		}
/*		
		case AV_OP_LOCK_MP4_CIF_VOL:
		{
			FrameInfo_t frame;
			ret = GetVolInfo(&frame,FMT_MPEG4_EXT);
			if( ret > 0 )
			{
				memcpy( VolInfo_cif,((unsigned char *)virptr + frame.offset),ret);
			}
			ptr->ptr = VolInfo_cif;
			ptr->size = ret;
			ret = RET_SUCCESS;
			break;
		}
*/		
		case AV_OP_UNLOCK_MP4_VOL:
			ret = RET_SUCCESS;
			break;
/*
		case AV_OP_UNLOCK_MP4_CIF_VOL:
			ret = RET_SUCCESS;
			break;
*/
		case AV_OP_LOCK_MP4_IFRAME:
		{
			FrameInfo_t curframe;
			LockMpeg4IFrame(&curframe, channel);
			if(curframe.serial_no == -1){
				ret = RET_NO_VALID_DATA;
			} else if(curframe.serial_no == -2){
				ret = RET_NO_MEM;
			} else {
				int cnt = 0;
				ptr->serial 	= curframe.serial_no;
				ptr->size 		= curframe.size;
				ptr->width 		= curframe.width;
				ptr->height 	= curframe.height;
				ptr->quality 	= curframe.quality;
				ptr->flags 		= curframe.flags;
				ptr->frameType 	= curframe.frameType;
				ptr->timestamp 	= curframe.timestamp;
				for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
				{
					ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
				}
				ptr->ptr = (unsigned char *)virptr + curframe.offset;
				ret = RET_SUCCESS;
			}
			break;
		}
/*
		case AV_OP_LOCK_MP4_CIF_IFRAME:
		{
			FrameInfo_t curframe;
			LockMpeg4IFrame(&curframe,FMT_MPEG4_EXT);
			if(curframe.serial_no == -1){
				ret = RET_NO_VALID_DATA;
			} else if(curframe.serial_no == -2){
				ret = RET_NO_MEM;
			} else {
				int cnt = 0;
				ptr->serial = curframe.serial_no;
				ptr->size = curframe.size;
				ptr->width = curframe.width;
				ptr->height = curframe.height;
				ptr->quality = curframe.quality;
				ptr->flags = curframe.flags;
				ptr->frameType = curframe.frameType;
				ptr->timestamp = curframe.timestamp;
				for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
				{
					ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
				}
				ptr->ptr = (unsigned char *)virptr + curframe.offset;
				ret = RET_SUCCESS;
			}
			break;
		}
*/		
		case AV_OP_LOCK_ULAW:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
//					frame.format = FMT_AUDIO;
					frame.format = FMT_AUDIO_CH1 + channel ;
					ret = LockFrame(&frame);
					switch(ret){
						case -97:
						case -98:
							ret = RET_NO_VALID_DATA;
							break;
						case -99:
							ret = RET_NO_MEM;
							break;
						case 0:
							ptr->serial 	= serial;
							ptr->size 		= frame.size;
							ptr->width 		= frame.width;
							ptr->height 	= frame.height;
							ptr->quality 	= frame.quality;
							ptr->flags 		= frame.flags;
							ptr->timestamp 	= frame.timestamp;
//							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							for (cnt = FMT_AUDIO_CH1; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							break;
					}
				}
			}
			break;
		case AV_OP_UNLOCK_ULAW:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
//					frame.format 	= FMT_AUDIO;
					frame.format 	= FMT_AUDIO_CH1 + channel;
					UnlockFrame(&frame);
				}
			}
			break;
		case AV_OP_GET_ULAW_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
//				FrameInfo_t curframe = GetCurrentFrame(channel);
				FrameInfo_t curframe = GetCurrentFrame(FMT_AUDIO_CH1 + channel);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial 	= curframe.serial_no;
					ptr->size 		= curframe.size;
					ptr->width 		= curframe.width;
					ptr->height 	= curframe.height;
					ptr->timestamp 	= curframe.timestamp;
//					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					for (cnt = FMT_AUDIO_CH1; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;

		case AV_OP_WAIT_NEW_ULAW_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
//				FrameInfo_t curframe = WaitNewFrame(FMT_AUDIO);
				FrameInfo_t curframe = WaitNewFrame(FMT_AUDIO_CH1 + channel);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size 	= curframe.size;
					ptr->width 	= curframe.width;
					ptr->height = curframe.height;
				}
			}
			break;
		default:
			ret = RET_INVALID_COMMAND;
			dbg("Command field = %d\n", field);
			break;
	}
	return ret;
}


extern int cmem_fd;

#if 0
void *CMEM_MMAP( unsigned long physp,  int size )
{
	void *userp;

	if (cmem_fd == -1) {
        printf("allocPool: You must initialize CMEM before making API calls.\n");
        return NULL;
    }
	/* Map the physical address to user space */
    userp = mmap(0,                       // Preferred start address
                 size,                    // Length to be mapped
                 PROT_WRITE | PROT_READ,  // Read and write access
                 MAP_SHARED,              // Shared memory
                 cmem_fd,                 // File descriptor
                 physp);                  // The byte offset from fd

    if (userp == MAP_FAILED) {
        printf("allocPool: Failed to mmap buffer at physical address %#lx\n",
            physp);
        ioctl(cmem_fd, CMEM_IOCFREE, &physp);
        return NULL;
    }

	return userp;
}
#endif

/**
 * @brief Setup the memory for getting audio / video data
 *
 *
 * @param   pdata    could be NULL, not use now
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int func_get_mem( void *pdata )
{
	MSG_MEM mem_info = GetPhyMem();
	
//	if(mem_info.addr == 0){
	if(mem_info.size == 0){
		fprintf(stderr, "Error: Memory return Invalid\n");
		return -1;
	}
	if(gMemSize == 0 && virptr == NULL)
	{
//		virptr = CMEM_MMAP( mem_info.addr,  (int)mem_info.size);		
		shmem_id = shmget((key_t)0x4321, mem_info.size, 0666|IPC_CREAT);
		if(shmem_id == -1)
		{
			printf("NETRA_INTERFACE: shmem_id is invalid!!!! key[%x] size[%d]\n");
			return 0;
		}
		
		virptr = shmat(shmem_id, 0, 0);
		if(virptr == NULL)
		{
			printf("Shared memory init fail!!!!\n");
			shmctl(shmem_id, IPC_RMID, 0);
			return 0;
		}
	}
	else{
		fprintf(stderr, "Warning: Memory map have been done before\n");
		return 0;
	}
	if(virptr == NULL)
		return -1;
	gMemSize = mem_info.size;
	return 0;
}
/**
 * @brief Release the resource of the appro interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int NetraInterfaceExit()
{
	if(gMemSize && shmem_id >= 0)
		shmctl(shmem_id, IPC_RMID, 0);
//  	munmap(virptr, gMemSize);
	else
		fprintf(stderr, "Error: Maping size is zero!!\n");
	virptr = NULL;
	gMemSize = 0;
//	return NetraDrvExit();
}

