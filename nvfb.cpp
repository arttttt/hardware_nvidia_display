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

int nvfb_device_open(struct nvfb_device **dev, struct nvfb_callbacks *callbacks)
{
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
