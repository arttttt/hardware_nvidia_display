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

#include "fb.h"

class hwc2_display {
public:
    hwc2_display(hwc2_display_t id, int fb_intf_fd,
                const struct fb_device &fb_dev);
    ~hwc2_display();
    hwc2_display_t get_id() const { return id; }
    static hwc2_display_t get_next_id();
    static void reset_ids() { display_cnt = 0; }
private:
    hwc2_display_t id;
    int fb_intf_fd;
    struct fb_device fb_dev;
    static uint64_t display_cnt;
};

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();
    int open_fb_device();
private:
    std::unordered_map<hwc2_display_t, hwc2_display> displays;

    int open_fb_display(int fb_id);
};

struct hwc2_context {
    hwc2_device_t hwc2_device;
    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
