#ifndef PMF_VIDEO_H
#define PMF_VIDEO_H

#include "main.h"

static unsigned int __attribute__((aligned(64)))  DisplayList[128 * 1024 * 4];

SceInt32 AVSyncStatus(DecoderThreadData* D);

int RenderFrame(int width, int height, void* Buffer);

int T_Video(SceSize _args, void *_argp);

SceInt32 InitVideo();

SceInt32 ShutdownVideo();

#endif
