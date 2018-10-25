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
#include <inttypes.h>
#include <vector>

#include "hwc2.h"

static void hwc2_vsync(void* /*data*/, int /*dpy_id*/, uint64_t /*timestamp*/)
{
    return;
}

hwc2_error_t hwc2_dev::get_display_type(hwc2_display_t dpy_id,
        hwc2_display_type_t *out_type) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    *out_type = it->second.get_type();
    return HWC2_ERROR_NONE;
}

static void hwc2_hotplug(void *data, int dpy_id, bool connected)
{
	ALOGE("hwc2_hotplug event\n");

    hwc2_dev *dev = static_cast<hwc2_dev *>(data);
    dev->hotplug(static_cast<hwc2_display_t>(dpy_id),
            (connected)? HWC2_CONNECTION_CONNECTED:
            HWC2_CONNECTION_DISCONNECTED);
}

const struct nvfb_callbacks hwc2_fb_callbacks = {
    .vsync = hwc2_vsync,
    .hotplug = hwc2_hotplug,
};

hwc2_dev::hwc2_dev()
	: callback_handler(),
	  displays() { }

hwc2_dev::~hwc2_dev() 
{
    hwc2_display::reset_ids();
}

int hwc2_dev::open_fb_device()
{
    int ret = open_fb_display(0);
    if (ret < 0)
        goto err;

	ALOGE("fb%u device: %s successfully opened", 0, strerror(ret));

    for (auto &dpy: displays) {
        ret = dpy.second.retrieve_display_configs();
        if (ret < 0) {
            ALOGE("dpy %" PRIu64 ": failed to retrieve display configs: %s",
                    dpy.second.get_id(), strerror(ret));
            goto err;
        }
    }

    for (auto &dpy: displays)
        callback_handler.call_hotplug(dpy.second.get_id(),
                dpy.second.get_connection());

err:
    return ret;
}

void hwc2_dev::hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGW("dpy %" PRIu64 ": invalid display handle preventing hotplug"
                " callback", dpy_id);
        return;
    }

    hwc2_error_t ret = it->second.set_connection(connection);
    if (ret != HWC2_ERROR_NONE)
        return;

    callback_handler.call_hotplug(dpy_id, connection);
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
            std::forward_as_tuple(dpy_id,
                                    nvfb_dev,
                                    HWC2_CONNECTION_CONNECTED,
                                    HWC2_DISPLAY_TYPE_PHYSICAL));

    return 0;
}
