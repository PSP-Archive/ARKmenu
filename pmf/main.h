#ifndef MAIN_H
#define MAIN_H

#include <common.h>

#include <pspkernel.h>
#include <pspsdk.h>
#include <psptypes.h>
#include <psppower.h>

#define DISPLAY_VIDEO 1

//#include "pmfplayer.h"
#include <psputilsforkernel.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <psputility.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <pspmpeg.h>

#define BUFFER_WIDTH 512
#define SWAPINT(x) (((x)<<24) | (((uint)(x)) >> 24) | (((x) & 0x0000FF00) << 8) | (((x) & 0x00FF0000) >> 8))

enum
{
	ReaderThreadData__READER_OK = 0,
	ReaderThreadData__READER_EOF,
	ReaderThreadData__READER_ABORT
};

#define PIXEL_SIZE          4

#define SCREEN_W            480
#define SCREEN_H            272
#define DRAW_BUFFER_SIZE    512 * SCREEN_H * PIXEL_SIZE
#define DISP_BUFFER_SIZE    512 * SCREEN_H * PIXEL_SIZE

#define TEXTURE_W           512
#define TEXTURE_H           512
#define TEXTURE_SIZE        TEXTURE_W * TEXTURE_H * PIXEL_SIZE

#define BUFFER_WIDTH            512

struct Vertex
{
	short u,v;
	short x,y,z;
};

typedef struct VideoThreadData
{
	SceUID                          m_SemaphoreStart;
	SceUID                          m_SemaphoreWait;
	SceUID                          m_SemaphoreLock;
	SceUID                          m_ThreadID;

	ScePVoid                        m_pVideoBuffer[2];
	SceInt32                        m_iBufferTimeStamp[2];

	SceInt32                        m_iNumBuffers;
	SceInt32                        m_iFullBuffers;
	SceInt32                        m_iPlayBuffer;

	SceInt32                        m_iAbort;

	SceInt32                        m_iWidth;
	SceInt32                        m_iHeight;

} VideoThreadData;

typedef struct {
	SceUID                          m_Semaphore;
	SceUID                          m_ThreadID;

	SceInt32                        m_StreamSize;
	SceMpegRingbuffer*              m_Ringbuffer;
	SceInt32                        m_RingbufferPackets;
	SceInt32                        m_Status;
	SceInt32                        m_TotalBytes;
} ReaderThreadData;

typedef struct
{
	SceUID                          m_SemaphoreStart;
	SceUID                          m_SemaphoreLock;
	SceUID                          m_ThreadID;

	SceInt32                        m_AudioChannel;

	ScePVoid                        m_pAudioBuffer[4];
	SceInt32                        m_iBufferTimeStamp[4];

	SceInt32                        m_iNumBuffers;
	SceInt32                        m_iFullBuffers;
	SceInt32                        m_iPlayBuffer;
	SceInt32                        m_iDecodeBuffer;

	SceInt32                        m_iAbort;
} AudioThreadData;


typedef struct {
	SceUID                          m_ThreadID;

	ReaderThreadData*               Reader;
	VideoThreadData*                Video;
	AudioThreadData*                Audio;

	SceMpeg                         m_Mpeg;

	SceMpegStream*                  m_MpegStreamAVC;
	SceMpegAu*                      m_MpegAuAVC;
	SceMpegStream*                  m_MpegStreamAtrac;
	SceMpegAu*                      m_MpegAuAtrac;
	SceInt32                        m_MpegAtracOutSize;

	SceInt32                        m_iAudioFrameDuration;
	SceInt32                        m_iVideoFrameDuration;
	SceInt32                        m_iLastTimeStamp;
} DecoderThreadData;

typedef struct {
	int   packets;
	uint  packetsRead;
	uint  packetsWritten;
	uint  packetsFree;
	uint  packetSize;
	void* data;
	uint  callback;
	void* callbackParameter;
	void* dataUpperBound;
	int   semaId;
	SceMpeg* mpeg;
} _SceMpegRingbuffer;

typedef struct {
	uint   magic1;
	uint   magic2;
	uint   magic3;
	uint   unk_m1;
	void*  ringbuffer_start;
	void*  ringbuffer_end;
} _SceMpeg;

extern int retVal;
extern SceMpegAvcMode m_MpegAvcMode;

extern ReaderThreadData                    Reader;
extern VideoThreadData                     Video;
extern AudioThreadData                     Audio;
extern DecoderThreadData                   Decoder;

extern SceUID                              m_FileHandle;
extern SceInt32                            m_MpegStreamOffset;
extern SceInt32                            m_MpegStreamSize;

extern SceMpeg                             m_Mpeg;
extern SceInt32                            m_MpegMemSize;
extern ScePVoid                            m_MpegMemData;

extern SceInt32                            m_RingbufferPackets;
extern SceInt32                            m_RingbufferSize;
extern ScePVoid                            m_RingbufferData;
extern SceMpegRingbuffer                   m_Ringbuffer;

extern SceMpegStream*                      m_MpegStreamAVC;
extern ScePVoid                            m_pEsBufferAVC;
extern SceMpegAu                           m_MpegAuAVC;

extern SceMpegStream*                      m_MpegStreamAtrac;
extern ScePVoid                            m_pEsBufferAtrac;
extern SceMpegAu                           m_MpegAuAtrac;

extern SceInt32                            m_MpegAtracEsSize;
extern SceInt32                            m_MpegAtracOutSize;

extern SceInt32                            m_iLastTimeStamp;


SceInt32 RingbufferCallback(ScePVoid pData, SceInt32 iNumPackets, ScePVoid pParam);
SceInt32 ParseHeader();

void Init();
void Load(char* pFileName);
int Play();
SceVoid Shutdown();

#endif
