/****************************************************************************
 *  Copyright (C) 2020 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#ifndef __PID_H__
#define __PID_H__

#include "stdint.h"
#include <math.h>
#if defined(__CC_ARM)
    typedef struct pid *pid_t;
#endif

#define PID_MAX_OUT 30000
#define INTERGRAL_LIMIT 5000
#define CHASSIS_KP 1.0
#define CHASSIS_KI 0.0
#define CHASSIS_KD 0.0
#define YAW_KP 80.0
#define YAW_KI -3.0
#define YAW_KD 0.0
#define PIT_KP 50.0
#define PIT_KI 0.0
#define PIT_KD 0.0
#define POKE_KP 0.0
#define POKE_KI 0.0
#define POKE_KD 0.0

struct pid_param
{
    float p;
    float i;
    float d;
    float input_max_err;

    float max_out;
    float integral_limit;
};

struct pid
{
    struct pid_param param;

    uint8_t enable;

    float set;
    float get;

    float err;
    float last_err;

    float pout;
    float iout;
    float dout;
    float out;

    void (*f_param_init)(struct pid *pid,
                         float max_output,
                         float integral_limit,
                         float p,
                         float i,
                         float d);
    void (*f_pid_reset)(struct pid *pid, float p, float i, float d);
};

void pid_struct_init(
    struct pid *pid,
    float maxout,
    float intergral_limit,

    float kp,
    float ki,
    float kd);

float pid_calculate(struct pid *pid, float fdb, float ref);

#endif // __PID_H__
