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
 
#include <fcntl.h>
#include <cutils/log.h>
#include <cstdlib>
#include <vector>

#include "include/tegrafb.h"
#include "hwc2.h"

hwc2_dev::hwc2_dev() { }
      
hwc2_dev::~hwc2_dev() { }

const char* fb_path = "/dev/graphics/fb0";

void get_modes(struct fb_device *dev) 
{
    struct tegra_fb_modedb fb_modedb;
    
    if (ioctl(dev->fd, FBIO_TEGRA_GET_MODEDB, &fb_modedb)) {
        return;
    }

    ALOGD("Display: found %d modes", fb_modedb.modedb_len);
}

int hwc2_dev::open_fb_device()
{
    int fb;
    struct fb_device *dev;
    
    dev = (struct fb_device*) malloc(sizeof(struct fb_device));
    if (!dev) {
        return -1;
    }
    
    memset(dev, 0, sizeof(struct fb_device));
    
    fb = open(fb_path, O_RDWR, 0);
    
    if (fb == -1) {
        ALOGE("Can't open framebuffer device\n");
        return fb;
    }
    
    dev->fd = fb;
    
    get_modes(dev);
    
    return 0;
}
