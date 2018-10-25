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

#include <array>
#include <cutils/log.h>
#include <inttypes.h>
#include <unistd.h>
#include <vector>

#include "hwc2.h"

uint64_t hwc2_display::display_cnt = 0;

hwc2_display::hwc2_display(hwc2_display_t id,
            const struct nvfb_device &fb_dev,
            hwc2_connection_t connection,
            hwc2_power_mode_t power_mode,
            hwc2_display_type_t type)
    : active_config(0),
      configs(),
      connection(connection),
      id(id),
      fb_dev(fb_dev),
      layers(),
      power_mode(power_mode),
      type(type),
      vsync_enabled(HWC2_VSYNC_DISABLE) { }

hwc2_display::~hwc2_display()
{
    close(fb_dev.fd);
}

hwc2_error_t hwc2_display::set_power_mode(hwc2_power_mode_t mode)
{
    bool blank;

    switch (mode) {
    case HWC2_POWER_MODE_ON:
        blank = false;
        break;
    case HWC2_POWER_MODE_OFF:
        blank = true;
        break;
    case HWC2_POWER_MODE_DOZE:
    case HWC2_POWER_MODE_DOZE_SUSPEND:
        ALOGE("dpy %" PRIu64 ": unsupported power mode: %u", id, mode);
        return HWC2_ERROR_UNSUPPORTED;
    default:
        ALOGE("dpy %" PRIu64 ": invalid power mode: %u", id, mode);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    nvfb_blank(&fb_dev, blank);
    power_mode = mode;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_doze_support(int32_t *out_support) const
{
    *out_support = 0;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::set_vsync_enabled(hwc2_vsync_t enabled)
{
    if (enabled == HWC2_VSYNC_INVALID) {
        ALOGE("dpy %" PRIu64 ": invalid vsync enabled", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    this->vsync_enabled = enabled;

    bool blank;
    switch (enabled) {
    case HWC2_VSYNC_ENABLE:
        blank = false;
        break;
    case HWC2_VSYNC_DISABLE:
        blank = true;
        break;
    default:
        ALOGW("dpy %" PRIu64 ": invalid vsync enable parameter %u", id,
                enabled);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    nvfb_blank(&fb_dev, blank);

    return HWC2_ERROR_NONE;
}

int hwc2_display::retrieve_display_configs()
{
    hwc2_config config;
    config.set_attribute(HWC2_ATTRIBUTE_WIDTH, fb_dev.vi.xres);
    config.set_attribute(HWC2_ATTRIBUTE_HEIGHT, fb_dev.vi.yres);
    config.set_attribute(HWC2_ATTRIBUTE_VSYNC_PERIOD, fb_dev.vi.vsync_len);
    config.set_attribute(HWC2_ATTRIBUTE_DPI_X, 324);
    config.set_attribute(HWC2_ATTRIBUTE_DPI_Y, 324);

    configs.emplace(0, hwc2_config());

    return 0;
}

hwc2_error_t hwc2_display::set_connection(hwc2_connection_t connection)
{
    if (connection == HWC2_CONNECTION_INVALID) {
        ALOGE("dpy %" PRIu64 ": invalid connection", id);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    this->connection = connection;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_display_attribute(hwc2_config_t config,
        hwc2_attribute_t attribute, int32_t *out_value) const
{
    auto it = configs.find(config);
    if (it == configs.end()) {
        ALOGE("dpy %" PRIu64 ": bad config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    *out_value = it->second.get_attribute(attribute);
    return HWC2_ERROR_NONE;
}
 hwc2_error_t hwc2_display::get_display_configs(uint32_t *out_num_configs,
        hwc2_config_t *out_configs) const
{
    if (!out_configs) {
        *out_num_configs = configs.size();
        return HWC2_ERROR_NONE;
    }

    size_t idx = 0;
    for (auto it = configs.begin(); it != configs.end()
            && idx < *out_num_configs; it++, idx++)
        out_configs[idx] = it->first;

    *out_num_configs = idx;
    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::get_active_config(hwc2_config_t *out_config) const
{
    if (!configs.size()) {
        ALOGE("dpy %" PRIu64 ": no active config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    *out_config = active_config;
    return HWC2_ERROR_NONE;
}

/*
 * It's just a stub. Will be implemented later
 */
hwc2_error_t hwc2_display::set_active_config(hwc2_config_t config)
{
    if (config >= configs.size()) {
        ALOGE("dpy %" PRIu64 ": bad config", id);
        return HWC2_ERROR_BAD_CONFIG;
    }

    active_config = config;

    return HWC2_ERROR_NONE;
}

hwc2_error_t hwc2_display::create_layer(hwc2_layer_t *out_layer)
{
    hwc2_layer_t lyr_id = hwc2_layer::get_next_id();
    layers.emplace(std::piecewise_construct, std::forward_as_tuple(lyr_id),
            std::forward_as_tuple(lyr_id));

    *out_layer = lyr_id;
    return HWC2_ERROR_NONE;
}

 hwc2_error_t hwc2_display::destroy_layer(hwc2_layer_t lyr_id)
{
    auto it = layers.find(lyr_id);
    if (it == layers.end()) {
        ALOGE("dpy %" PRIu64 ": lyr %" PRIu64 ": bad layer handle", id, lyr_id);
        return HWC2_ERROR_BAD_LAYER;
    }

    layers.erase(lyr_id);
    return HWC2_ERROR_NONE;
}

hwc2_display_t hwc2_display::get_next_id()
{
    return display_cnt++;
}
