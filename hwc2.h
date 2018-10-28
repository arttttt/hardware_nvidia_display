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

#include <unordered_map>

#include "nvfb.h"
#include "hwc2_display.h"

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();

    int open_fb_device();
private:
    /* The physical and virtual displays associated with this device */
    std::unordered_map<hwc2_display_t, hwc2_display> displays;

    int open_fb_display(int dpy);

    int get_displays(struct nvfb_device *dev);
	int open_display(struct nvfb_device *dev, hwc2_display *dpy);
};

struct hwc2_context {
    hwc2_device_t hwc2_device; /* must be first member in struct */

    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
