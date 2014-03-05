/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include "msm_sensor.h"
#define SENSOR_NAME "s5k4e1"
#define PLATFORM_DRIVER_NAME "msm_camera_s5k4e1"
#define s5k4e1_obj s5k4e1_##obj
#define MSB                             1
#define LSB                             0

#define S5K4E1_REG_READ_MODE 0x0101
#define S5K4E1_READ_NORMAL_MODE 0x0000	
#define S5K4E1_READ_MIRROR 0x0001			
#define S5K4E1_READ_FLIP 0x0002			
#define S5K4E1_READ_MIRROR_FLIP 0x0003	


DEFINE_MUTEX(s5k4e1_mut);
static struct msm_sensor_ctrl_t s5k4e1_s_ctrl;

static struct msm_camera_i2c_reg_conf s5k4e1_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4e1_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k4e1_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf s5k4e1_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf s5k4e1_prev_settings[] = {
	
	{0x30A9, 0x02},
	{0x300E, 0xEB},
	{0x0387, 0x03},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0348, 0x0A},
	{0x0349, 0x2F},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x034A, 0x07},
	{0x034B, 0xA7},
	{0x0380, 0x00},
	{0x0381, 0x01},
	{0x0382, 0x00},
	{0x0383, 0x01},
	{0x0384, 0x00},
	{0x0385, 0x01},
	{0x0386, 0x00},
	{0x0387, 0x03},
	{0x034C, 0x05},
	{0x034D, 0x18},
	{0x034E, 0x03},
	{0x034F, 0xd4},
	{0x30BF, 0xAB},
	{0x30C0, 0xA0},
	{0x30C8, 0x06},
	{0x30C9, 0x5E},
	
	{0x0202, 0x01},
	{0x0203, 0xFD},
	{0x0204, 0x00},
	{0x0205, 0x80},
	{0x0340, 0x04},
	{0x0341, 0xC1},
	{0x0342, 0x0D},
	{0x0343, 0x18},
};

static struct msm_camera_i2c_reg_conf s5k4e1_video_settings[] = {
	
	{0x0202, 0x01},
	{0x0203, 0xFD},
	{0x0204, 0x00},
	{0x0205, 0x80},
	
	{0x0340, 0x04},
	{0x0341, 0xC1},
	
	{0x0342, 0x0D}, 
	{0x0343, 0x18},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x02},  
	{0x300E, 0xEB},  
	{0x0387, 0x03},  

	{0x0344, 0x00}, 
	{0x0345, 0x00},  
	{0x0348, 0x0A}, 
	{0x0349, 0x2F}, 
	{0x0346, 0x00}, 
	{0x0347, 0x00},
	{0x034A, 0x07}, 
	{0x034B, 0xA7},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x03},
	{0x034C, 0x05}, 
	{0x034D, 0x18},
	{0x034E, 0x03}, 
	{0x034F, 0xD4},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xA0}, 
	{0x30C8, 0x06}, 
	{0x30C9, 0x5E},

	{0x301B, 0x83},  
	{0x3017, 0x84}, 

};


static struct msm_camera_i2c_reg_conf s5k4e1_fast_video_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x07}, 
	{0x034F, 0x98},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};

static struct msm_camera_i2c_reg_conf s5k4e1_4_3_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x00},  
	{0x0348, 0x0A}, 
	{0x0349, 0x2F}, 
	{0x0346, 0x00}, 
	{0x0347, 0x00},
	{0x034A, 0x07}, 
	{0x034B, 0xA7},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x30},
	{0x034E, 0x07}, 
	{0x034F, 0xA8},

	{0x30BF, 0xAB}, 
	{0x30C0, 0x80}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xBC},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};

static struct msm_camera_i2c_reg_conf s5k4e1_16_9_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x05},
	{0x0341, 0xC0},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0xFC},
	{0x034A, 0x06}, 
	{0x034B, 0xAB},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x05}, 
	{0x034F, 0xB0},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 
};

static struct msm_camera_i2c_reg_conf s5k4e1_5_3_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x06},	
	{0x0341, 0x20},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0xCC},
	{0x034A, 0x06}, 
	{0x034B, 0xDB},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x06}, 
	{0x034F, 0x10},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};

static struct msm_camera_i2c_reg_conf s5k4e1_video_hfr_5_3_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x07}, 
	{0x034F, 0x98},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};

static struct msm_camera_i2c_reg_conf s5k4e1_video_24fps_16_9_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x07}, 
	{0x034F, 0x98},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};
static struct msm_camera_i2c_reg_conf s5k4e1_video_60fps_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x07}, 
	{0x034F, 0x98},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 
};
static struct msm_camera_i2c_reg_conf s5k4e1_video_24fps_5_3_settings[] = {
	
	{0x0202, 0x04},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x07},
	{0x0341, 0xef},
	
	{0x0342, 0x0A}, 
	{0x0343, 0xF0},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x01},  
	{0x300E, 0xE9},  
	{0x0387, 0x01},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x01},
	{0x034C, 0x0A}, 
	{0x034D, 0x20},
	{0x034E, 0x07}, 
	{0x034F, 0x98},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x0C}, 
	{0x30C9, 0xA8},

	{0x301B, 0x77},  
	{0x3017, 0x94}, 

};
static struct msm_camera_i2c_reg_conf s5k4e1_video_16_9_settings[] = {
	
	{0x0202, 0x02},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x04},
	{0x0341, 0xC1},
	
	{0x0342, 0x0D}, 
	{0x0343, 0x18},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x02},  
	{0x300E, 0xEB},  
	{0x0387, 0x03},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0xFC},
	{0x034A, 0x06}, 
	{0x034B, 0xAB},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x03},
	{0x034C, 0x05}, 
	{0x034D, 0x10},
	{0x034E, 0x02}, 
	{0x034F, 0xDB},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x06}, 
	{0x30C9, 0x54},

	{0x301B, 0x77}, 
	{0x3017, 0x94}, 

};
static struct msm_camera_i2c_reg_conf s5k4e1_video_5_3_settings[] = {
	
	{0x0202, 0x02},
	{0x0203, 0x12},
	{0x0204, 0x00},
	{0x0205, 0x20},
	
	{0x0340, 0x04},
	{0x0341, 0xC1},
	
	{0x0342, 0x0D}, 
	{0x0343, 0x18},

	
	
	
	
	{0x0305, 0x04},
	{0x0306, 0x00},
	{0x0307, 0x66},
	{0x30B5, 0x01},
	{0x30E2, 0x02}, 
	{0x30F1, 0xA0},

	{0x30e8, 0x07},  

	
	
	{0x30A9, 0x02},  
	{0x300E, 0xEB},  
	{0x0387, 0x03},  

	{0x0344, 0x00}, 
	{0x0345, 0x08},  
	{0x0348, 0x0A}, 
	{0x0349, 0x27}, 
	{0x0346, 0x00}, 
	{0x0347, 0x08},
	{0x034A, 0x07}, 
	{0x034B, 0x9F},
	{0x0380, 0x00}, 
	{0x0381, 0x01},
	{0x0382, 0x00}, 
	{0x0383, 0x01},
	{0x0384, 0x00}, 
	{0x0385, 0x01},
	{0x0386, 0x00}, 
	{0x0387, 0x03},
	{0x034C, 0x05}, 
	{0x034D, 0x10},
	{0x034E, 0x03}, 
	{0x034F, 0xCC},

	{0x30BF, 0xAB}, 
	{0x30C0, 0xC0}, 
	{0x30C8, 0x06}, 
	{0x30C9, 0x54},

	{0x301B, 0x83}, 
	{0x3017, 0x84}, 
};



static struct msm_camera_i2c_reg_conf s5k4e1_recommend_settings[] = {
	
	
	
	{0x3030, 0x06},
	{0x3000, 0x05},
	{0x3001, 0x03},
	{0x3002, 0x48},  
	{0x3003, 0x4A},  
	{0x3004, 0x50},  
	{0x3005, 0x0E},  
	{0x3006, 0x5E},  
	{0x3007, 0x00},
	{0x3008, 0x78},  
	{0x3009, 0x78},  
	{0x300A, 0x50},  
	{0x300B, 0x08},  
	{0x300C, 0x14},  
	{0x300D, 0x00},  
	{0x300F, 0x40},  
	{0x3010, 0x00},
	{0x3011, 0x3A}, 
	{0x3012, 0x30},
	{0x3013, 0xA0},  
	{0x3014, 0x00},
	{0x3015, 0x00},
	{0x3016, 0x22},  
	{0x3018, 0x70},  
	{0x301C, 0x06},  
	{0x301D, 0xD4},
	{0x3021, 0x02},
	{0x3022, 0x24},
	{0x3024, 0x40},
	{0x3027, 0x08},
	{0x3029, 0xC4},  
	{0x30BC, 0xA8},  
	{0x302B, 0x01},
	{0x30D8, 0x3F},
	
	
	{0x3070, 0x5F},
	{0x3071, 0x00},
	{0x3080, 0x04},
	{0x3081, 0x38},

	{0x302E, 0x0B},  
	
	
	{0x30BD, 0x00}, 
	{0x3084, 0x15}, 
	{0x30BE, 0x1A}, 
	{0x30C1, 0x01}, 
	{0x3111, 0x86}, 

	
	{0x30E3, 0x38},  
	{0x30E4, 0x40},  
	{0x3113, 0x70},  
	{0x3114, 0x80},  
	{0x3115, 0x7B},  
	{0x3116, 0xC0},  
	{0x30EE, 0x12},  

};

static struct v4l2_subdev_info s5k4e1_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	
};

static struct msm_camera_i2c_conf_array s5k4e1_init_conf[] = {
	{&s5k4e1_recommend_settings[0],
	ARRAY_SIZE(s5k4e1_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA}
};

static struct msm_camera_i2c_conf_array s5k4e1_confs[] = {
	{&s5k4e1_4_3_settings[0],
	ARRAY_SIZE(s5k4e1_4_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_prev_settings[0],
	ARRAY_SIZE(s5k4e1_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_settings[0],
	ARRAY_SIZE(s5k4e1_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_fast_video_settings[0],
	ARRAY_SIZE(s5k4e1_fast_video_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_16_9_settings[0],
	ARRAY_SIZE(s5k4e1_16_9_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_4_3_settings[0],
	ARRAY_SIZE(s5k4e1_4_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_hfr_5_3_settings[0],
	ARRAY_SIZE(s5k4e1_video_hfr_5_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_5_3_settings[0],
	ARRAY_SIZE(s5k4e1_5_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_24fps_16_9_settings[0],
	ARRAY_SIZE(s5k4e1_video_24fps_16_9_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_60fps_settings[0],
	ARRAY_SIZE(s5k4e1_video_60fps_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_24fps_5_3_settings[0],
	ARRAY_SIZE(s5k4e1_video_24fps_5_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_16_9_settings[0],
	ARRAY_SIZE(s5k4e1_video_16_9_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&s5k4e1_video_5_3_settings[0],
	ARRAY_SIZE(s5k4e1_video_5_3_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t s5k4e1_dimensions[] = {
	{	
		.x_output = 0xA30,
		.y_output = 0x7A8,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0x518,	
		.y_output = 0x3D4,	
		.line_length_pclk = 0xD18,	
		.frame_length_lines = 0x4C1,	
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 1,
	},
	{	
		.x_output = 0x518,	
		.y_output = 0x3D4,	
		.line_length_pclk = 0xD18,	
		.frame_length_lines = 0x4C1,	
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 1,
	},
	{	
		.x_output = 0xA20,
		.y_output = 0x798,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,	
		.y_output = 0x5B0,	
		.line_length_pclk = 0xAF0,		
		.frame_length_lines = 0x5C0,	
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,	
		.y_output = 0x798,	
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,
		.y_output = 0x798,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,	
		.y_output = 0x610,	
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x620,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,
		.y_output = 0x798,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,
		.y_output = 0x798,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0xA20,
		.y_output = 0x798,
		.line_length_pclk = 0xAF0,
		.frame_length_lines = 0x7EF,
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 0,
	},
	{	
		.x_output = 0x510,	
		.y_output = 0x2D8,	
		.line_length_pclk = 0xD18,	
		.frame_length_lines = 0x4C1,	
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 1,
	},
	{	
		.x_output = 0x510,	
		.y_output = 0x3CC,	
		.line_length_pclk = 0xD18,	
		.frame_length_lines = 0x4C1,	
		.vt_pixel_clk = 122400000,
		.op_pixel_clk = 122400000,
		.binning_factor = 1,
	},

};

static struct msm_camera_csi_params s5k4e1_csi_params = {
	.data_format = CSI_10BIT,
	.lane_cnt    = 2,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt  = 24,
};

static struct msm_camera_csi_params *s5k4e1_csi_params_array[] = {
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,
	&s5k4e1_csi_params,

};

static struct msm_sensor_output_reg_addr_t s5k4e1_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_id_info_t s5k4e1_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x4E10,
};

static struct msm_sensor_exp_gain_info_t s5k4e1_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0204,
	.vert_offset = 16,
	.min_vert = 4,  
	.sensor_max_linecount = 65519,
};

static int s5k4e1_read_fuseid(struct sensor_cfg_data *cdata,
	struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t  rc;
	unsigned short i, R1, R2, R3;
	unsigned short  OTP[10] = {0};

	struct msm_camera_i2c_client *s5k4e1_msm_camera_i2c_client = s_ctrl->sensor_i2c_client;

	pr_info("[CAM]%s: sensor OTP information:\n", __func__);

	rc = msm_camera_i2c_write_b(s5k4e1_msm_camera_i2c_client, 0x30F9, 0x0E);
	if (rc < 0)
		pr_info("[CAM]%s: i2c_write_b 0x30F9 fail\n", __func__);

	rc = msm_camera_i2c_write_b(s5k4e1_msm_camera_i2c_client, 0x30FA, 0x0A);
	if (rc < 0)
		pr_info("[CAM]%s: i2c_write_b 0x30FA fail\n", __func__);

	rc = msm_camera_i2c_write_b(s5k4e1_msm_camera_i2c_client, 0x30FB, 0x71);
	if (rc < 0)
		pr_info("[CAM]%s: i2c_write_b 0x30FB fail\n", __func__);

	rc = msm_camera_i2c_write_b(s5k4e1_msm_camera_i2c_client, 0x30FB, 0x70);
	if (rc < 0)
		pr_info("[CAM]%s: i2c_write_b 0x30FB fail\n", __func__);

	mdelay(4);

	for (i = 0; i < 10; i++) {
		rc = msm_camera_i2c_write_b(s5k4e1_msm_camera_i2c_client, 0x310C, i);
		if (rc < 0)
			pr_info("[CAM]%s: i2c_write_b 0x310C fail\n", __func__);
		rc = msm_camera_i2c_read_b(s5k4e1_msm_camera_i2c_client, 0x310F, &R1);
		if (rc < 0)
			pr_info("[CAM]%s: i2c_read_b 0x310F fail\n", __func__);
		rc = msm_camera_i2c_read_b(s5k4e1_msm_camera_i2c_client, 0x310E, &R2);
		if (rc < 0)
			pr_info("[CAM]%s: i2c_read_b 0x310E fail\n", __func__);
		rc = msm_camera_i2c_read_b(s5k4e1_msm_camera_i2c_client, 0x310D, &R3);
			if (rc < 0)
			pr_info("[CAM]%s: i2c_read_b 0x310D fail\n", __func__);

		if ((R3&0x0F) != 0)
			OTP[i] = (short)(R3&0x0F);
		else if ((R2&0x0F) != 0)
			OTP[i] = (short)(R2&0x0F);
		else if ((R2>>4) != 0)
			OTP[i] = (short)(R2>>4);
		else if ((R1&0x0F) != 0)
			OTP[i] = (short)(R1&0x0F);
		else
			OTP[i] = (short)(R1>>4);

	}
	pr_info("[CAM]%s: VenderID=%x,LensID=%x,SensorID=%x%x\n", __func__,
		OTP[0], OTP[1], OTP[2], OTP[3]);
	pr_info("[CAM]%s: ModuleFuseID= %x%x%x%x%x%x\n", __func__,
		OTP[4], OTP[5], OTP[6], OTP[7], OTP[8], OTP[9]);

	cdata->cfg.fuse.fuse_id_word1 = 0;
	cdata->cfg.fuse.fuse_id_word2 = 0;
	cdata->cfg.fuse.fuse_id_word3 = (OTP[0]);
	cdata->cfg.fuse.fuse_id_word4 =
		(OTP[4]<<20) |
		(OTP[5]<<16) |
		(OTP[6]<<12) |
		(OTP[7]<<8) |
		(OTP[8]<<4) |
		(OTP[9]);

	pr_info("[CAM]s5k4e1: fuse->fuse_id_word1:%d\n",
		cdata->cfg.fuse.fuse_id_word1);
	pr_info("[CAM]s5k4e1: fuse->fuse_id_word2:%d\n",
		cdata->cfg.fuse.fuse_id_word2);
	pr_info("[CAM]s5k4e1: fuse->fuse_id_word3:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word3);
	pr_info("[CAM]s5k4e1: fuse->fuse_id_word4:0x%08x\n",
		cdata->cfg.fuse.fuse_id_word4);
	return 0;
}

static int s5k4e1_sensor_open_init(const struct msm_camera_sensor_info *data)
{
	int rc = 0;
	uint16_t value = 0;

	pr_info("%s E \n", __func__);


	if (data->sensor_platform_info)
		s5k4e1_s_ctrl.mirror_flip = data->sensor_platform_info->mirror_flip;

	
	if (s5k4e1_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR_FLIP)
		value = S5K4E1_READ_MIRROR_FLIP;
	else if (s5k4e1_s_ctrl.mirror_flip == CAMERA_SENSOR_MIRROR)
		value = S5K4E1_READ_MIRROR;
	else if (s5k4e1_s_ctrl.mirror_flip == CAMERA_SENSOR_FLIP)
		value = S5K4E1_READ_FLIP;
	else
		value = S5K4E1_READ_NORMAL_MODE;
	msm_camera_i2c_write(s5k4e1_s_ctrl.sensor_i2c_client,
		S5K4E1_REG_READ_MODE, value, MSM_CAMERA_I2C_BYTE_DATA);

	pr_info("[CAM] %s X \n", __func__);

	return rc;
}


static inline uint8_t s5k4e1_byte(uint16_t word, uint8_t offset)
{
	return word >> (offset * BITS_PER_BYTE);
}

static const char *s5k4e1Vendor = "samsung";
static const char *s5k4e1NAME = "s5k4e1";
static const char *s5k4e1Size = "5M";

static ssize_t sensor_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	sprintf(buf, "%s %s %s\n", s5k4e1Vendor, s5k4e1NAME, s5k4e1Size);
	ret = strlen(buf) + 1;

	return ret;
}


static DEVICE_ATTR(sensor, 0444, sensor_vendor_show, NULL);

static struct kobject *android_s5k4e1;

static int s5k4e1_sysfs_init(void)
{
	int ret ;
	pr_info("s5k4e1:kobject creat and add\n");
	android_s5k4e1 = kobject_create_and_add("android_camera", NULL);
	if (android_s5k4e1 == NULL) {
		pr_info("s5k4e1_sysfs_init: subsystem_register " \
		"failed\n");
		ret = -ENOMEM;
		return ret ;
	}
	pr_info("s5k4e1:sysfs_create_file\n");
	ret = sysfs_create_file(android_s5k4e1, &dev_attr_sensor.attr);
	if (ret) {
		pr_info("s5k4e1_sysfs_init: sysfs_create_file " \
		"failed\n");
		kobject_del(android_s5k4e1);
	}

	return 0 ;
}


int32_t s5k4e1_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int	rc = 0;
	pr_info("[CAM] %s\n", __func__);
	rc = msm_sensor_i2c_probe(client, id);
	if(rc >= 0)
		s5k4e1_sysfs_init();
	pr_info("[CAM] %s: rc(%d)\n", __func__, rc);
	return rc;
}



static const struct i2c_device_id s5k4e1_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&s5k4e1_s_ctrl},
	{ }
};

static struct i2c_driver s5k4e1_i2c_driver = {
	.id_table = s5k4e1_i2c_id,
	.probe  = s5k4e1_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};


static struct msm_camera_i2c_client s5k4e1_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&s5k4e1_i2c_driver);
}

static struct v4l2_subdev_core_ops s5k4e1_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops s5k4e1_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops s5k4e1_subdev_ops = {
	.core = &s5k4e1_subdev_core_ops,
	.video  = &s5k4e1_subdev_video_ops,
};


int32_t s5k4e1_power_up(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc;
	struct msm_camera_sensor_info *sdata = NULL;
	pr_info("[CAM] %s\n", __func__);

	if (s_ctrl && s_ctrl->sensordata)
		sdata = s_ctrl->sensordata;
	else {
		pr_err("[CAM] %s: s_ctrl sensordata NULL\n", __func__);
		return -EINVAL;
	}

	if (sdata->camera_power_on == NULL) {
		pr_err("sensor platform_data didnt register\n");
		return -EIO;
	}

	if (!sdata->use_rawchip) {
		pr_info("msm_camio_clk_enable\n");
		rc = msm_camio_clk_enable(CAMIO_CAM_MCLK_CLK);
		if (rc < 0) {
			pr_err("[CAM] %s: msm_camio_sensor_clk_on failed:%d\n",
			 __func__, rc);
			goto enable_mclk_failed;
		}
	}

	rc = sdata->camera_power_on();
	if (rc < 0) {
		pr_err("[CAM] %s failed to enable power\n", __func__);
		goto enable_power_on_failed;
	}

	rc = msm_sensor_set_power_up(s_ctrl);
	if (rc < 0) {
		pr_err("[CAM] %s msm_sensor_power_up failed\n", __func__);
		goto enable_sensor_power_up_failed;
	}


	s5k4e1_sensor_open_init(sdata);
	pr_info("[CAM] %s end\n", __func__);

	return rc;

enable_sensor_power_up_failed:
	if (sdata->camera_power_off == NULL)
		pr_err("sensor platform_data didnt register\n");
	else
		sdata->camera_power_off();
enable_power_on_failed:
	if (!sdata->use_rawchip)
	msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
enable_mclk_failed:
	return rc;
}

int32_t s5k4e1_power_down(struct msm_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	struct msm_camera_sensor_info *sdata = NULL;
	pr_info("[CAM] %s\n", __func__);

	if (s_ctrl && s_ctrl->sensordata)
		sdata = s_ctrl->sensordata;
	else {
		pr_err("[CAM] %s: s_ctrl sensordata NULL\n", __func__);
		return -EINVAL;
	}

	rc = msm_sensor_set_power_down(s_ctrl);
	if (rc < 0) {
		pr_err("[CAM] %s msm_sensor_set_power_down failed\n", __func__);
	}

	if (sdata->camera_power_off == NULL) {
		pr_err("sensor platform_data didnt register\n");
		return -EIO;
	}

	rc = sdata->camera_power_off();
	if (rc < 0) {
		pr_err("[CAM] %s failed to disable power\n", __func__);
		return rc;
	}

	if (!sdata->use_rawchip) {
		msm_camio_clk_disable(CAMIO_CAM_MCLK_CLK);
		if (rc < 0)
			pr_err("[CAM] %s: msm_camio_sensor_clk_off failed:%d\n",
				 __func__, rc);
	}

	return rc;
}

int32_t s5k4e1_write_exp_gain1_ex(struct msm_sensor_ctrl_t *s_ctrl,
		int mode, uint16_t gain, uint16_t dig_gain, uint32_t line) 
{
	uint32_t fl_lines;
	uint8_t offset;

	uint32_t fps_divider = Q10;
	if (s_ctrl->mode == SENSOR_PREVIEW_MODE)
		fps_divider = s_ctrl->fps_divider;

	if(line > s_ctrl->sensor_exp_gain_info->sensor_max_linecount)
		line = s_ctrl->sensor_exp_gain_info->sensor_max_linecount;

	fl_lines = s_ctrl->curr_frame_length_lines;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line * Q10 > (fl_lines - offset) * fps_divider)
		fl_lines = line + offset;
	else
		fl_lines = (fl_lines * fps_divider) / Q10;

	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);

	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_WORD_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}

int32_t s5k4e1_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res) {

	int rc = 0;
	pr_info("[CAM] %s\n", __func__);

	rc = msm_sensor_setting1(s_ctrl, update_type, res);

	return rc;
}
static struct msm_sensor_fn_t s5k4e1_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
#if 0
	.sensor_write_exp_gain = s5k4e1_write_prev_exp_gain,
	.sensor_write_snapshot_exp_gain = s5k4e1_write_pict_exp_gain,
#else
	.sensor_write_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_write_exp_gain_ex = s5k4e1_write_exp_gain1_ex,
	.sensor_write_snapshot_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_write_snapshot_exp_gain_ex = s5k4e1_write_exp_gain1_ex,
#endif
#if 0
	.sensor_setting = msm_sensor_setting,
#else
	.sensor_csi_setting = s5k4e1_sensor_setting,
#endif
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_power_up = s5k4e1_power_up,
	.sensor_power_down = s5k4e1_power_down,
	.sensor_config = msm_sensor_config,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_i2c_read_fuseid = s5k4e1_read_fuseid,	
};

static struct msm_sensor_reg_t s5k4e1_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = s5k4e1_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(s5k4e1_start_settings),
	.stop_stream_conf = s5k4e1_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(s5k4e1_stop_settings),
	.group_hold_on_conf = s5k4e1_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(s5k4e1_groupon_settings),
	.group_hold_off_conf = s5k4e1_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(s5k4e1_groupoff_settings),
	.init_settings = &s5k4e1_init_conf[0],
	.init_size = ARRAY_SIZE(s5k4e1_init_conf),
	.mode_settings = &s5k4e1_confs[0],
	.output_settings = &s5k4e1_dimensions[0],
	.num_conf = ARRAY_SIZE(s5k4e1_confs),
};

static struct msm_sensor_ctrl_t s5k4e1_s_ctrl = {
	.msm_sensor_reg = &s5k4e1_regs,
	.sensor_i2c_client = &s5k4e1_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.mirror_flip = CAMERA_SENSOR_NONE,
	.sensor_output_reg_addr = &s5k4e1_reg_addr,
	.sensor_id_info = &s5k4e1_id_info,
	.sensor_exp_gain_info = &s5k4e1_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.csic_params = &s5k4e1_csi_params_array[0],
	.msm_sensor_mutex = &s5k4e1_mut,
	.sensor_i2c_driver = &s5k4e1_i2c_driver,
	.sensor_v4l2_subdev_info = s5k4e1_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(s5k4e1_subdev_info),
	.sensor_v4l2_subdev_ops = &s5k4e1_subdev_ops,
	.func_tbl = &s5k4e1_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Samsung 5MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");


