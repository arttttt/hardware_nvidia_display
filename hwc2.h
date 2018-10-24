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

#ifndef _HWC2_H
#define _HWC2_H

#include <hardware/hwcomposer2.h>

/*class hwc2_display {
public:
    hwc2_display(hwc2_display_t id, int fb_intf_fd);
    ~hwc2_display();
    hwc2_display_t get_id() const { return id; }
    static hwc2_display_t get_next_id();
    static void reset_ids() { display_cnt = 0; }
private:
    hwc2_display_t id;
    int fb_intf_fd;
    static uint64_t display_cnt;
};*/

struct fb_device {
    int fd;
};

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();
    int open_fb_device();
    
private:
    int open_fb_display(struct fb_device *dev);
};

struct hwc2_context {
    hwc2_device_t hwc2_device;
    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
