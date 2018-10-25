/*
 * Copyright (C) 2018 arttttt <artem-bambalov@yandex.ru>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "nvfb.h"

#define FB_BASE_PATH "/dev/graphics/"

int nvfb_device_open(int id, int flags, struct nvfb_device *dev)
{
    char filename[64];

    dev->id = id;

    snprintf(filename, sizeof(filename), FB_BASE_PATH "fb%u", id);
    dev->fd = open(filename, flags);
    if (dev->fd < 0)
        return -errno;

    if (ioctl(dev->fd, FBIOGET_VSCREENINFO, &dev->vi) < 0) {
        ALOGE("failed to get fb0 info (FBIOGET_VSCREENINFO)");
        close(dev->fd);
        return NULL;
    }

    if (ioctl(dev->fd, FBIOGET_FSCREENINFO, &dev->fi) < 0) {
        ALOGE("failed to get fb0 info (FBIOGET_FSCREENINFO)");
        close(dev->fd);
        return NULL;
	}

    ALOGD("fb%d reports (possibly inaccurate):\n"
            "  vi.bits_per_pixel = %d\n"
            "  vi.red.offset   = %3d   .length = %3d\n"
            "  vi.green.offset = %3d   .length = %3d\n"
            "  vi.blue.offset  = %3d   .length = %3d\n",
            id,
            dev->vi.bits_per_pixel,
            dev->vi.red.offset, dev->vi.red.length,
            dev->vi.green.offset, dev->vi.green.length,
            dev->vi.blue.offset, dev->vi.blue.length);

    return 0;
}
