#ifndef PMF_AUDIO_H
#define PMF_AUDIO_H

#include "main.h"

SceInt32 AudioSyncStatus(DecoderThreadData* D);

int T_Audio(SceSize _args, void *_argp);

SceInt32 InitAudio();

SceInt32 ShutdownAudio();

#endif
