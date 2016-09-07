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
#include <inttypes.h>
#include <cstdlib>
#include <vector>

#include "hwc2.h"

static void hwc2_vsync(void *data, int dpy_id, uint64_t timestamp)
{
    hwc2_dev *dev = static_cast<hwc2_dev *>(data);
    dev->vsync(static_cast<hwc2_display_t>(dpy_id), timestamp);
}

static void hwc2_hotplug(void *data, int dpy_id, bool connected)
{
    hwc2_dev *dev = static_cast<hwc2_dev *>(data);
    dev->hotplug(static_cast<hwc2_display_t>(dpy_id),
            (connected)? HWC2_CONNECTION_CONNECTED:
            HWC2_CONNECTION_DISCONNECTED);
}

hwc2_dev::hwc2_dev()
    : callback_handler(),
      displays() { }

hwc2_dev::~hwc2_dev()
{
    hwc2_display::reset_ids();
}

hwc2_error_t hwc2_dev::get_display_name(hwc2_display_t dpy_id, uint32_t *out_size,
        char *out_name) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_name(out_size, out_name);
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

uint32_t hwc2_dev::get_max_virtual_display_count() const
{
    return 0;
}

hwc2_error_t hwc2_dev::create_virtual_display(uint32_t /*width*/,
        uint32_t /*height*/, android_pixel_format_t* /*format*/,
        hwc2_display_t* /*out_display*/)
{
    return HWC2_ERROR_NO_RESOURCES;
}

hwc2_error_t hwc2_dev::destroy_virtual_display(hwc2_display_t dpy_id)
{
    if (displays.find(dpy_id) != displays.end())
        return HWC2_ERROR_BAD_PARAMETER;
    else
        return HWC2_ERROR_BAD_DISPLAY;
}

hwc2_error_t hwc2_dev::set_output_buffer(hwc2_display_t dpy_id,
        buffer_handle_t /*buffer*/, int32_t /*release_fence*/)
{
    if (displays.find(dpy_id) != displays.end())
        return HWC2_ERROR_UNSUPPORTED;
    else
        return HWC2_ERROR_BAD_DISPLAY;
}

hwc2_error_t hwc2_dev::set_power_mode(hwc2_display_t dpy_id,
        hwc2_power_mode_t mode)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_power_mode(mode);
}

hwc2_error_t hwc2_dev::get_doze_support(hwc2_display_t dpy_id,
        int32_t *out_support) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_doze_support(out_support);
}

hwc2_error_t hwc2_dev::validate_display(hwc2_display_t dpy_id,
        uint32_t *out_num_types, uint32_t *out_num_requests)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.validate_display(out_num_types, out_num_requests);
}

hwc2_error_t hwc2_dev::get_changed_composition_types(hwc2_display_t dpy_id,
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        hwc2_composition_t *out_types) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_changed_composition_types(out_num_elements,
            out_layers, out_types);
}

hwc2_error_t hwc2_dev::get_display_requests(hwc2_display_t dpy_id,
        hwc2_display_request_t *out_display_requests,
        uint32_t *out_num_elements, hwc2_layer_t *out_layers,
        hwc2_layer_request_t *out_layer_requests) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_requests(out_display_requests,
            out_num_elements, out_layers, out_layer_requests);
}

hwc2_error_t hwc2_dev::accept_display_changes(hwc2_display_t dpy_id)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.accept_display_changes();
}

hwc2_error_t hwc2_dev::get_display_attribute(hwc2_display_t dpy_id,
        hwc2_config_t config, hwc2_attribute_t attribute, int32_t *out_value)
        const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_attribute(config, attribute, out_value);
}

hwc2_error_t hwc2_dev::get_display_configs(hwc2_display_t dpy_id,
        uint32_t *out_num_configs, hwc2_config_t *out_configs) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_display_configs(out_num_configs, out_configs);
}

hwc2_error_t hwc2_dev::get_active_config(hwc2_display_t dpy_id,
        hwc2_config_t *out_config) const
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_active_config(out_config);
}

hwc2_error_t hwc2_dev::set_active_config(hwc2_display_t dpy_id,
        hwc2_config_t config)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_active_config(config);
}

hwc2_error_t hwc2_dev::get_client_target_support(hwc2_display_t dpy_id,
        uint32_t width, uint32_t height, android_pixel_format_t format,
        android_dataspace_t dataspace)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.get_client_target_support(width, height, format,
            dataspace);
}

hwc2_error_t hwc2_dev::set_client_target(hwc2_display_t dpy_id,
        buffer_handle_t target, int32_t acquire_fence,
        android_dataspace_t dataspace, const hwc_region_t &surface_damage)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.set_client_target(target, acquire_fence, dataspace,
            surface_damage);
}

hwc2_error_t hwc2_dev::create_layer(hwc2_display_t dpy_id, hwc2_layer_t *out_layer)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.create_layer(out_layer);
}

hwc2_error_t hwc2_dev::destroy_layer(hwc2_display_t dpy_id, hwc2_layer_t lyr_id)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return it->second.destroy_layer(lyr_id);
}

hwc2_error_t hwc2_dev::set_layer_composition_type(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, hwc2_composition_t comp_type)
{
    return displays.find(dpy_id)->second.set_layer_composition_type(lyr_id,
            comp_type);
}

hwc2_error_t hwc2_dev::set_layer_buffer(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, buffer_handle_t handle, int32_t acquire_fence)
{
    return displays.find(dpy_id)->second.set_layer_buffer(lyr_id, handle,
            acquire_fence);
}

hwc2_error_t hwc2_dev::set_layer_dataspace(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, android_dataspace_t dataspace)
{
    return displays.find(dpy_id)->second.set_layer_dataspace(lyr_id, dataspace);
}

hwc2_error_t hwc2_dev::set_layer_display_frame(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_rect_t &display_frame)
{
    return displays.find(dpy_id)->second.set_layer_display_frame(lyr_id,
            display_frame);
}

hwc2_error_t hwc2_dev::set_layer_source_crop(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_frect_t &source_crop)
{
    return displays.find(dpy_id)->second.set_layer_source_crop(lyr_id,
            source_crop);
}

hwc2_error_t hwc2_dev::set_layer_z_order(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, uint32_t z_order)
{
    return displays.find(dpy_id)->second.set_layer_z_order(lyr_id, z_order);
}

hwc2_error_t hwc2_dev::set_layer_surface_damage(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_region_t &surface_damage)
{
    return displays.find(dpy_id)->second.set_layer_surface_damage(lyr_id,
            surface_damage);
}

hwc2_error_t hwc2_dev::set_layer_blend_mode(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, hwc2_blend_mode_t blend_mode)
{
    return displays.find(dpy_id)->second.set_layer_blend_mode(lyr_id,
            blend_mode);
}

hwc2_error_t hwc2_dev::set_layer_plane_alpha(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, float plane_alpha)
{
    return displays.find(dpy_id)->second.set_layer_plane_alpha(lyr_id,
            plane_alpha);
}

hwc2_error_t hwc2_dev::set_layer_transform(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_transform_t transform)
{
    return displays.find(dpy_id)->second.set_layer_transform(lyr_id, transform);
}

hwc2_error_t hwc2_dev::set_layer_visible_region(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_region_t &visible_region)
{
    return displays.find(dpy_id)->second.set_layer_visible_region(lyr_id,
            visible_region);
}

hwc2_error_t hwc2_dev::set_layer_color(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, const hwc_color_t &color)
{
    return displays.find(dpy_id)->second.set_layer_color(lyr_id, color);
}

hwc2_error_t hwc2_dev::set_cursor_position(hwc2_display_t dpy_id,
        hwc2_layer_t lyr_id, int32_t x, int32_t y)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    return displays.find(dpy_id)->second.set_cursor_position(lyr_id, x, y);
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

void hwc2_dev::vsync(hwc2_display_t dpy_id, uint64_t timestamp)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGW("dpy %" PRIu64 ": invalid display handle preventing vsync"
                " callback", dpy_id);
        return;
    }

    callback_handler.call_vsync(dpy_id, timestamp);
}

hwc2_error_t hwc2_dev::set_vsync_enabled(hwc2_display_t dpy_id,
        hwc2_vsync_t enabled)
{
    auto it = displays.find(dpy_id);
    if (it == displays.end()) {
        ALOGE("dpy %" PRIu64 ": invalid display handle", dpy_id);
        return HWC2_ERROR_BAD_DISPLAY;
    }

    if (it->second.get_type() != HWC2_DISPLAY_TYPE_PHYSICAL)
        return HWC2_ERROR_NONE;

    if (it->second.get_vsync_enabled() == enabled)
        return HWC2_ERROR_NONE;

    return it->second.set_vsync_enabled(enabled);
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

    displays.emplace(std::piecewise_construct, std::forward_as_tuple(fb_id),
            std::forward_as_tuple(fb_id,
                                    nvfb_dev,
                                    HWC2_CONNECTION_CONNECTED,
                                    HWC2_DISPLAY_TYPE_PHYSICAL,
                                    HWC2_POWER_MODE_ON));

    return 0;
}
