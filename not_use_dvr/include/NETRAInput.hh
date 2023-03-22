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
// An interface to the WIS GO7007 capture device.
// C++ header

#ifndef _NETRA_INPUT_HH
#define _NETRA_INPUT_HH

#include <MediaSink.hh>
#include <Netra_interface.h>

class NETRAInput: public Medium {
public:
  static NETRAInput* createNew(UsageEnvironment& env, int vType);

  FramedSource* videoSource();
  FramedSource* audioSource();
  virtual ~NETRAInput();

private:
  NETRAInput(UsageEnvironment& env, int vType); // called only by createNew()
  

//  Boolean initialize(UsageEnvironment& env);
//  Boolean openFiles(UsageEnvironment& env);
//  static Boolean initALSA(UsageEnvironment& env);
//  static Boolean initV4L(UsageEnvironment& env);
//  static unsigned long getFrame(UsageEnvironment& env, unsigned char *addr, unsigned long len);
//  static void listVideoInputDevices(UsageEnvironment& env);

private:
  friend class VideoOpenFileSource;
  friend class AudioOpenFileSource;
  int videoType;
  int channel;
//  int fVideoFileNo;
  FramedSource* fOurVideoSource;
//  static int fOurAudioFileNo;
  FramedSource* fOurAudioSource;
};

//RTSP_EXT_CHANNEL
enum{
	VIDEO_TYPE_H264_CH1	= 0,
	VIDEO_TYPE_H264_CH2,
	VIDEO_TYPE_H264_CH3,
	VIDEO_TYPE_H264_CH4,
	
	VIDEO_TYPE_H264_CH5,
	VIDEO_TYPE_H264_CH6,
	VIDEO_TYPE_H264_CH7,
	VIDEO_TYPE_H264_CH8,
	VIDEO_TYPE_H264_CH9,
	VIDEO_TYPE_H264_CH10,
	VIDEO_TYPE_H264_CH11,
	VIDEO_TYPE_H264_CH12,
	VIDEO_TYPE_H264_CH13,
	VIDEO_TYPE_H264_CH14,
	VIDEO_TYPE_H264_CH15,
	VIDEO_TYPE_H264_CH16,
	VIDEO_TYPE_MAX_NUM
};

/*
enum{
	VIDEO_TYPE_MPEG4	= 0,
	VIDEO_TYPE_MPEG4_CIF,
	VIDEO_TYPE_H264		,
	VIDEO_TYPE_H264_CIF	,
	VIDEO_TYPE_MJPEG	,
};
*/

// Functions to set the optimal buffer size for RTP sink objects.
// These should be called before each RTPSink is created.
#define AUDIO_MAX_FRAME_SIZE 20480
#define VIDEO_MAX_FRAME_SIZE 400000
inline void setAudioRTPSinkBufferSize() { OutPacketBuffer::maxSize = AUDIO_MAX_FRAME_SIZE; }
inline void setVideoRTPSinkBufferSize() { OutPacketBuffer::maxSize = VIDEO_MAX_FRAME_SIZE; }

#endif
