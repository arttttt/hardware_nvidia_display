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

struct fb_device {
    int fd;
};

class hwc2_dev {
public:
    hwc2_dev();
    ~hwc2_dev();
    int open_fb_device();
    
private:
};

struct hwc2_context {
    hwc2_device_t hwc2_device;
    hwc2_dev *hwc2_dev;
};

#endif /* ifndef _HWC2_H */
