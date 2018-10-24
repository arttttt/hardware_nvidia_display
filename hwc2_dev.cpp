/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include "hwc2.h"

hwc2_dev::hwc2_dev() { }
      
hwc2_dev::~hwc2_dev() { }

int hwc2_dev::open_fb_device()
{
    struct fb_device *dev;
    
    dev = (struct fb_device*) malloc(sizeof(struct fb_device));
    if (!dev) {
        return -1;
    }
    
    memset(dev, 0, sizeof(struct fb_device));
    
    open_fb_display(dev);
    
    return 0;
}

const char* fb_path = "/dev/graphics/fb0";

int hwc2_dev::open_fb_display(struct fb_device *dev) 
{
    int fb;
    
    fb = open(fb_path, O_RDWR, 0);
    
    if (fb == -1) {
        ALOGE("Can't open framebuffer device\n");
        return fb;
    }
    
    dev->fd = fb;
    
    return 0;
}
