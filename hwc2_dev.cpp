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
#include <cstdlib>
#include <fcntl.h>
#include <vector>

#include "hwc2.h"

static void hwc2_vsync(void* /*data*/, int /*dpy_id*/, uint64_t /*timestamp*/)
{
    return;
}

static void hwc2_hotplug(void* /*data*/, int /*dpy_id*/, bool /*connected*/)
{
	ALOGE("hwc2_hotplug event\n");

    return;
}

const struct nvfb_callbacks hwc2_fb_callbacks = {
    .vsync = hwc2_vsync,
    .hotplug = hwc2_hotplug,
};

hwc2_dev::hwc2_dev()
    : callback_handler(),
    : displays() { }

hwc2_dev::~hwc2_dev() 
{
    hwc2_display::reset_ids();
}

int hwc2_dev::open_fb_device()
{
    int ret = open_fb_display(0);

	ALOGE("fb%u device: %s successfully opened", 0, strerror(ret));

    return ret;
}

hwc2_error_t hwc2_dev::register_callback(hwc2_callback_descriptor_t descriptor,
        hwc2_callback_data_t callback_data, hwc2_function_pointer_t pointer)
{
    if (descriptor == HWC2_CALLBACK_INVALID) {
        ALOGE("invalid callback descriptor %u", descriptor);
        return HWC2_ERROR_BAD_PARAMETER;
    }
     return callback_handler.register_callback(descriptor, callback_data,
            pointer);
}

int hwc2_dev::open_fb_display(int fb_id)
{
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
            std::forward_as_tuple(dpy_id, nvfb_dev));

    return 0;
}
