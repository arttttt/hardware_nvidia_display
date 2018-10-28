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

#include <linux/fb.h>

#include "include/tegra_dc_ext.h"

struct nvfb_hotplug_status {
    union {
        struct {
            unsigned type        : 2;  // unique NVFB display type
            unsigned connected   : 1;  // HDMI device connected bit
            unsigned stereo      : 1;  // HDMI device stereo capable bit
            unsigned unused      : 4;  // spare bits
            unsigned transform   : 8;  // forced rotation
            unsigned generation  : 16; // hotplug generation counter
        };
        uint32_t value;
    };
};

struct nvfb_callbacks {
    int (*hotplug)(void *data, int disp, struct nvfb_hotplug_status hotplug);
    int (*acquire)(void *data, int disp);
    int (*release)(void *data, int disp);
    int (*bandwidth_change)(void *data);
};

struct nvfb_device {
	void* data;
    int id;
    int fd;
    int ctrl_fd;
    fb_fix_screeninfo fi;
	fb_var_screeninfo vi;

    struct nvfb_callbacks callbacks;
};

int nvfb_device_open(struct nvfb_device **dev, struct nvfb_callbacks *callbacks);
void nvfb_blank(struct nvfb_device *dev, bool blank);
void nvfb_write(struct nvfb_device *dev, void* new_data);
