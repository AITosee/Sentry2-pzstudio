/*
 * Copyright (C) 2022 Polygon Zone Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */



#ifndef  EXTDEV_ULTRASONIC_H
#define  EXTDEV_ULTRASONIC_H



#include "hi_io.h"
#include "hi_gpio.h"



typedef struct _extdev_ultra_obj_t {
    mp_obj_base_t base;
	gpio_func  trig_pin;
	gpio_func  echo_pin;
	uint64_t   time_frist;
	uint64_t   time_last;
	int8_t     finsh_flag;
} extdev_ultra_obj_t;









#endif


