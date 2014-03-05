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
 */
#include <asm/htc_version.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/irqs-7xxx.h>
#include "devices-msm7x2xa.h"
#include "board-g3u.h"
#include <mach/vreg.h>
#include <media/msm_camera.h>

static int config_gpio_table(uint32_t *table, int len);

#ifdef CONFIG_MSM_CAMERA_V4L2

#if defined(CONFIG_S5K4E1) || defined(CONFIG_MT9V113)

struct msm_camera_device_platform_data g3u_msm_camera_csi_device_data[] = {

	{
		.ioclk.mclk_clk_rate = 24000000,
		.ioclk.vfe_clk_rate  = 266000000,
		.csid_core = 1,
		.is_csic = 1,
	},
	{
		.ioclk.mclk_clk_rate = 24000000,
		.ioclk.vfe_clk_rate  = 266000000,
		.csid_core = 0,
		.is_csic = 1,
	},
};

static int config_gpio_table(uint32_t *table, int len)
{
	int rc = 0, i = 0;

	for (i = 0; i < len; i++) {
		rc = gpio_tlmm_config(table[i], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[CAM]%s not able to get gpio\n", __func__);
			for (i--; i >= 0; i--)
				gpio_tlmm_config(table[i],
					GPIO_CFG_ENABLE);
		break;
		}
	}
	return rc;
}

static uint32_t camera_off_gpio_table[] = {
	GPIO_CFG(G3U_GPIO_CAM_I2C_SDA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(G3U_GPIO_CAM_I2C_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	
	
	GPIO_CFG(G3U_GPIO_CAM_MCLK,    0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
};

static uint32_t camera_on_gpio_table[] = {
	GPIO_CFG(G3U_GPIO_CAM_I2C_SDA, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(G3U_GPIO_CAM_I2C_SCL, 1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	GPIO_CFG(G3U_GPIO_CAM_MCLK,    1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_16MA),
};



static struct msm_camera_gpio_conf gpio_conf = {
	.camera_off_table = camera_off_gpio_table,
	.camera_off_table_size = ARRAY_SIZE(camera_off_gpio_table),
	.camera_on_table = camera_on_gpio_table,
	.camera_on_table_size = ARRAY_SIZE(camera_on_gpio_table),
	.gpio_no_mux = 1,
};
#endif

#if defined(CONFIG_S5K4E1)
static struct msm_camera_sensor_flash_data flash_s5k4e5yx = {
	.flash_type             = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info s5k4e1_sensor_7627a_info = {
	.mount_angle = 90,
	.gpio_conf = &gpio_conf,
	.mirror_flip = CAMERA_SENSOR_NONE,
};

static void g3u_camera_vreg_config_s5k4e1(int vreg_en)
{

	int rc;

	if (vreg_en)
	{


		if (htc_get_board_revision() >= BOARD_G3U_EVT_XB)
		{
			
			rc = gpio_request(G3U_GPIO_CAM_VCM2V85_EN, "GPIO_CAM_VCM2V85");
			pr_info("[CAM] G3U_GPIO_CAM_VCM2V85, %d\n", G3U_GPIO_CAM_VCM2V85_EN);
			if (rc < 0) {
				pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_VCM2V85_EN);
			} else {
				gpio_tlmm_config(
				GPIO_CFG(G3U_GPIO_CAM_VCM2V85_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
			}
			gpio_set_value(G3U_GPIO_CAM_VCM2V85_EN, 1);
			gpio_direction_output(G3U_GPIO_CAM_VCM2V85_EN,1);
			udelay(50);
		}

		
		rc = gpio_request(G3U_GPIO_CAM_A2V85_EN, "GPIO_CAM_A2V85");
		pr_info("[CAM] G3U_GPIO_CAM_A2V85_EN, %d\n", G3U_GPIO_CAM_A2V85_EN);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_A2V85_EN);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAM_A2V85_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_set_value(G3U_GPIO_CAM_A2V85_EN, 1);
		gpio_direction_output(G3U_GPIO_CAM_A2V85_EN,1);
		udelay(50);

		
		rc = gpio_request(G3U_GPIO_CAMIO_1V8_EN, "GPIO_CAMIO_1V8");
		pr_info("[CAM] G3U_GPIO_CAMIO_1V8_EN, %d\n", G3U_GPIO_CAMIO_1V8_EN);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAMIO_1V8_EN);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAMIO_1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_set_value(G3U_GPIO_CAMIO_1V8_EN, 1);
		gpio_direction_output(G3U_GPIO_CAMIO_1V8_EN,1);
		udelay(50);

		if (htc_get_board_revision() < BOARD_G3U_EVT_XB)
		{
			
			rc = gpio_request(G3U_GPIO_CAM_VDD1V5_EN, "GPIO_CAM_VDD1V5");
			pr_info("[CAM] G3U_GPIO_CAM_VDD1V5_EN, %d\n", G3U_GPIO_CAM_VDD1V5_EN);
			if (rc < 0) {
				pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_VDD1V5_EN);
			} else {
				gpio_tlmm_config(
				GPIO_CFG(G3U_GPIO_CAM_VDD1V5_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
			}
			gpio_set_value(G3U_GPIO_CAM_VDD1V5_EN, 1);
			gpio_direction_output(G3U_GPIO_CAM_VDD1V5_EN,1);
			udelay(50);
		}

		
		rc = gpio_request(G3U_GPIO_CAM_SEL, "G3U_GPIO_CAM_SEL");
		pr_info("[CAM] G3U_GPIO_CAM_SEL, %d\n", G3U_GPIO_CAM_SEL);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_SEL);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_set_value(G3U_GPIO_CAM_SEL, 0);
		gpio_direction_output(G3U_GPIO_CAM_SEL, 0);
		udelay(50);

	}
	else
	{
		
		pr_info("[CAM] G3U_GPIO_CAM_SEL, %d = 0\n", G3U_GPIO_CAM_SEL);
		gpio_direction_output(G3U_GPIO_CAM_SEL, 0);
		gpio_free(G3U_GPIO_CAM_SEL);
		udelay(50);

		if (htc_get_board_revision() < BOARD_G3U_EVT_XB)
		{
			
			pr_info("[CAM] G3U_GPIO_CAM_VDD1V5_EN, %d = 0\n", G3U_GPIO_CAM_VDD1V5_EN);
			gpio_direction_output(G3U_GPIO_CAM_VDD1V5_EN,0);
			gpio_free(G3U_GPIO_CAM_VDD1V5_EN);
			udelay(50);
		}

		
		pr_info("[CAM] G3U_GPIO_CAMIO_1V8_EN, %d = 0\n", G3U_GPIO_CAMIO_1V8_EN);
		gpio_direction_output(G3U_GPIO_CAMIO_1V8_EN,0);
		gpio_free(G3U_GPIO_CAMIO_1V8_EN);
		udelay(50);

		
		pr_info("[CAM] G3U_GPIO_CAM_A2V85_EN, %d = 0\n", G3U_GPIO_CAM_A2V85_EN);
		gpio_direction_output(G3U_GPIO_CAM_A2V85_EN,0);
		gpio_free(G3U_GPIO_CAM_A2V85_EN);
		udelay(50);

		if (htc_get_board_revision() >= BOARD_G3U_EVT_XB)
		{
			
			pr_info("[CAM] G3U_GPIO_CAM_VCM2V85, %d = 0\n", G3U_GPIO_CAM_VCM2V85_EN);
			gpio_direction_output(G3U_GPIO_CAM_VCM2V85_EN,0);
			gpio_free(G3U_GPIO_CAM_VCM2V85_EN);
			udelay(50);
		}

	}



}

static int config_camera_on_gpios_rear_s5k4e1(void)
{
	int rc = 0;

	g3u_camera_vreg_config_s5k4e1(1);

	pr_info("[CAM]%s: config gpio on table\n", __func__);
	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("[CAM]%s: CAMSENSOR gpio table request failed\n", __func__);
		return rc;
	}

	return rc;
}

static int config_camera_off_gpios_rear_s5k4e1(void)
{
	int rc = 0;


	pr_info("[CAM]%s: \n", __func__);

	g3u_camera_vreg_config_s5k4e1(0);

	pr_info("[CAM]%s %d: \n", __func__, __LINE__);

	rc = config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));

	pr_info("[CAM]%s %d: \n", __func__, __LINE__);

	if (rc < 0) {
		pr_err("[CAM]%s: CAMSENSOR gpio table request failed\n", __func__);
		return rc;
	}
	return rc;
}

#if defined(CONFIG_AD5823_ACT)
#if defined(CONFIG_S5K4E1)
static struct i2c_board_info ad5823_actuator_i2c_info = {
	I2C_BOARD_INFO("ad5823_act", 0x1C),
};

static struct msm_actuator_info ad5823_actuator_info = {
	.board_info     = &ad5823_actuator_i2c_info,
	.bus_id         = MSM_GSBI0_QUP_I2C_BUS_ID,
	.vcm_pwd        = G3U_GPIO_CAM_VCM_PD,
	.vcm_enable     = 1,
};
#endif
#endif

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name = "s5k4e1",
	.camera_power_on = config_camera_on_gpios_rear_s5k4e1,
	.camera_power_off = config_camera_off_gpios_rear_s5k4e1,
	.sensor_reset_enable = 1,
	.sensor_reset = G3U_GPIO_CAM_RST,

	.pmic_gpio_enable    = 0,
	.pdata = &g3u_msm_camera_csi_device_data[0],
	.flash_data             = &flash_s5k4e5yx,
	.sensor_platform_info = &s5k4e1_sensor_7627a_info,
#ifdef CONFIG_AD5823_ACT
	.actuator_info = &ad5823_actuator_info,
#endif
	.csi_if = 1,
	.use_rawchip = 0,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};
#endif

#if defined(CONFIG_MT9V113)
static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type             = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info mt9v113_sensor_7627a_info = {
	.mount_angle = 270,
	.gpio_conf = &gpio_conf,
	.mirror_flip = CAMERA_SENSOR_NONE,
};

static void g3u_camera_vreg_config_mt9v113(int vreg_en)
{

	int rc;

	if (vreg_en)
	{
		if (htc_get_board_revision() >= BOARD_G3U_EVT_XB)
		{
			
			rc = gpio_request(G3U_GPIO_CAM_VCM2V85_EN, "GPIO_CAM_VCM2V85");
			pr_info("[CAM] G3U_GPIO_CAM_VCM2V85, %d\n", G3U_GPIO_CAM_VCM2V85_EN);
			if (rc < 0) {
				pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_VCM2V85_EN);
			} else {
				gpio_tlmm_config(
				GPIO_CFG(G3U_GPIO_CAM_VCM2V85_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
				GPIO_CFG_ENABLE);
			}
			gpio_set_value(G3U_GPIO_CAM_VCM2V85_EN, 1);
			gpio_direction_output(G3U_GPIO_CAM_VCM2V85_EN,1);
			udelay(50);
		}

		
		rc = gpio_request(G3U_GPIO_CAM_A2V85_EN, "GPIO_CAM_A2V85");
		pr_info("[CAM] G3U_GPIO_CAM_A2V85_EN, %d\n", G3U_GPIO_CAM_A2V85_EN);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_A2V85_EN);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAM_A2V85_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_direction_output(G3U_GPIO_CAM_A2V85_EN,1);
		udelay(50);


		
		rc = gpio_request(G3U_GPIO_CAMIO_1V8_EN, "GPIO_CAMIO_1V8");
		pr_info("[CAM] G3U_GPIO_CAMIO_1V8_EN, %d\n", G3U_GPIO_CAMIO_1V8_EN);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAMIO_1V8_EN);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAMIO_1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_direction_output(G3U_GPIO_CAMIO_1V8_EN,1);
		udelay(50);


		
		rc = gpio_request(G3U_GPIO_CAM2_VDD1V8_EN, "G3U_GPIO_CAM2_VDD1V8_EN");
		pr_info("[CAM] G3U_GPIO_CAM2_VDD1V8_EN, %d\n", G3U_GPIO_CAM2_VDD1V8_EN);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM2_VDD1V8_EN);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAM2_VDD1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_direction_output(G3U_GPIO_CAM2_VDD1V8_EN,1);
		udelay(50);


		
		rc = gpio_request(G3U_GPIO_CAM_SEL, "G3U_GPIO_CAM_SEL");
		pr_info("[CAM] G3U_GPIO_CAM_SEL, %d\n", G3U_GPIO_CAM_SEL);
		if (rc < 0) {
			pr_err("[CAM] GPIO(%d) request failed\n", G3U_GPIO_CAM_SEL);
		} else {
			gpio_tlmm_config(
			GPIO_CFG(G3U_GPIO_CAM_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		}
		gpio_set_value(G3U_GPIO_CAM_SEL, 1);
		gpio_direction_output(G3U_GPIO_CAM_SEL, 1);
		udelay(50);

	}
	else
	{
		
		pr_info("[CAM] G3U_GPIO_CAM2_VDD1V8_EN, %d = 0\n", G3U_GPIO_CAM2_VDD1V8_EN);
		gpio_direction_output(G3U_GPIO_CAM2_VDD1V8_EN,0);
		gpio_free(G3U_GPIO_CAM2_VDD1V8_EN);
		udelay(50);


		
		pr_info("[CAM] G3U_GPIO_CAMIO_1V8_EN, %d = 0\n", G3U_GPIO_CAMIO_1V8_EN);
		gpio_direction_output(G3U_GPIO_CAMIO_1V8_EN,0);
		gpio_free(G3U_GPIO_CAMIO_1V8_EN);
		udelay(50);


		
		pr_info("[CAM] G3U_GPIO_CAM_A2V85_EN, %d = 0\n", G3U_GPIO_CAM_A2V85_EN);
		gpio_direction_output(G3U_GPIO_CAM_A2V85_EN,0);
		gpio_free(G3U_GPIO_CAM_A2V85_EN);
		udelay(50);

		
		pr_info("[CAM] G3U_GPIO_CAM_SEL, %d = 0\n", G3U_GPIO_CAM_SEL);
		gpio_direction_output(G3U_GPIO_CAM_SEL, 1);
		gpio_free(G3U_GPIO_CAM_SEL);
		udelay(50);

		if (htc_get_board_revision() >= BOARD_G3U_EVT_XB)
		{
			
			pr_info("[CAM] G3U_GPIO_CAM_VCM2V85, %d = 0\n", G3U_GPIO_CAM_VCM2V85_EN);
			gpio_direction_output(G3U_GPIO_CAM_VCM2V85_EN,0);
			gpio_free(G3U_GPIO_CAM_VCM2V85_EN);
			udelay(50);
		}
	}
}

static int config_camera_on_gpios_front_mt9v113(void)
{
	int rc = 0;

	g3u_camera_vreg_config_mt9v113(1);

	pr_info("[CAM]%s: config gpio on table\n", __func__);
	rc = config_gpio_table(camera_on_gpio_table,
			ARRAY_SIZE(camera_on_gpio_table));
	if (rc < 0) {
		pr_err("[CAM]%s: CAMSENSOR gpio table request failed\n", __func__);
		return rc;
	}

	return rc;
}

static int config_camera_off_gpios_front_mt9v113(void)
{
	int rc = 0;

	pr_info("[CAM]%s: \n", __func__);
	g3u_camera_vreg_config_mt9v113(0);

	rc = config_gpio_table(camera_off_gpio_table,
			ARRAY_SIZE(camera_off_gpio_table));

	pr_info("[CAM]%s %d: \n", __func__, __LINE__);

	if (rc < 0) {
		pr_err("[CAM]%s: CAMSENSOR gpio table request failed\n", __func__);
		return rc;
	}
	return rc;
}


static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
	.sensor_name = "mt9v113",
	.camera_power_on = config_camera_on_gpios_front_mt9v113,
	.camera_power_off = config_camera_off_gpios_front_mt9v113,
	.sensor_reset_enable = 1,
	.sensor_reset = G3U_GPIO_CAM2_RST,
	
	.pmic_gpio_enable    = 0,
	.pdata = &g3u_msm_camera_csi_device_data[1],
	.flash_data             = &flash_mt9v113,	
	.sensor_platform_info = &mt9v113_sensor_7627a_info,
	.csi_if = 1,
	.use_rawchip = 0,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};
#endif


static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

static void __init g3u_init_cam(void)
{
    platform_device_register(&msm_camera_server);
	platform_device_register(&msm8625_device_csic0);
	platform_device_register(&msm8625_device_csic1);
	platform_device_register(&msm7x27a_device_clkctl);
	platform_device_register(&msm7x27a_device_vfe);


	if (htc_get_board_revision() >= BOARD_G3U_EVT_XB) {
		s5k4e1_sensor_7627a_info.mirror_flip = CAMERA_SENSOR_NONE;
		pr_info("[CAM]: s5k4e1 - Use XB layout - mirror_flip = CAMERA_SENSOR_NONE\n");
	} else {
		s5k4e1_sensor_7627a_info.mirror_flip = CAMERA_SENSOR_MIRROR_FLIP;
		pr_info("[CAM]: s5k4e1 - Use XA layout - mirror_flip = CAMERA_SENSOR_MIRROR_FLIP\n");
		msm_camera_sensor_s5k4e1_data.actuator_info = NULL;	
	}

}

static struct i2c_board_info i2c_camera_devices[] = {
#ifdef CONFIG_S5K4E1
	{
		I2C_BOARD_INFO("s5k4e1", 0x20 >> 1),
		.platform_data = &msm_camera_sensor_s5k4e1_data,
	},
#endif
#ifdef CONFIG_MT9V113
	{
		I2C_BOARD_INFO("mt9v113", 0x3C >> 1),
		.platform_data = &msm_camera_sensor_mt9v113_data,
	},
#endif
};

void __init g3u_camera_init(void)
{

	g3u_init_cam();

#ifdef CONFIG_I2C
	pr_info("[CAM]%s: i2c_register_board_info\n", __func__);

	i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				i2c_camera_devices, ARRAY_SIZE(i2c_camera_devices));	
#endif
}
#endif

