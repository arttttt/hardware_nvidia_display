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

hwc2_dev::hwc2_dev()
    : displays() { }

hwc2_dev::~hwc2_dev()
{
    hwc2_display::reset_ids();
}

int hwc2_dev::open_fb_device()
{
    int ret;

    ret = open_fb_display(0);
    if (ret < 0)
        goto err;

    if (displays.empty()) {
        ALOGE("failed to open any physical displays");
        ret = -1;
        goto err;
    }

err:
    return ret;
}

int hwc2_dev::open_fb_display(int fb_id) {
    struct nvfb_device nvfb_dev;

    int ret = nvfb_device_open(fb_id, O_RDWR, &nvfb_dev);
    if (ret < 0) {
        ALOGE("failed to open fb%u device: %s", fb_id, strerror(ret));
        return ret;
    }

    ALOGE("panel height: %d\npanel width: %d", 
                nvfb_dev.vi.yres, nvfb_dev.vi.xres);

    ALOGE("panel virtual height: %d\npanel virtual width: %d", 
                nvfb_dev.vi.yres_virtual, nvfb_dev.vi.xres_virtual);

    hwc2_display_t dpy_id = hwc2_display::get_next_id();
    displays.emplace(std::piecewise_construct, std::forward_as_tuple(dpy_id),
            std::forward_as_tuple(dpy_id,
                                    nvfb_dev));

    return 0;
}
