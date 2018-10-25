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

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>

#include "fb.h"

#define FB_BASE_PATH "/dev/graphics/"

int fb_device_open(int id, int flags, struct fb_device *dev)
{
    char filename[64];

    dev->id = id;

    snprintf(filename, sizeof(filename), FB_BASE_PATH "fb%u", id);
    dev->fd = open(filename, flags);
    if (dev->fd < 0)
        return -errno;

    return 0;
}
