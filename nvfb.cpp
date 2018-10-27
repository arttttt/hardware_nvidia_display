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
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
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
        return -1;

    if (ioctl(dev->fd, FBIOGET_VSCREENINFO, &dev->vi) < 0) {
        ALOGE("failed to get fb0 info (FBIOGET_VSCREENINFO)");
        close(dev->fd);
        return -1;
    }

    if (ioctl(dev->fd, FBIOGET_FSCREENINFO, &dev->fi) < 0) {
        ALOGE("failed to get fb0 info (FBIOGET_FSCREENINFO)");
        close(dev->fd);
        return -1;
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

    dev->data = mmap(0, dev->fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, dev->fd, 0);
    if (dev->data == MAP_FAILED) {
        ALOGE("failed to mmap framebuffer");
        close(dev->fd);
        return NULL;
    }

    memset(dev->data, 0, dev->fi.smem_len);

    nvfb_blank(dev, true);
    nvfb_blank(dev, false);

    return 0;
}

void nvfb_blank(struct nvfb_device *dev, bool blank)
{
    int ret;

    ret = ioctl(dev->fd, FBIOBLANK, blank ? FB_BLANK_POWERDOWN : FB_BLANK_UNBLANK);
    if (ret < 0)
        ALOGE("ioctl(): blank");
}

void nvfb_write(struct nvfb_device *dev, void* new_data)
{
    memcpy(dev->data, new_data, dev->vi.xres * dev->vi.yres * dev->vi.bits_per_pixel / 8);
}
