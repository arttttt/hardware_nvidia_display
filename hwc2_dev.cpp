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

#include <errno.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <cstdlib>
#include <vector>

#include "hwc2.h"

#define TEGRA_CTRL_PATH "/dev/tegra_dc_ctrl"

static int hotplug(void *data, int disp, struct nvfb_hotplug_status hotplug)
{
    return 0;
}

static int acquire(void *data, int disp)
{
    return 0;
}

static int release(void *data, int disp)
{
    return 0;
}

static int bandwidth_change(void *data)
{
    return 0;
}

hwc2_dev::hwc2_dev()
    : displays() { }

hwc2_dev::~hwc2_dev() { }

int hwc2_dev::open_fb_device()
{
    int ret;
    struct nvfb_device *dev;

    struct nvfb_callbacks callbacks = {
        .hotplug = hotplug,
        .acquire = acquire,
        .release = release,
        .bandwidth_change = bandwidth_change,
    };

    dev = (struct nvfb_device*) malloc(sizeof(struct nvfb_device));
    if (!dev) {
        return -1;
    }

    memset(dev, 0, sizeof(struct nvfb_device));
    dev->ctrl_fd = -1;

    dev->callbacks = callbacks;

    ret = get_displays(dev);
    if (ret < 0)
        return ret;

    return 0;
}

int hwc2_dev::get_displays(struct nvfb_device *dev)
{
    int num, ii, ret;

    dev->ctrl_fd = open(TEGRA_CTRL_PATH, O_RDWR);
    if (dev->ctrl_fd < 0) {
        return errno;
    }

    if (ioctl(dev->ctrl_fd, TEGRA_DC_EXT_CONTROL_GET_NUM_OUTPUTS, &num)) {
        return errno;
    }

    ALOGE("TEGRA_DC_EXT_CONTROL_GET_NUM_OUTPUTS = %d", num);

    for (ii = 0; ii < num; ii++) {
        struct tegra_dc_ext_control_output_properties props;
        hwc2_display_t dpy_id = ii;
        hwc2_display dpy(dpy_id);

        props.handle = ii;
        if (ioctl(dev->ctrl_fd, TEGRA_DC_EXT_CONTROL_GET_OUTPUT_PROPERTIES,
                  &props)) {
            return errno;
        }

        ret = open_display(dev, &dpy);
        if (ret < 0)
            return ret;
    }

	return 0;
}

int hwc2_dev::open_display(struct nvfb_device *dev, hwc2_display *dpy)
{
    return 0;
}
