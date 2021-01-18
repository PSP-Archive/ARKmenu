#ifndef PMF_DECODER
#define PMF_DECODER

#include "main.h"

#define DEBUG_TIMING    0

SceInt32 IsRingbufferFull(ReaderThreadData* D);

int T_Decoder(SceSize _args, void *_argp);

SceInt32 InitDecoder();

SceInt32 ShutdownDecoder();

#endif
