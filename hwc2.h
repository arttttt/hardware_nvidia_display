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

#ifndef _HWC2_H
#define _HWC2_H

#include <hardware/hwcomposer2.h>

#include <unordered_map>
#include <queue>
#include <mutex>

#include "nvfb.h"

class hwc2_callback {
public:
    hwc2_callback();
    hwc2_error_t register_callback(hwc2_callback_descriptor_t descriptor,
            hwc2_callback_data_t callback_data,
            hwc2_function_pointer_t pointer);

    void call_hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection);
 private:
    std::mutex state_mutex;
    std::queue<std::pair<hwc2_display_t, hwc2_connection_t>> hotplug_pending;

    hwc2_callback_data_t hotplug_data;
    HWC2_PFN_HOTPLUG hotplug;
    hwc2_callback_data_t refresh_data;
    HWC2_PFN_REFRESH refresh;
    hwc2_callback_data_t vsync_data;
    HWC2_PFN_VSYNC vsync;
};

class hwc2_config {
public:
    hwc2_config();

    int set_attribute(hwc2_attribute_t attribute, int32_t value);

private:
    int32_t width;
    int32_t height;

    int32_t vsync_period;

    int32_t dpi_x;
    int32_t dpi_y;
};

class hwc2_display {
public:
    hwc2_display(hwc2_display_t id, 
                    const struct nvfb_device &fb_dev,
                    hwc2_connection_t connection);
    ~hwc2_display();
    hwc2_display_t get_id() const { return id; }
    hwc2_connection_t get_connection() const { return connection; }
    int retrieve_display_configs();
    hwc2_error_t set_connection(hwc2_connection_t connection);
    static hwc2_display_t get_next_id();
    static void reset_ids() { display_cnt = 0; }
private:
    hwc2_config_t active_config;
    std::unordered_map<hwc2_config_t, hwc2_config> configs;
    hwc2_connection_t connection;
    hwc2_display_t id;
    struct nvfb_device fb_dev;
    static uint64_t display_cnt;
};

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();

    int open_fb_device();

    void hotplug(hwc2_display_t dpy_id, hwc2_connection_t connection);
    hwc2_error_t register_callback(hwc2_callback_descriptor_t descriptor,
                    hwc2_callback_data_t callback_data,
                    hwc2_function_pointer_t pointer);
private:
    hwc2_callback callback_handler;
    std::unordered_map<hwc2_display_t, hwc2_display> displays;

    int open_fb_display(int fb_id);
};

struct hwc2_context {
    hwc2_device_t hwc2_device;
    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
