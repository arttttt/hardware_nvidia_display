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

static void hwc2_vsync(void* /*data*/, int /*dpy_id*/, uint64_t /*timestamp*/)
{
    return;
}

static void hwc2_hotplug(void* /*data*/, int /*dpy_id*/, bool /*connected*/)
{
    return;
}

static void hwc2_custom_event(void* /*data*/, int /*dpy_id*/,
        struct adf_event* /*event*/)
{
    return;
}

const struct adf_hwc_event_callbacks hwc2_adfhwc_callbacks = {
    .vsync = hwc2_vsync,
    .hotplug = hwc2_hotplug,
    .custom_event = hwc2_custom_event,
};

hwc2_dev::hwc2_dev()
    : displays(),
      adf_helper(nullptr) { }

hwc2_dev::~hwc2_dev()
{
    if (adf_helper)
        adf_hwc_close(adf_helper);
    hwc2_display::reset_ids();
}

int hwc2_dev::open_adf_device()
{
    adf_id_t *dev_ids = nullptr;
    int ret;

    ssize_t n_devs = adf_devices(&dev_ids);
    if (n_devs < 0) {
        ALOGE("failed to enumerate adf devices: %s", strerror(n_devs));
        return n_devs;
    }

    std::vector<int> intf_fds;

    for (ssize_t idx = 0; idx < n_devs; idx++) {
        int intf_fd = open_adf_display(dev_ids[idx]);
        if (intf_fd >= 0)
            intf_fds.push_back(intf_fd);
    }

    if (displays.empty()) {
        ALOGE("failed to open any physical displays");
        ret = -EINVAL;
        goto err_open;
    }

    ret = adf_hwc_open(intf_fds.data(), intf_fds.size(),
            &hwc2_adfhwc_callbacks, this, &adf_helper);
    if (ret < 0) {
        ALOGE("failed to open adf helper: %s", strerror(ret));
        displays.clear();
    }

err_open:
    free(dev_ids);
    return ret;
}

int hwc2_dev::open_adf_display(adf_id_t adf_id) {
    struct adf_device adf_dev;

    int ret = adf_device_open(adf_id, O_RDWR, &adf_dev);
    if (ret < 0) {
        ALOGE("failed to open adf%u device: %s", adf_id, strerror(ret));
        return ret;
    }

    int intf_fd = adf_interface_open(&adf_dev, 0, O_RDWR);
    if (intf_fd < 0) {
        ALOGE("failed to open adf%u interface: %s", adf_id, strerror(intf_fd));
        adf_device_close(&adf_dev);
        return intf_fd;
    }

    struct adf_interface_data intf;
    ret = adf_get_interface_data(intf_fd, &intf);
    if (ret < 0) {
        ALOGE("failed to get adf%u interface data: %s", adf_id, strerror(ret));
        close(intf_fd);
        adf_device_close(&adf_dev);
        return ret;
    }

    hwc2_display_t dpy_id = hwc2_display::get_next_id();
    displays.emplace(std::piecewise_construct, std::forward_as_tuple(dpy_id),
            std::forward_as_tuple(dpy_id, intf_fd, adf_dev));

    adf_free_interface_data(&intf);

    return intf_fd;
}