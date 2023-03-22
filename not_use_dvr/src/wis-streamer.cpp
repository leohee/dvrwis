/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// An application that streams audio/video captured by a WIS GO7007,
// using a built-in RTSP server.
// main program

#include <signal.h>
#include <BasicUsageEnvironment.hh>
#include <getopt.h>
#include <liveMedia.hh>
#include "Err.hh"
#include "WISJPEGVideoServerMediaSubsession.hh"
#include "WISMPEG4VideoServerMediaSubsession.hh"
#include "WISH264VideoServerMediaSubsession.hh"
#include "WISPCMAudioServerMediaSubsession.hh"
#include <NetraDrvMsg.h>
#include <Netra_interface.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <GroupsockHelper.hh>
#include "WISJPEGStreamSource.hh"


enum  StreamingMode 
{  
	STREAMING_UNICAST,  
	STREAMING_UNICAST_THROUGH_DARWIN,  
	STREAMING_MULTICAST_ASM,  
	STREAMING_MULTICAST_SSM
};


portNumBits rtspServerPortNum = 554;
char const* MjpegStreamName = "mjpeg";
char const* Mpeg4StreamName = "mpeg4";
//char const* H264StreamName = "h264_ch1";

//RTSP_EXT_CHANNEL
char const* H264StreamName[VIDEO_TYPE_MAX_NUM] = {"h264_ch1", "h264_ch2", "h264_ch3", "h264_ch4", "h264_ch5", "h264_ch6" , "h264_ch7", "h264_ch8", "h264_ch9", "h264_ch10", "h264_ch11","h264_ch12","h264_ch13","h264_ch14","h264_ch15","h264_ch16"};
char const* streamDescription = "RTSP/RTP stream from NETRA";

int MjpegVideoBitrate = 1500000;
int Mpeg4VideoBitrate = 1500000;
int H264VideoBitrate = 1500000;
int audioOutputBitrate = 128000;


unsigned audioSamplingFrequency = 16000;
unsigned audioNumChannels = 1;
int audio_enable = 1;
char watchVariable = 0;
char videoType = -1;
char IsAudioAlarm = 0;



int qid = 0;

void sigterm(int dummy)
{
	printf("caught SIGTERM: shutting down\n");
	NetraInterfaceExit();
	exit(1);
}

void sigint(int dummy)
{
	if( IsAudioAlarm == 1 )
	{
		printf("Audio Alarm!!\n");
		IsAudioAlarm = 0;
		return;
	}

	printf("caught SIGINT: shutting down\n");
	//printf("watchVariable = %d\n",watchVariable);
	//printf("videoType = %d\n",videoType);
	watchVariable = 1;
	alarm(1);
	//NetraInterfaceExit();
	//exit(1);
}

void sig_audio_enable(int dummy)
{

	printf("Audio enabled!\n");
	audio_enable = 1;
	
	//printf("videoType = %d\n",videoType);
	IsAudioAlarm = 1;
	alarm(2);
}

void sig_audio_disable(int dummy)
{
	
	printf("Audio disabled!\n");
	audio_enable = 0;
	
	//printf("videoType = %d\n",videoType);
	IsAudioAlarm = 1;
	alarm(2);
}

void init_signals(void)
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sigaddset(&sa.sa_mask, SIGALRM);

	sa.sa_flags = 0;
	sa.sa_handler = sigterm;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sig_audio_enable;
	sigaction(SIGUSR1, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sig_audio_disable;
	sigaction(SIGUSR2, &sa, NULL);

	sa.sa_flags = 0;
	sa.sa_handler = sigint;
	sigaction( SIGALRM, &sa, NULL );
}

void share_memory_init(int msg_type)
{
  if(NetraDrvInit(msg_type))
    exit(1);
  if (func_get_mem(&qid)) {
  	NetraInterfaceExit();
	NetraDrvExit();
    exit(1);
  }
}
#include <fcntl.h>
int GetSampleRate( void )
{
	static int CurrentStatus = -255;
	int fd_proc = -1;
	int ret = -1;
	char StrBuffer[300];
	char TmpBuf[50];
	char *pStr = NULL;
	int samplerate = 0;
	char delima_buf[] = ":\r\n ";

	if( CurrentStatus >= -1 )
	{
		fprintf(stderr,"CurrentStatus is = %d \n", CurrentStatus);
		return CurrentStatus;
	}
	

	fd_proc = open("/proc/asound/card0/pcm0c/sub0/hw_params", O_RDONLY);
	if( fd_proc < 0 )
	{
		fprintf(stderr,"GetSampleRate open file fail \n");
		ret = -1;
		goto CHECK_CPU_END;
		
	}

	ret = read(fd_proc,  StrBuffer, sizeof(StrBuffer)-1);
	if( ret <= 0 )
	{
		fprintf(stderr,"read device error !!");
		ret = -1;
		goto CHECK_CPU_END;

	}

	ret = -1;
	StrBuffer[sizeof(StrBuffer)-1] = '\0';
	pStr = strtok(StrBuffer,delima_buf );
	
	while( pStr != NULL )
	{
		sscanf( pStr,"%s",TmpBuf);
		
		if( strncmp(TmpBuf, "rate", sizeof("rate")) == 0 )
		{
			
			pStr = strtok(NULL, delima_buf);

			sscanf( pStr,"%d",&samplerate);
			
			//fprintf(stderr,"samplerate = %d \n", samplerate);
			
			ret = samplerate;
			goto CHECK_CPU_END;
			
			
		}
		
		pStr = strtok(NULL, delima_buf);
		
	}

CHECK_CPU_END:

	if( fd_proc >= 0 )
		close(fd_proc);


	CurrentStatus = ret;
	
	return ret;

	
}

int main(int argc, char** argv) {
  init_signals();
  setpriority(PRIO_PROCESS, 0, 0);
/*
  if( GetSampleRate() == 16000 )
  {
	audioOutputBitrate = 128000;
	audioSamplingFrequency = 16000;
  }
  else
  {
	audioOutputBitrate = 64000;
	audioSamplingFrequency = 8000;
  }
*/
	audioOutputBitrate = 128000;
	audioSamplingFrequency = 16000;

printf("RTSP audio sampling Frequency = %d\n",audioSamplingFrequency) ;
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
  int msg_type, video_type;
  NETRAInput* MjpegInputDevice = NULL;
  NETRAInput* Mpeg4InputDevice = NULL;

//RTSP_EXT_CHANNEL
  NETRAInput* H264InputDevice[VIDEO_TYPE_MAX_NUM] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};//, NULL, NULL, NULL, NULL};
    
  static pid_t child[VIDEO_TYPE_MAX_NUM - 1] = {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };
  
  StreamingMode streamingMode 		= STREAMING_UNICAST;
  netAddressBits multicastAddress 	= 0;//our_inet_addr("224.1.4.6");
  portNumBits videoRTPPortNum 		= 0;
  portNumBits audioRTPPortNum 		= 0;


  int i,j;

  char live_msg_type[VIDEO_TYPE_MAX_NUM] = { LIVE_MSG_TYPE, LIVE_MSG_TYPE2, LIVE_MSG_TYPE3, LIVE_MSG_TYPE4, 
												LIVE_MSG_TYPE5, LIVE_MSG_TYPE6, LIVE_MSG_TYPE7, LIVE_MSG_TYPE8,
												LIVE_MSG_TYPE9, LIVE_MSG_TYPE10, LIVE_MSG_TYPE11, LIVE_MSG_TYPE12, 
												LIVE_MSG_TYPE13, LIVE_MSG_TYPE14, LIVE_MSG_TYPE15, LIVE_MSG_TYPE16};
  char vtype[VIDEO_TYPE_MAX_NUM] = { VIDEO_TYPE_H264_CH1, VIDEO_TYPE_H264_CH2, VIDEO_TYPE_H264_CH3, VIDEO_TYPE_H264_CH4,
										 VIDEO_TYPE_H264_CH5, VIDEO_TYPE_H264_CH6, VIDEO_TYPE_H264_CH7, VIDEO_TYPE_H264_CH8,
										 VIDEO_TYPE_H264_CH9, VIDEO_TYPE_H264_CH10, VIDEO_TYPE_H264_CH11, VIDEO_TYPE_H264_CH12,
										 VIDEO_TYPE_H264_CH13, VIDEO_TYPE_H264_CH14, VIDEO_TYPE_H264_CH15, VIDEO_TYPE_H264_CH16 };
  int rtsp_port[VIDEO_TYPE_MAX_NUM] = { 8551, 8552, 8553, 8554, 8555, 8556, 8557, 8558,
  										8559, 8560, 8561, 8562, 8563, 8564, 8565, 8566 };
  int mpeg4_vbitrate[VIDEO_TYPE_MAX_NUM] = { 500000, 500000, 500000, 500000, 500000, 500000, 500000, 500000,
  												500000, 500000, 500000, 500000, 500000, 500000, 500000, 500000 };
  int h264_vbitrate[VIDEO_TYPE_MAX_NUM] = { 500000, 500000, 500000, 500000, 500000, 500000, 500000, 500000, 
  												500000, 500000, 500000, 500000, 500000, 500000, 500000, 500000 };
  int vRTPport[VIDEO_TYPE_MAX_NUM] = { 6000, 6004, 6008, 6012, 6016, 6020, 6024, 6028,
  										6032, 6036, 6040, 6044, 6048, 6052, 6054, 6058 };
  int aRTPport[VIDEO_TYPE_MAX_NUM] = { 6002, 6006, 6010, 6014, 6018, 6022, 6026, 6030,
  										6034, 6038, 6042, 6046, 6050, 6054, 6058, 6062 };

 //////////////////////////////RTSP_EXT_CHANNEL//////////////////////
  for(i=0;i<15;i++)
  {
  	for(j=0;j<i;j++)
  	{
  		if( child[j] == 0 )
			break;
  	}
	if( j == i )
	{
		child[i] = fork();
	}
  }


  
  for(i=0; i<15; i++)
  {
	  if( child[i] == 0 )
	  {
	  	msg_type      			= live_msg_type[i];
		video_type 			= vtype[i];
		rtspServerPortNum 	= rtsp_port[i];
		Mpeg4VideoBitrate 	= mpeg4_vbitrate[i];
		H264VideoBitrate 	= h264_vbitrate[i];
		videoRTPPortNum 	= vRTPport[i];
		audioRTPPortNum 	= aRTPport[i];
		//streamingMode 		= STREAMING_MULTICAST_SSM;
		break;
	  }
  }
  if( i == 15)
  {
  	msg_type      			= live_msg_type[i];
	video_type 			= vtype[i];
	rtspServerPortNum 	= rtsp_port[i];
	Mpeg4VideoBitrate 	= mpeg4_vbitrate[i];
	H264VideoBitrate 	= h264_vbitrate[i];
	videoRTPPortNum 	= vRTPport[i];
	audioRTPPortNum 	= aRTPport[i];
	//streamingMode 		= STREAMING_MULTICAST_SSM;
  }

  if( argc > 1 )
  {
    if( strcmp( argv[1],"-m" )== 0 )
    {
        streamingMode = STREAMING_MULTICAST_SSM;
    }
    else
    {
        streamingMode = STREAMING_UNICAST;
    }
  }
  else
  {
    streamingMode = STREAMING_UNICAST;
  }	

  videoType = video_type;

  // Objects used for multicast streaming:
  static Groupsock* rtpGroupsockAudio = NULL;
  static Groupsock* rtcpGroupsockAudio = NULL;
  static Groupsock* rtpGroupsockVideo = NULL;
  static Groupsock* rtcpGroupsockVideo = NULL;
  static FramedSource* sourceAudio = NULL;
  static RTPSink* sinkAudio = NULL;
  static RTCPInstance* rtcpAudio = NULL;
  static FramedSource* sourceVideo = NULL;
  static RTPSink* sinkVideo = NULL;
  static RTCPInstance* rtcpVideo = NULL;


  share_memory_init(msg_type);

  //init_signals();

  *env << "Initializing...\n";
  
  // Initialize the WIS input device:
	H264InputDevice[video_type] = NETRAInput::createNew(*env, video_type);
	if (H264InputDevice[video_type] == NULL) {
		err(*env) << "Failed to create MJPEG input device\n";
		exit(1);
	}
/*  
  if( video_type == VIDEO_TYPE_MJPEG)
  {  
	  MjpegInputDevice = NETRAInput::createNew(*env, VIDEO_TYPE_MJPEG);
	  if (MjpegInputDevice == NULL) {
	    err(*env) << "Failed to create MJPEG input device\n";
	    exit(1);
	  }
  }

  if( video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF)
  {  
	  H264InputDevice = NETRAInput::createNew(*env, video_type);
	  if (H264InputDevice == NULL) {
	    err(*env) << "Failed to create MJPEG input device\n";
	    exit(1);
	  }
  }
  if( video_type == VIDEO_TYPE_MPEG4 || video_type == VIDEO_TYPE_MPEG4_CIF )
  {
	  Mpeg4InputDevice = NETRAInput::createNew(*env, video_type);
	  if (Mpeg4InputDevice == NULL) {
		err(*env) << "Failed to create MPEG4 input device\n";
		exit(1);
	  }
  }
*/  

  // Create the RTSP server:
  RTSPServer* rtspServer = NULL;
  // Normal case: Streaming from a built-in RTSP server:
  rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, NULL);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "...done initializing \n";


  if( streamingMode == STREAMING_UNICAST )
  {
	    ServerMediaSession* sms
	      = ServerMediaSession::createNew(*env, H264StreamName[video_type], H264StreamName[video_type], streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISH264VideoServerMediaSubsession
				 ::createNew(sms->envir(), *H264InputDevice[video_type], H264VideoBitrate));
	    sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *H264InputDevice[video_type]));
	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
	    *env << "Play this stream using the URL:\t" << url << "\n";
	    delete[] url;  	
/*  	
	  if( video_type == VIDEO_TYPE_MJPEG)
	  {
	    ServerMediaSession* sms
	      = ServerMediaSession::createNew(*env, MjpegStreamName, MjpegStreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISJPEGVideoServerMediaSubsession
				 ::createNew(sms->envir(), *MjpegInputDevice, MjpegVideoBitrate));
		sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *MjpegInputDevice));
	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
	    *env << "Play this stream using the URL:\n\t" << url << "\n";
	    delete[] url;
	  }

	  if( video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF)
	  {
	    ServerMediaSession* sms
	      = ServerMediaSession::createNew(*env, H264StreamName, H264StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISH264VideoServerMediaSubsession
				 ::createNew(sms->envir(), *H264InputDevice, H264VideoBitrate));
	    sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *H264InputDevice));
	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
	    *env << "Play this stream using the URL:\n\t" << url << "\n";
	    delete[] url;
	  }

	    // Create a record describing the media to be streamed:
	  if( video_type == VIDEO_TYPE_MPEG4 || video_type == VIDEO_TYPE_MPEG4_CIF )
	  {
	    ServerMediaSession* sms
	      = ServerMediaSession::createNew(*env, Mpeg4StreamName, Mpeg4StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	    sms->addSubsession(WISMPEG4VideoServerMediaSubsession
				 ::createNew(sms->envir(), *Mpeg4InputDevice, Mpeg4VideoBitrate));
	    sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), *Mpeg4InputDevice));
	    rtspServer->addServerMediaSession(sms);

	    char *url = rtspServer->rtspURL(sms);
	    *env << "Play this stream using the URL:\n\t" << url << "\n";
	    delete[] url;
	  } 
*/	  
  }
  #if 0 //not use--whx
  else{

	if (streamingMode == STREAMING_MULTICAST_SSM) 
	{
		if (multicastAddress == 0) 
			multicastAddress = chooseRandomIPv4SSMAddress(*env);
	} else if (multicastAddress != 0) {
		streamingMode = STREAMING_MULTICAST_ASM;
	}

	struct in_addr dest; dest.s_addr = multicastAddress;
	const unsigned char ttl = 255;

	// For RTCP:
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char *) CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0';      // just in case
  	
	ServerMediaSession* sms;
	
	sms = ServerMediaSession::createNew(*env, H264StreamName[video_type], H264StreamName[video_type], streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
	
	sourceAudio = H264InputDevice[video_type]->audioSource();  
	sourceVideo = H264VideoStreamFramer::createNew(*env, H264InputDevice[video_type]->videoSource());

	// Create 'groupsocks' for RTP and RTCP:
    const Port rtpPortVideo(videoRTPPortNum);
    const Port rtcpPortVideo(videoRTPPortNum+1);
    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
    if (streamingMode == STREAMING_MULTICAST_SSM) {
      rtpGroupsockVideo->multicastSendOnly();
      rtcpGroupsockVideo->multicastSendOnly();
    }
	setVideoRTPSinkBufferSize();
	sinkVideo = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96, 0x42, "h264");	
/*
	if( video_type == VIDEO_TYPE_MJPEG)
	{	
		sms = ServerMediaSession::createNew(*env, MjpegStreamName, MjpegStreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
		
		sourceAudio = MjpegInputDevice->audioSource();
		sourceVideo = WISJPEGStreamSource::createNew(MjpegInputDevice->videoSource());
		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
		setVideoRTPSinkBufferSize();
		sinkVideo = JPEGVideoRTPSink::createNew(*env, rtpGroupsockVideo);
	}

	if( video_type == VIDEO_TYPE_H264 || video_type == VIDEO_TYPE_H264_CIF)
	{
 		sms = ServerMediaSession::createNew(*env, H264StreamName, H264StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);
 		
		sourceAudio = H264InputDevice->audioSource();  
		sourceVideo = H264VideoStreamFramer::createNew(*env, H264InputDevice->videoSource());

		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
		setVideoRTPSinkBufferSize();
		sinkVideo = H264VideoRTPSink::createNew(*env, rtpGroupsockVideo,96, 0x42, "h264");
	}

	// Create a record describing the media to be streamed:
	if( video_type == VIDEO_TYPE_MPEG4 || video_type == VIDEO_TYPE_MPEG4_CIF )
	{
		sms = ServerMediaSession::createNew(*env, Mpeg4StreamName, Mpeg4StreamName, streamDescription,streamingMode == STREAMING_MULTICAST_SSM);		   

		sourceAudio = Mpeg4InputDevice->audioSource();    
		sourceVideo = MPEG4VideoStreamDiscreteFramer::createNew(*env, Mpeg4InputDevice->videoSource());

		// Create 'groupsocks' for RTP and RTCP:
	    const Port rtpPortVideo(videoRTPPortNum);
	    const Port rtcpPortVideo(videoRTPPortNum+1);
	    rtpGroupsockVideo = new Groupsock(*env, dest, rtpPortVideo, ttl);
	    rtcpGroupsockVideo = new Groupsock(*env, dest, rtcpPortVideo, ttl);
	    if (streamingMode == STREAMING_MULTICAST_SSM) {
	      rtpGroupsockVideo->multicastSendOnly();
	      rtcpGroupsockVideo->multicastSendOnly();
	    }
		setVideoRTPSinkBufferSize();
		sinkVideo = MPEG4ESVideoRTPSink::createNew(*env, rtpGroupsockVideo,97);
		
	} 
*/	
	/* VIDEO Channel initial */
	if(1)
	{
		// Create (and start) a 'RTCP instance' for this RTP sink:
		unsigned totalSessionBandwidthVideo = (Mpeg4VideoBitrate+500)/1000; // in kbps; for RTCP b/w share
		rtcpVideo = RTCPInstance::createNew(*env, rtcpGroupsockVideo,
					totalSessionBandwidthVideo, CNAME,
					sinkVideo, NULL /* we're a server */ ,
					streamingMode == STREAMING_MULTICAST_SSM);
	    // Note: This starts RTCP running automatically
		sms->addSubsession(PassiveServerMediaSubsession::createNew(*sinkVideo, rtcpVideo));
		
		// Start streaming:
		sinkVideo->startPlaying(*sourceVideo, NULL, NULL);
	}		
	/* AUDIO Channel initial */
	if(1)
	{ 
		// there's a separate RTP stream for audio
		// Create 'groupsocks' for RTP and RTCP:
		const Port rtpPortAudio(audioRTPPortNum);
		const Port rtcpPortAudio(audioRTPPortNum+1);

		rtpGroupsockAudio = new Groupsock(*env, dest, rtpPortAudio, ttl);
		rtcpGroupsockAudio = new Groupsock(*env, dest, rtcpPortAudio, ttl);

		if (streamingMode == STREAMING_MULTICAST_SSM) 
		{
			rtpGroupsockAudio->multicastSendOnly();
			rtcpGroupsockAudio->multicastSendOnly();
		}
		if( audioSamplingFrequency == 16000 )
		{
			sinkAudio = SimpleRTPSink::createNew(*env, rtpGroupsockAudio, 96, audioSamplingFrequency, "audio", "PCMU", 1);
		}else{
			sinkAudio = SimpleRTPSink::createNew(*env, rtpGroupsockAudio, 0, audioSamplingFrequency, "audio", "PCMU", 1);
		}

		// Create (and start) a 'RTCP instance' for this RTP sink:
		unsigned totalSessionBandwidthAudio = (audioOutputBitrate+500)/1000; // in kbps; for RTCP b/w share
		rtcpAudio = RTCPInstance::createNew(*env, rtcpGroupsockAudio,
					  totalSessionBandwidthAudio, CNAME,
					  sinkAudio, NULL /* we're a server */,
					  streamingMode == STREAMING_MULTICAST_SSM);
		// Note: This starts RTCP running automatically
		sms->addSubsession(PassiveServerMediaSubsession::createNew(*sinkAudio, rtcpAudio));

		// Start streaming:
		sinkAudio->startPlaying(*sourceAudio, NULL, NULL);
    }
	
	rtspServer->addServerMediaSession(sms);
	{
		struct in_addr dest; dest.s_addr = multicastAddress;
		char *url = rtspServer->rtspURL(sms);
		//char *url2 = inet_ntoa(dest);
		*env << "Mulicast Play this stream using the URL:\n\t" << url << "\n";
		//*env << "2 Mulicast addr:\n\t" << url2 << "\n";
		delete[] url;
	}
  }
#endif //not use--whx

  // Begin the LIVE555 event loop:
  env->taskScheduler().doEventLoop(&watchVariable); // does not return
  
  
  if( streamingMode!= STREAMING_UNICAST )
  {
	Medium::close(rtcpAudio);
	Medium::close(sinkAudio);
	Medium::close(sourceAudio);
	delete rtpGroupsockAudio;
	delete rtcpGroupsockAudio;
  
	Medium::close(rtcpVideo);
	Medium::close(sinkVideo);
	Medium::close(sourceVideo);
	delete rtpGroupsockVideo;
	delete rtcpGroupsockVideo;

  }

  
  Medium::close(rtspServer); // will also reclaim "sms" and its "ServerMediaSubsession"s
  if( MjpegInputDevice != NULL )
  {
	Medium::close(MjpegInputDevice);
  }
  

  if( H264InputDevice[video_type] != NULL )
  {
	Medium::close(H264InputDevice[video_type]);
  }
   	
  if( Mpeg4InputDevice != NULL )
  {
	Medium::close(Mpeg4InputDevice);
  }
   
  env->reclaim();
   
  delete scheduler;
   
  NetraInterfaceExit();
  NetraDrvExit();
   
  return 0; // only to prevent compiler warning

}
