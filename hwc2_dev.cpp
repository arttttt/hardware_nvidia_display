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

#include "hwc2.h"

static void hwc2_vsync(void* /*data*/, int /*dpy_id*/, uint64_t /*timestamp*/)
{
    return;
}

static void hwc2_hotplug(void* /*data*/, int /*dpy_id*/, bool /*connected*/)
{
    return;
}

 onst struct fb_callbacks hwc2_fb_callbacks = {
    .vsync = hwc2_vsync,
    .hotplug = hwc2_hotplug,
};

hwc2_dev::hwc2_dev()
    : displays() { }

hwc2_dev::~hwc2_dev() 
{
    hwc2_display::reset_ids();
}

int hwc2_dev::open_fb_device()
{
    return 0;
}

int open_fb_display(int fb_id)
{
    return 0;
}
