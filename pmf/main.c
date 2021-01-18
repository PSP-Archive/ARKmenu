#include "main.h"

#include "pmf_decoder.h"
#include "pmf_reader.h"
#include "pmf_video.h"
#include "pmf_audio.h"

PSP_MODULE_INFO("pmf", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int retVal;
SceMpegAvcMode m_MpegAvcMode;

ReaderThreadData                    Reader;
VideoThreadData                     Video;
AudioThreadData                     Audio;
DecoderThreadData                   Decoder;

SceUID                              m_FileHandle;
SceInt32                            m_MpegStreamOffset;
SceInt32                            m_MpegStreamSize;

SceMpeg                             m_Mpeg;
SceInt32                            m_MpegMemSize;
ScePVoid                            m_MpegMemData;

SceInt32                            m_RingbufferPackets;
SceInt32                            m_RingbufferSize;
ScePVoid                            m_RingbufferData;
SceMpegRingbuffer                   m_Ringbuffer;

SceMpegStream*                      m_MpegStreamAVC;
ScePVoid                            m_pEsBufferAVC;
SceMpegAu                           m_MpegAuAVC;

SceMpegStream*                      m_MpegStreamAtrac;
ScePVoid                            m_pEsBufferAtrac;
SceMpegAu                           m_MpegAuAtrac;

SceInt32                            m_MpegAtracEsSize;
SceInt32                            m_MpegAtracOutSize;

SceInt32                            m_iLastTimeStamp;

int main(int argc, char *argv[]) {
	Init();
	Load("test.pmf");
	Play();
	Shutdown();

	sceKernelExitGame();

	return 0;
}

SceInt32 RingbufferCallback(ScePVoid pData, SceInt32 iNumPackets, ScePVoid pParam)
{
	int retVal, iPackets;
	SceUID hFile = *(SceUID*)pParam;

	printf("RingbufferCallback(pData=0x%08X, iNumPackets=0x%08X, pParam=0x%08X)\n", (unsigned int)pData, (unsigned int)iNumPackets, (unsigned int)pParam);

	retVal = sceIoRead(hFile, pData, iNumPackets * 2048);
	if(retVal < 0) return -1;

	iPackets = retVal / 2048;

	return iPackets;
}

SceInt32 ParseHeader()
{
	int retVal;
	char * pHeader = (char *)malloc(2048);

	sceIoLseek(m_FileHandle, 0, SEEK_SET);

	retVal = sceIoRead(m_FileHandle, pHeader, 2048);
	if (retVal < 2048)
	{
		printf("sceIoRead() failed!\n");
		goto error;
	}

	printf("sceMpegQueryStreamOffset     :0x%08X\n", (unsigned int)(retVal = sceMpegQueryStreamOffset(&m_Mpeg, pHeader, &m_MpegStreamOffset)));
	printf("  value: 0x%08X\n", (unsigned int)m_MpegStreamOffset);
	if (retVal != 0)
	{
		printf("sceMpegQueryStreamOffset() failed: 0x%08X\n", retVal);
		goto error;
	}

	printf("sceMpegQueryStreamSize       :0x%08X\n", (unsigned int)(retVal = sceMpegQueryStreamSize(pHeader, &m_MpegStreamSize)));
	printf("  value: 0x%08X\n", (unsigned int)m_MpegStreamSize);
	if (retVal != 0)
	{
		printf("sceMpegQueryStreamSize() failed: 0x%08X\n", retVal);
		goto error;
	}

	m_iLastTimeStamp = *(int*)(pHeader + 80 + 12);
	m_iLastTimeStamp = SWAPINT(m_iLastTimeStamp);

	printf("m_iLastTimeStamp             :0x%08X\n", (unsigned int)(m_iLastTimeStamp));

	free(pHeader);

	sceIoLseek(m_FileHandle, m_MpegStreamOffset, SEEK_SET);

	return 0;

error:
	free(pHeader);
	return -1;
}

void Init() {
	m_RingbufferPackets = 0x3C0;

	int status = 0;
	status |= sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC);
	status |= sceUtilityLoadModule(PSP_MODULE_AV_ATRAC3PLUS);
	status |= sceUtilityLoadModule(PSP_MODULE_AV_MP3);
	status |= sceUtilityLoadModule(PSP_MODULE_AV_MPEGBASE);
	status |= sceUtilityLoadModule(PSP_MODULE_AV_VAUDIO);
	printf("sceUtilityLoadModule         :0x%08x\n", (unsigned int)status);
	
	printf("sceMpegInit                  :0x%08X\n", (unsigned int)sceMpegInit());
	printf("sceMpegRingbufferQueryMemSize:0x%08X\n", (unsigned int)(m_RingbufferSize = sceMpegRingbufferQueryMemSize(m_RingbufferPackets)));
	
	printf("sceMpegQueryMemSize          :0x%08X\n", (unsigned int)(m_MpegMemSize    = sceMpegQueryMemSize(0)));
	printf("m_RingbufferData             :0x%08X\n", (unsigned int)(m_RingbufferData = malloc(m_RingbufferSize)));
	printf("m_MpegMemData                :0x%08X\n", (unsigned int)(m_MpegMemData    = malloc(m_MpegMemSize)));
	printf("sceMpegRingbufferConstruct   :0x%08X\n", (unsigned int)(sceMpegRingbufferConstruct(&m_Ringbuffer, m_RingbufferPackets, m_RingbufferData, m_RingbufferSize, &RingbufferCallback, &m_FileHandle)));
	printf("    packets          : %d\n"    , (         int)((_SceMpegRingbuffer *)&m_Ringbuffer)->packets);
	printf("    packetsRead      : %d\n"    , (         int)((_SceMpegRingbuffer *)&m_Ringbuffer)->packetsRead);
	printf("    packetsWritten   : %d\n"    , (         int)((_SceMpegRingbuffer *)&m_Ringbuffer)->packetsWritten);
	printf("    packetsFree      : %d\n"    , (         int)((_SceMpegRingbuffer *)&m_Ringbuffer)->packetsFree);
	printf("    packetSize       : %d\n"    , (         int)((_SceMpegRingbuffer *)&m_Ringbuffer)->packetSize);
	printf("    data             : 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->data);
	printf("    callback         : 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->callback);
	printf("    callbackParameter: 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->callbackParameter);
	printf("    dataUpperBound   : 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->dataUpperBound);
	printf("    semaId           : 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->semaId);
	printf("    mpeg             : 0x%08X\n", (unsigned int)((_SceMpegRingbuffer *)&m_Ringbuffer)->mpeg);
	printf("sceMpegCreate                :0x%08X\n", (unsigned int)(sceMpegCreate(&m_Mpeg, m_MpegMemData, m_MpegMemSize, &m_Ringbuffer, BUFFER_WIDTH, 0, 0)));
	printf("    pointer          : '%s'\n"  , (char *)m_Mpeg);
	printf("    unk_m1           : 0x%08X\n", (unsigned int)((_SceMpeg *)m_Mpeg)->unk_m1);
	printf("    ringbuffer_start : 0x%08X\n", (unsigned int)((_SceMpeg *)m_Mpeg)->ringbuffer_start);
	printf("    ringbuffer_end   : 0x%08X\n", (unsigned int)((_SceMpeg *)m_Mpeg)->ringbuffer_end);
	
	m_MpegAvcMode.iUnk0 = -1;
	m_MpegAvcMode.iPixelFormat = 3;
	printf("sceMpegAvcDecodeMode         :0x%08X\n", (unsigned int)(sceMpegAvcDecodeMode(&m_Mpeg, &m_MpegAvcMode)));
}

void Load(char* pFileName) {
	printf("sceIoOpen                    :0x%08X\n", (unsigned int)(m_FileHandle = sceIoOpen(pFileName, PSP_O_RDONLY, 0777)));
	ParseHeader();
	printf("sceMpegRegistStream          :0x%08X\n", (unsigned int)(m_MpegStreamAVC = sceMpegRegistStream(&m_Mpeg, 0, 0)));
	printf("sceMpegRegistStream          :0x%08X\n", (unsigned int)(m_MpegStreamAtrac = sceMpegRegistStream(&m_Mpeg, 1, 0)));
	printf("sceMpegMallocAvcEsBuf        :0x%08X\n", (unsigned int)(m_pEsBufferAVC = sceMpegMallocAvcEsBuf(&m_Mpeg)));
	printf("sceMpegInitAu                :0x%08X\n", (unsigned int)(retVal = sceMpegInitAu(&m_Mpeg, m_pEsBufferAVC, &m_MpegAuAVC)));
	printf("   iPtsMSB           : 0x%08X\n", (unsigned int)m_MpegAuAVC.iPtsMSB);
	printf("   iPts              : 0x%08X\n", (unsigned int)m_MpegAuAVC.iPts);
	printf("   iDtsMSB           : 0x%08X\n", (unsigned int)m_MpegAuAVC.iDtsMSB);
	printf("   iDts              : 0x%08X\n", (unsigned int)m_MpegAuAVC.iDts);
	printf("   iEsBuffer         : 0x%08X\n", (unsigned int)m_MpegAuAVC.iEsBuffer);
	printf("   iAuSize           : 0x%08X\n", (unsigned int)m_MpegAuAVC.iAuSize);
	printf("sceMpegQueryAtracEsSize      :0x%08X\n", (unsigned int)(retVal = sceMpegQueryAtracEsSize(&m_Mpeg, &m_MpegAtracEsSize, &m_MpegAtracOutSize)));
	printf("   m_MpegAtracEsSize : %d\n", (int)m_MpegAtracEsSize);
	printf("   m_MpegAtracOutSize: %d\n", (int)m_MpegAtracOutSize);
	printf("m_pEsBufferAtrac             :0x%08X\n", (unsigned int)(m_pEsBufferAtrac = memalign(64, m_MpegAtracEsSize)));
	printf("sceMpegInitAu                :0x%08X\n", (unsigned int)(retVal = sceMpegInitAu(&m_Mpeg, m_pEsBufferAtrac, &m_MpegAuAtrac)));
}

int Play() {
	int retVal, fail = 0;

	retVal = InitReader();
	printf("  : 0x%08X\n", (unsigned int)retVal);
	if (retVal < 0)
	{
		fail++;
		goto exit_reader;
	}
	
	retVal = InitVideo();
	printf("  : 0x%08X\n", (unsigned int)retVal);
	if (retVal < 0)
	{
		fail++;
		goto exit_video;
	}

	retVal = InitAudio();
	printf("  : 0x%08X\n", (unsigned int)retVal);
	if (retVal < 0)
	{
		fail++;
		goto exit_audio;
	}

	retVal = InitDecoder();
	printf("  : 0x%08X\n", (unsigned int)retVal);
	if (retVal < 0)
	{
		fail++;
		goto exit_decoder;
	}

	ReaderThreadData * TDR = &Reader;
	DecoderThreadData* TDD = &Decoder;

	sceKernelStartThread(Reader.m_ThreadID,  sizeof(void*), &TDR);
	sceKernelStartThread(Audio.m_ThreadID,   sizeof(void*), &TDD);
	sceKernelStartThread(Video.m_ThreadID,   sizeof(void*), &TDD);
	sceKernelStartThread(Decoder.m_ThreadID, sizeof(void*), &TDD);

	sceKernelWaitThreadEnd(Decoder.m_ThreadID, 0);
	sceKernelWaitThreadEnd(Video.m_ThreadID, 0);
	sceKernelWaitThreadEnd(Audio.m_ThreadID, 0);
	sceKernelWaitThreadEnd(Reader.m_ThreadID, 0);

	ShutdownDecoder();
exit_decoder:
	ShutdownAudio();
exit_audio:
	ShutdownVideo();
exit_video:
	ShutdownReader();
exit_reader:

	if (fail > 0) return -1;

	return 0;
}

void Shutdown()
{
	if (m_pEsBufferAtrac  != NULL) free(m_pEsBufferAtrac);
	if (m_pEsBufferAVC    != NULL) sceMpegFreeAvcEsBuf(&m_Mpeg, m_pEsBufferAVC);
	if (m_MpegStreamAVC   != NULL) sceMpegUnRegistStream(&m_Mpeg, m_MpegStreamAVC);
	if (m_MpegStreamAtrac != NULL) sceMpegUnRegistStream(&m_Mpeg, m_MpegStreamAtrac);
	if (m_FileHandle      > -1   ) sceIoClose(m_FileHandle);

	sceMpegDelete(&m_Mpeg);
	sceMpegRingbufferDestruct(&m_Ringbuffer);
	sceMpegFinish();

	if (m_RingbufferData != NULL) free(m_RingbufferData);
	if (m_MpegMemData    != NULL) free(m_MpegMemData);
}
