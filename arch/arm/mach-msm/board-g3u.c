/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio_event.h>
#include <linux/memblock.h>
#include <asm/mach-types.h>
#include <linux/memblock.h>
#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#ifdef CONFIG_USB_MSM_OTG_72K
#include <mach/msm_hsusb.h>
#else
#include <linux/usb/msm_hsusb.h>
#endif
#include <mach/htc_usb.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#ifdef CONFIG_USB_G_ANDROID
#include <linux/usb/android_composite.h>
#include <mach/usbdiag.h>
#endif
#include <mach/msm_memtypes.h>
#include <mach/msm_serial_hs.h>
#include <linux/usb/android.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <mach/socinfo.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/mach/mmc.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/htc_flashlight.h>
#include <linux/gpio.h>
#include <linux/android_pmem.h>
#include <linux/bootmem.h>
#include <linux/mfd/marimba.h>
#include <mach/vreg.h>
#include <linux/power_supply.h>
#include <linux/regulator/consumer.h>
#include <mach/rpc_pmapp.h>
#include <mach/system.h>
#include <linux/smsc911x.h>
#include <linux/atmel_maxtouch.h>
#include <linux/fmem.h>
#include <linux/msm_adc.h>
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_3K
#include <linux/synaptics_i2c_rmi.h>
#endif
#include <linux/max17050_battery.h>
#include <linux/max17050_gauge.h>
#include <linux/tps65200.h>
#include <mach/htc_battery.h>
#include <mach/htc_headset_mgr.h>
#include <mach/htc_headset_gpio.h>
#include <mach/htc_headset_pmic.h>
#include <mach/htc_headset_one_wire.h>
#include <linux/ion.h>
#include <asm/setup.h>
#include "devices.h"
#include "timer.h"
#include "board-msm7x27a-regulator.h"
#include "devices-msm7x2xa.h"
#include <mach/board_htc.h>
#include "pm.h"
#include <mach/rpc_server_handset.h>
#include <mach/socinfo.h>
#include <linux/leds-pm8029.h>
#include <linux/bma250.h>
#include "pm-boot.h"
#include <linux/proc_fs.h>
#include <linux/cm3629.h>
#include "board-g3u.h"
#include "board-g3u-wifi.h"
#ifdef CONFIG_PERFLOCK
#include <mach/perflock.h>
#endif
#ifdef CONFIG_BT
#include <mach/htc_bdaddress.h>
#endif
#include <mach/htc_util.h>

int htc_get_usb_accessory_adc_level(uint32_t *buffer);
#include <mach/cable_detect.h>

extern int emmc_partition_read_proc(char *page, char **start, off_t off,
					int count, int *eof, void *data);

#if defined(CONFIG_GPIO_SX150X)
enum {
	SX150X_CORE,
};

static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CORE]	= {
		.gpio_base		= GPIO_CORE_EXPANDER_BASE,
		.oscio_is_gpo		= false,
		.io_pullup_ena		= 0,
		.io_pulldn_ena		= 0x02,
		.io_open_drain_ena	= 0xfef8,
		.irq_summary		= -1,
	},
};
#endif


static struct platform_device msm_wlan_ar6000_pm_device = {
	.name           = "wlan_ar6000_pm_dev",
	.id             = -1,
};

#if defined(CONFIG_I2C) && defined(CONFIG_GPIO_SX150X)
static struct i2c_board_info core_exp_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
	},
};

static void __init register_i2c_devices(void)
{
	if (machine_is_msm7x27a_surf() || machine_is_msm7625a_surf() ||
			machine_is_msm8625_surf() || machine_is_g3u() || machine_is_protodug() || machine_is_magnids() || machine_is_protodcg())
		sx150x_data[SX150X_CORE].io_open_drain_ena = 0xe0f0;

	core_exp_i2c_info[0].platform_data =
			&sx150x_data[SX150X_CORE];

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				core_exp_i2c_info,
				ARRAY_SIZE(core_exp_i2c_info));
}
#endif
#if defined(CONFIG_INPUT_CAPELLA_CM3629)
#define G3U_GPIO_PROXIMITY_INT 36
static uint8_t cm3629_mapping_table[] = {0x0, 0x3, 0x6, 0x9, 0xC,
					0xF, 0x12, 0x15, 0x18, 0x1B,
					0x1E, 0x21, 0x24, 0x27, 0x2A,
					0x2D, 0x30, 0x33, 0x36, 0x39,
					0x3C, 0x3F, 0x43, 0x47, 0x4B,
					0x4F, 0x53, 0x57, 0x5B, 0x5F,
					0x63, 0x67, 0x6B, 0x70, 0x75,
					0x7A, 0x7F, 0x84, 0x89, 0x8E,
					0x93, 0x98, 0x9D, 0xA2, 0xA8,
					0xAE, 0xB4, 0xBA, 0xC0, 0xC6,
					0xCC, 0xD3, 0xDA, 0xE1, 0xE8,
					0xEF, 0xF6, 0xFF};
static struct cm3629_platform_data cm36282_pdata = {
        .model = CAPELLA_CM36282,
        .ps_select = CM3629_PS1_ONLY,
        .intr = G3U_GPIO_PROXIMITY_INT,
        .levels = {9, 33, 58, 250, 587, 3009, 4806, 9740, 14675, 65535},
        .golden_adc = 0xD9A,
        .power = NULL,
        .cm3629_slave_address = 0xC0>>1,
        .ps_calibration_rule = 5,
        .ps1_thd_set = 0x08,
        .ps1_thh_diff = 2,
        .ps1_thd_no_cal = 0xF1,
        .ps1_thd_with_cal = 0x08,
        .ps_conf1_val = CM3629_PS_DR_1_40 | CM3629_PS_IT_1_6T |
                        CM3629_PS1_PERS_2,
        .ps_conf2_val = CM3629_PS_ITB_1 | CM3629_PS_ITR_1 |
                        CM3629_PS2_INT_DIS | CM3629_PS1_INT_DIS,
        .ps_conf3_val = CM3629_PS2_PROL_32,
	.dark_level = 1,
	.dynamical_threshold = 1,
	.mapping_table = cm3629_mapping_table,
	.mapping_size = ARRAY_SIZE(cm3629_mapping_table),
};

static struct i2c_board_info i2c_cm36282_devices[] = {
        {
                I2C_BOARD_INFO(CM3629_I2C_NAME,0xC0 >> 1),
        .platform_data = &cm36282_pdata,
        .irq = MSM_GPIO_TO_INT(G3U_GPIO_PROXIMITY_INT),
        },
};
#endif

static uint32_t qup_i2c_gpio_table_io[] = {
	GPIO_CFG(60, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(61, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(131, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(132, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static uint32_t qup_i2c_gpio_table_hw[] = {
	GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
	GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
};

static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
	if (adap_id < 0 || adap_id > 1)
		return;

	pr_info("%s: adap_id = %d, config_type = %d\n",
		__func__, adap_id, config_type);

	if (config_type){
		gpio_tlmm_config(qup_i2c_gpio_table_hw[adap_id*2], GPIO_CFG_ENABLE);
		gpio_tlmm_config(qup_i2c_gpio_table_hw[adap_id*2 + 1], GPIO_CFG_ENABLE);
	} else {
		gpio_tlmm_config(qup_i2c_gpio_table_io[adap_id*2], GPIO_CFG_ENABLE);
		gpio_tlmm_config(qup_i2c_gpio_table_io[adap_id*2 + 1], GPIO_CFG_ENABLE);
	}
}

static struct msm_i2c_platform_data msm_gsbi0_qup_i2c_pdata = {
	.clk_freq		= 400000,
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi1_qup_i2c_pdata = {
	.clk_freq		= 400000,
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};
#ifdef CONFIG_ARCH_MSM7X27A
#define CAMERA_ZSL_SIZE		(SZ_1M * 60)
#endif

#ifdef CONFIG_ION_MSM
#define MSM_ION_HEAP_NUM        4
static struct platform_device ion_dev;
static int msm_ion_camera_size;
static int msm_ion_audio_size;
static int msm_ion_sf_size;
#endif
static struct android_usb_platform_data android_usb_pdata = {
	.vendor_id              = 0x0bb4,
#ifdef CONFIG_MACH_G3U
	.product_id             = 0x061D,
#else
	
	.product_id             = 0x0dd0,
#endif
	.version                = 0x0100,
	.product_name           = "Android Phone",
	.manufacturer_name      = "HTC",
	.num_products           = ARRAY_SIZE(usb_products),
	.products               = usb_products,
	.num_functions          = ARRAY_SIZE(usb_functions_all),
	.functions              = usb_functions_all,
	.usb_id_pin_gpio	= G3U_GPIO_USB_ID,
	.usb_diag_interface	= "diag",
	.fserial_init_string = "tty:modem,tty:autobot,tty:serial,tty:autobot,tty:acm",
	.nluns                  = 1,
};

static int __init board_serialno_setup(char *serialno)
{
	android_usb_pdata.serial_number = serialno;
	return 1;
}
__setup("androidboot.serialno=", board_serialno_setup);
static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

#ifdef CONFIG_USB_EHCI_MSM_72K
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	int rc = 0;
	unsigned gpio;

	gpio = GPIO_HOST_VBUS_EN;

	rc = gpio_request(gpio, "i2c_host_vbus_en");
	if (rc < 0) {
		pr_err("failed to request %d GPIO\n", gpio);
		return;
	}
	gpio_direction_output(gpio, !!on);
	gpio_set_value_cansleep(gpio, !!on);
	gpio_free(gpio);
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info       = (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
};

static void __init msm7x2x_init_host(void)
{
	msm_add_host(0, &msm_usb_host_pdata);
}
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}

static struct regulator *reg_hsusb;
static int msm_hsusb_ldo_init(int init)
{
	int rc = 0;

	if (init) {
		reg_hsusb = regulator_get(NULL, "usb");
		if (IS_ERR(reg_hsusb)) {
			rc = PTR_ERR(reg_hsusb);
			pr_err("%s: could not get regulator: %d\n",
					__func__, rc);
			goto out;
		}

		rc = regulator_set_voltage(reg_hsusb, 3300000, 3300000);
		if (rc) {
			pr_err("%s: could not set voltage: %d\n",
					__func__, rc);
			goto reg_free;
		}

		return 0;
	}
	
reg_free:
	regulator_put(reg_hsusb);
out:
	reg_hsusb = NULL;
	return rc;
}

static int msm_hsusb_ldo_enable(int enable)
{
	static int ldo_status;

	if (IS_ERR_OR_NULL(reg_hsusb))
		return reg_hsusb ? PTR_ERR(reg_hsusb) : -ENODEV;

	if (ldo_status == enable)
		return 0;

	ldo_status = enable;

	return enable ?
		regulator_enable(reg_hsusb) :
		regulator_disable(reg_hsusb);
}

#ifndef CONFIG_USB_EHCI_MSM_72K
static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init)
{
	int ret = 0;

	if (init)
		ret = msm_pm_app_rpc_init(callback);
	else
		msm_pm_app_rpc_deinit(callback);

	return ret;
}
#endif

static int phy_init_settings[] =
{	0x0c, 0x31,
	0x2a, 0x32,
        -1
};

static struct msm_otg_platform_data msm_otg_pdata = {
        .vbus_power              = msm_hsusb_vbus_power,
        .rpc_connect             = hsusb_rpc_connect,
        .pemp_level              = PRE_EMPHASIS_WITH_20_PERCENT,
        .cdr_autoreset           = CDR_AUTO_RESET_DISABLE,
        .drv_ampl                = HS_DRV_AMPLITUDE_DEFAULT,
        .se1_gating              = SE1_GATING_DISABLE,
        .ldo_init                = msm_hsusb_ldo_init,
        .ldo_enable              = msm_hsusb_ldo_enable,
        .chg_init                = hsusb_chg_init,
        .chg_connected           = hsusb_chg_connected,
        .chg_vbus_draw           = hsusb_chg_vbus_draw,
};

void g3u_add_usb_devices(void)
{
#if 0
FIXME
	printk(KERN_INFO "%s rev: %d\n", __func__, system_rev);
#endif
	android_usb_pdata.products[0].product_id =
		android_usb_pdata.product_id;

	
	if (get_radio_flag() & 0x20000)
		android_usb_pdata.diag_init = 1;
	
	if (board_mfg_mode() == 0) {
		android_usb_pdata.nluns = 1;
		android_usb_pdata.cdrom_lun = 0x1;
	}

	msm8625_device_gadget_peripheral.dev.parent = &msm8625_device_otg.dev;
	platform_device_register(&msm8625_device_gadget_peripheral);
	platform_device_register(&android_usb_device);
}


#endif

static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.phy_init_seq		= phy_init_settings,
	.is_phy_status_timer_on = 1,
};

#ifdef CONFIG_PERFLOCK
static unsigned msm7x27a_perf_acpu_table[] = {
	480000000, 
	700800000, 
	1008000000,
	1008000000,
	1008000000,
};

static struct perflock_data msm7x27a_floor_data = {
	.perf_acpu_table = msm7x27a_perf_acpu_table,
	.table_size = ARRAY_SIZE(msm7x27a_perf_acpu_table),
};

static struct perflock_data msm7x27a_cpufreq_ceiling_data = {
	.perf_acpu_table = msm7x27a_perf_acpu_table,
	.table_size = ARRAY_SIZE(msm7x27a_perf_acpu_table),
};

static struct perflock_pdata perflock_pdata = {
	.perf_floor = &msm7x27a_floor_data,
	.perf_ceiling = &msm7x27a_cpufreq_ceiling_data,
};

struct platform_device msm7x27a_device_perf_lock = {
	.name = "perf_lock",
	.id = -1,
	.dev = {
		.platform_data = &perflock_pdata,
	},
};

#ifdef CONFIG_PERFLOCK_SCREEN_POLICY
static struct perf_lock screen_off_ceiling_lock;
static struct perflock_screen_policy msm7x27a_screen_off_policy = {
	.on_min  = NULL,
	.off_max = &screen_off_ceiling_lock,
};
#endif 
#endif

static int get_thermal_id(void)
{
	return THERMAL_470_100_4360;
}
static int get_battery_id(void)
{
	return BATTERY_ID_FORMOSA_SANYO;
}

static int dq_result = 0;
static void g3u_poweralg_config_init(struct poweralg_config_type *config)
{
	INT32 batt_id = 0;

	batt_id = get_batt_id();

	pr_info("[BATT] dq_result -> %d, batt_id -> %d\n",dq_result, batt_id);
	if (dq_result == 1) {
		config->full_charging_mv = 4250;
		config->voltage_exit_full_mv = 4200;
		config->capacity_recharge_p = 98;
	} else {
		config->full_charging_mv = 4100;
		config->voltage_exit_full_mv = 4000;
		config->capacity_recharge_p = 98;
	}

	config->full_charging_ma = 160;
	config->full_pending_ma = 50;
	config->full_charging_timeout_sec = 60 * 60;
	config->min_taper_current_mv = 0;	
	config->min_taper_current_ma = 0;	
	config->wait_votlage_statble_sec = 1 * 60;
	config->predict_timeout_sec = 10;
	config->polling_time_in_charging_sec = 30;

	config->enable_full_calibration = TRUE;
	config->enable_weight_percentage = TRUE;
	config->software_charger_timeout_sec = 57600; 
	config->superchg_software_charger_timeout_sec = 0;	
	config->charger_hw_safety_timer_watchdog_sec =  0;	

	config->debug_disable_shutdown = FALSE;
	config->debug_fake_room_temp = FALSE;

	config->debug_disable_hw_timer = FALSE;
	config->debug_always_predict = FALSE;
	config->full_level = 0;

	config->smooth_chg_full_delay_min = 1;
}

static int g3u_update_charging_protect_flag(int ibat_ma, int vbat_mv, int temp_01c, BOOL* chg_allowed, BOOL* hchg_allowed, BOOL* temp_fault)
{
	static int pState = 0;
	int old_pState = pState;
	enum {
		PSTAT_DETECT = 0,
		PSTAT_LOW_STOP,
		PSTAT_NORMAL,
		PSTAT_SLOW,
		PSTAT_LIMITED,
		PSTAT_HIGH_STOP
	};

	
	pr_debug("[BATT] %s(i=%d, v=%d, t=%d, %d, %d)\n",__func__, ibat_ma, vbat_mv, temp_01c, *chg_allowed, *hchg_allowed);
	switch(pState) {
		default:
			pr_info("[BATT] error: unexpected pState\n");
		case PSTAT_DETECT:
			if (temp_01c < 0)
				pState = PSTAT_LOW_STOP;
			if ((0 <= temp_01c) && (temp_01c <= 450))
				pState = PSTAT_NORMAL;
			if ((450 < temp_01c) && (temp_01c <= 480))
				pState = PSTAT_SLOW;
			if ((480 < temp_01c) && (temp_01c <= 600))
				pState = PSTAT_LIMITED;
			if (600 < temp_01c)
				pState = PSTAT_HIGH_STOP;
			break;
		case PSTAT_LOW_STOP:
			if (30 <= temp_01c)
				pState = PSTAT_NORMAL;
			
			break;
		case PSTAT_NORMAL:
			if (temp_01c < 0)
				pState = PSTAT_LOW_STOP;
			else if (600 < temp_01c)
				pState = PSTAT_HIGH_STOP;
			else if (480 < temp_01c) 
				pState = PSTAT_LIMITED;
			else if (450 < temp_01c)
				pState = PSTAT_SLOW;
			break;
		case PSTAT_SLOW:
			if (temp_01c < 0)
				pState = PSTAT_LOW_STOP;
			else if (600 < temp_01c)
				pState = PSTAT_HIGH_STOP;
			else if (480 < temp_01c)
				pState = PSTAT_LIMITED;
			else if (temp_01c <= 420)
				pState = PSTAT_NORMAL;
			break;
		case PSTAT_LIMITED:
			if (temp_01c < 0)
				pState = PSTAT_LOW_STOP;
			else if (600 < temp_01c)
				pState = PSTAT_HIGH_STOP;
			else if (temp_01c <= 420)
				pState = PSTAT_NORMAL;
			else if (temp_01c <= 450)
				pState = PSTAT_SLOW;
			break;
		case PSTAT_HIGH_STOP:
			if (temp_01c <= 420)
				pState = PSTAT_NORMAL;
			else if (temp_01c <= 450)
				pState = PSTAT_SLOW;
			else if (temp_01c <= 570)
				pState = PSTAT_LIMITED;
			
			break;
	}
	if (old_pState != pState)
		pr_info("[BATT] Protect pState changed from %d to %d\n", old_pState, pState);

	
	switch(pState) {
		default:
		case PSTAT_DETECT:
			pr_info("[BATT] error: unexpected pState\n");
			break;
		case PSTAT_LOW_STOP:
			*chg_allowed = FALSE;
			*hchg_allowed = FALSE;
			break;
		case PSTAT_NORMAL:
		case PSTAT_SLOW:
			*chg_allowed = TRUE;
			*hchg_allowed = TRUE;
			break;
#if 0 
		case PSTAT_SLOW:	
			if (4200 < vbat_mv)
				*chg_allowed = FALSE;
			else if (vbat_mv <= 4150)
				*chg_allowed = TRUE;
			*hchg_allowed = TRUE;
			break;
#endif
		case PSTAT_LIMITED:	
			if (PSTAT_LIMITED != old_pState)
				*chg_allowed = TRUE;
			if (4100 < vbat_mv)
				*chg_allowed = FALSE;
			else
				*chg_allowed = TRUE;
			*hchg_allowed = TRUE;
			break;
		case PSTAT_HIGH_STOP:
			*chg_allowed = FALSE;
			*hchg_allowed = FALSE;
			break;
	}

	
	if (PSTAT_NORMAL == pState || PSTAT_SLOW == pState)
		*temp_fault = FALSE;
	else
		*temp_fault = TRUE;

	return pState;
}

static struct htc_battery_platform_data htc_battery_pdev_data = {
	.gpio_mbat_in = G3U_GPIO_MBAT_IN,
	.gpio_mbat_in_trigger_level = MBAT_IN_HIGH_TRIGGER,
	.mbat_in_keep_charging = 1,
	.mbat_in_unreg_rmt = 1,
	.chg_limit_active_mask = HTC_BATT_CHG_LIMIT_BIT_TALK,
	.func_show_batt_attr = htc_battery_show_attr,
	.func_show_htc_extension_attr = htc_battery_show_htc_extension_attr,
	.func_get_batt_rt_attr = htc_battery_get_rt_attr,
	.suspend_highfreq_check_reason = SUSPEND_HIGHFREQ_CHECK_BIT_TALK,
	.guage_driver = GAUGE_MAX17050,
	.charger = SWITCH_CHARGER_TPS65200,
	.m2a_cable_detect = 1,
	.enable_bootup_voltage = 3300,
	.gpio_hw_chg_led = G3U_GPIO_HW_CHG_LED,
};

static struct platform_device htc_battery_pdev = {
	.name = "htc_battery",
	.id = -1,
	.dev	= {
		.platform_data = &htc_battery_pdev_data,
	},
};

UINT32 m_parameter_TWS_SDI_1650mah[] = {
	
	10000, 4250, 8100, 4101, 5100, 3846,
	2000, 3700, 900, 3641, 0, 3411,
};
UINT32 m_parameter_Formosa_Sanyo_1650mah[] = {
	
	10000, 4250, 8200, 4145, 5000, 3845,
	1800, 3735, 500, 3625, 0, 3405,
};
UINT32 m_parameter_PydTD_1520mah[] = {
	10000, 4100, 5500, 3826, 2000, 3739,
	500, 3665, 0, 3397,
};
UINT32 m_parameter_unknown_1650mah[] = {
	10000, 4250, 8100, 4145, 5100, 3845,
	2000, 3735, 900, 3625, 0, 3405,
};

static UINT32* m_param_tbl[] = {
	m_parameter_unknown_1650mah,
	m_parameter_TWS_SDI_1650mah,
	m_parameter_Formosa_Sanyo_1650mah,
	m_parameter_PydTD_1520mah
};

static UINT32 capacity[] = {1650, 1650, 1650, 1520};
static UINT32 pd_m_coef[] = {26, 22, 26, 26};
static UINT32 pd_m_resl[] = {100, 100, 100, 100};
static UINT32 pd_t_coef[] = {220, 100, 220, 220};
static INT32 padc[] = {100, 100, 100, 100};
static INT32 pw[] = {5, 5, 5, 5};

static UINT32* pd_m_coef_tbl[] = {pd_m_coef,};
static UINT32* pd_m_resl_tbl[] = {pd_m_resl,};
static UINT32 capacity_deduction_tbl_01p[] = {0,};

static struct battery_parameter g3u_battery_parameter = {
	.fl_25 = NULL,
	.pd_m_coef_tbl = pd_m_coef_tbl,
	.pd_m_coef_tbl_boot = pd_m_coef_tbl,
	.pd_m_resl_tbl = pd_m_resl_tbl,
	.pd_m_resl_tbl_boot = pd_m_resl_tbl,
	.pd_t_coef = pd_t_coef,
	.padc = padc,
	.pw = pw,
	.capacity_deduction_tbl_01p = capacity_deduction_tbl_01p,
	.id_tbl = NULL,
	.temp_index_tbl = NULL,
	.m_param_tbl = m_param_tbl,
	.m_param_tbl_size = sizeof(m_param_tbl)/sizeof(UINT32*),
	.capacity = capacity,
	.capacity_size = sizeof(capacity)/sizeof(UINT32),
};

static max17050_platform_data max17050_pdev_data = {
	.func_get_thermal_id = get_thermal_id,
	.func_get_battery_id = get_battery_id,
	.func_poweralg_config_init = g3u_poweralg_config_init,
	.func_update_charging_protect_flag = g3u_update_charging_protect_flag,
	.r2_kohm = 0,	
	.batt_param = &g3u_battery_parameter,
#ifdef CONFIG_TPS65200
	.func_kick_charger_ic = tps65200_kick_charger_ic,
#endif
};

static struct platform_device max17050_battery_pdev = {
	.name = "max17050-battery",
	.id = -1,
	.dev = {
		.platform_data = &max17050_pdev_data,
	},
};

static struct tps65200_platform_data tps65200_data = {
	.gpio_chg_int = MSM_GPIO_TO_INT(G3U_GPIO_CHG_INT),
	.gpio_chg_stat = MSM_GPIO_TO_INT(G3U_GPIO_CHG_STAT),
	.vdpm_idx = 3, 
};

static struct i2c_board_info i2c_tps65200_devices[] = {
	{
		I2C_BOARD_INFO("tps65200", 0xD4 >> 1),
		.platform_data = &tps65200_data,
	},
};

#ifdef CONFIG_SUPPORT_DQ_BATTERY
static int __init check_dq_setup(char *str)
{
	if (!strcmp(str, "PASS")) {
		tps65200_data.dq_result = 1;
		dq_result = 1;
	} else {
		tps65200_data.dq_result = 0;
		dq_result = 0;
	}

	return 1;
}
__setup("androidboot.dq=", check_dq_setup);
#endif

#if 0
static struct resource smc91x_resources[] = {
	[0] = {
		.start = 0x90000300,
		.end   = 0x900003ff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = MSM_GPIO_TO_INT(4),
		.end   = MSM_GPIO_TO_INT(4),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device smc91x_device = {
	.name           = "smc91x",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(smc91x_resources),
	.resource       = smc91x_resources,
};
#endif
#if 0
#ifdef CONFIG_SERIAL_MSM_HS
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup	= 1,
	.rx_to_inject		= 0xFD,
};
#endif
#endif


#ifdef CONFIG_SERIAL_MSM_HS
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup = 1,
	.rx_to_inject		= 0xFD,
};
#endif


static struct msm_pm_platform_data msm7x27a_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 16000,
					.residency = 20000,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 12000,
					.residency = 20000,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 1,
					.latency = 2000,
					.residency = 0,
	},
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 0,
	},
};

static struct bma250_platform_data gsensor_bma250_platform_data = {
	.intr = G3U_GPIO_GSENSORS_INT,
	.chip_layout = 0,
	.layouts = G3U_LAYOUTS,
};

static struct i2c_board_info i2c_bma250_devices[] = {
	{
		I2C_BOARD_INFO(BMA250_I2C_NAME_REMOVE_ECOMPASS, \
				0x32 >> 1),
		.platform_data = &gsensor_bma250_platform_data,
		.irq = MSM_GPIO_TO_INT(G3U_GPIO_GSENSORS_INT),
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_RESET_VECTOR_PHYS,
	.p_addr = 0,
};

static struct msm_pm_platform_data msm8625_pm_data[MSM_PM_SLEEP_MODE_NR * 2] = {
	
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 16000,
					.residency = 20000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 12000,
					.residency = 20000,
	},

	
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
					.idle_supported = 0,
					.suspend_supported = 0,
					.idle_enabled = 0,
					.suspend_enabled = 0,
					.latency = 500,
					.residency = 6000,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 10,
	},

	
	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 500,
					.residency = 6000,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 10,
	},

};

static struct msm_pm_boot_platform_data msm_pm_8625_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_REMAP_BOOT_ADDR,
	.v_addr = MSM_CFG_CTL_BASE,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
	.request_region = request_fmem_c_region,
	.release_region = release_fmem_c_region,
	.reusable = 1,
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static int __init pmem_mdp_size_setup(char *p)
{
	pmem_mdp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_mdp_size", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_adsp_size", pmem_adsp_size_setup);

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};
#ifdef CONFIG_BATTERY_MSM
static u32 msm_calculate_batt_capacity(u32 current_voltage);

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design     = 3200,
	.voltage_max_design     = 4200,
	.voltage_fail_safe      = 3340,
	.avail_chg_sources      = AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
	.calculate_capacity     = &msm_calculate_batt_capacity,
};

static u32 msm_calculate_batt_capacity(u32 current_voltage)
{
	u32 low_voltage	 = msm_psy_batt_data.voltage_min_design;
	u32 high_voltage = msm_psy_batt_data.voltage_max_design;

	if (current_voltage <= low_voltage)
		return 0;
	else if (current_voltage >= high_voltage)
		return 100;
	else
		return (current_voltage - low_voltage) * 100
			/ (high_voltage - low_voltage);
}

static struct platform_device msm_batt_device = {
	.name               = "msm-battery",
	.id                 = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};
#endif
#if 0
static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_HIGH,
	.irq_type	= SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags		= SMSC911X_USE_16BIT,
};

static struct resource smsc911x_resources[] = {
	[0] = {
		.start	= 0x90000000,
		.end	= 0x90007fff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= MSM_GPIO_TO_INT(48),
		.end	= MSM_GPIO_TO_INT(48),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct platform_device smsc911x_device = {
	.name		= "smsc911x",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smsc911x_resources),
	.resource	= smsc911x_resources,
	.dev		= {
		.platform_data	= &smsc911x_config,
	},
};

static struct msm_gpio smsc911x_gpios[] = {
	{ GPIO_CFG(48, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
							 "smsc911x_irq"  },
	{ GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
							 "eth_fifo_sel" },
};
#endif
static char *msm_adc_surf_device_names[] = {
	"XO_ADC",
};

static struct msm_adc_platform_data msm_adc_pdata = {
	.dev_names = msm_adc_surf_device_names,
	.num_adc = ARRAY_SIZE(msm_adc_surf_device_names),
	.target_hw = MSM_8x25,
};

static struct platform_device msm_adc_device = {
	.name   = "msm_adc",
	.id = -1,
	.dev = {
		.platform_data = &msm_adc_pdata,
	},
};
#if 0
#define ETH_FIFO_SEL_GPIO	49
static void msm7x27a_cfg_smsc911x(void)
{
	int res;

	res = msm_gpios_request_enable(smsc911x_gpios,
				 ARRAY_SIZE(smsc911x_gpios));
	if (res) {
		pr_err("%s: unable to enable gpios for SMSC911x\n", __func__);
		return;
	}

	
	res = gpio_direction_output(ETH_FIFO_SEL_GPIO, 0);
	if (res) {
		pr_err("%s: unable to get direction for gpio %d\n", __func__,
							 ETH_FIFO_SEL_GPIO);
		msm_gpios_disable_free(smsc911x_gpios,
						 ARRAY_SIZE(smsc911x_gpios));
		return;
	}
	gpio_set_value(ETH_FIFO_SEL_GPIO, 0);
}
#endif
#if 0

#if defined(CONFIG_SERIAL_MSM_HSL_CONSOLE) \
		&& defined(CONFIG_MSM_SHARED_GPIO_FOR_UART2DM)
static struct msm_gpio uart2dm_gpios[] = {
	{GPIO_CFG(19, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rfr_n" },
	{GPIO_CFG(20, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_cts_n" },
	{GPIO_CFG(21, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rx"    },
	{GPIO_CFG(108, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_tx"    },
};

static void msm7x27a_cfg_uart2dm_serial(void)
{
	int ret;
	ret = msm_gpios_request_enable(uart2dm_gpios,
					ARRAY_SIZE(uart2dm_gpios));
	if (ret)
		pr_err("%s: unable to enable gpios for uart2dm\n", __func__);
}
#else
static void msm7x27a_cfg_uart2dm_serial(void) { }
#endif
#endif
static struct fmem_platform_data fmem_pdata;

static struct platform_device fmem_device = {
	.name = "fmem",
	.id = 1,
	.dev = { .platform_data = &fmem_pdata },
};

__maybe_unused static struct platform_device *rumi_sim_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&msm_device_uart1,
	&msm_device_nand,
#if 0
	&msm_device_uart_dm1,
#endif
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
};

__maybe_unused static struct platform_device *msm8625_rumi3_devices[] __initdata = {
	&msm8625_device_dmov,
	&msm8625_device_smd,
	&msm8625_device_uart1,
	&msm8625_gsbi0_qup_i2c_device,
};

#define PM8058ADC_16BIT(adc) ((adc * 1800) / 65535) 
int64_t g3u_get_usbid_adc(void)
{
	uint32_t adc_value = 0xffffffff;
#if 1
	htc_get_usb_accessory_adc_level(&adc_value);
#endif
	adc_value = PM8058ADC_16BIT(adc_value);
	return adc_value;
}

static uint32_t usb_ID_PIN_input_table[] = {
	GPIO_CFG(G3U_GPIO_USB_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};

static uint32_t usb_ID_PIN_ouput_table[] = {
	GPIO_CFG(G3U_GPIO_USB_ID, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),
};


static void config_usb_id_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[CAM] %s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

void config_g3u_usb_id_gpios(bool output)
{
	if (output) {
		config_usb_id_table(usb_ID_PIN_ouput_table, ARRAY_SIZE(usb_ID_PIN_ouput_table));
		gpio_set_value(G3U_GPIO_USB_ID, 1);
		printk(KERN_INFO "%s %d output high\n",  __func__, G3U_GPIO_USB_ID);
	} else {
		config_usb_id_table(usb_ID_PIN_input_table, ARRAY_SIZE(usb_ID_PIN_input_table));
		printk(KERN_INFO "%s %d input none pull\n",  __func__, G3U_GPIO_USB_ID);
	}
}

static struct cable_detect_platform_data cable_detect_pdata = {
	.detect_type = CABLE_TYPE_PMIC_ADC,
	.usb_id_pin_gpio = G3U_GPIO_USB_ID,
	.config_usb_id_gpios = config_g3u_usb_id_gpios,
	.get_adc_cb		= g3u_get_usbid_adc,
};

static struct platform_device cable_detect_device = {
	.name	= "cable_detect",
	.id	= -1,
	.dev	= {
		.platform_data = &cable_detect_pdata,
	},
};
static struct resource ram_console_resources[] = {
	{
		.start  = MSM_RAM_CONSOLE_BASE,
		.end    = MSM_RAM_CONSOLE_BASE + MSM_RAM_CONSOLE_SIZE - 1,
		.flags  = IORESOURCE_MEM,
	},
};

static struct platform_device ram_console_device = {
	.name           = "ram_console",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(ram_console_resources),
	.resource       = ram_console_resources,
};

#ifdef CONFIG_LEDS_GPIO
static struct gpio_led gpio_exp_leds_config[] = {
        {
                .name = "button-backlight",
                .gpio = G3U_GPIO_AP_LED_EN,
                .active_low = 0,
                .retain_state_suspended = 0,
                .default_state = LEDS_GPIO_DEFSTATE_OFF,
        },
};

static struct gpio_led_platform_data gpio_leds_pdata = {
        .num_leds = ARRAY_SIZE(gpio_exp_leds_config),
        .leds = gpio_exp_leds_config,
};

static struct platform_device gpio_leds = {
        .name             = "leds-gpio",
        .id                     = -1,
        .dev               = {
                .platform_data = &gpio_leds_pdata,
        },
};
#endif 
#ifdef CONFIG_LEDS_PM8029
static struct pm8029_led_config pm_led_config[] = {
        {
                .name = "green",
                .bank = PMIC8029_GPIO1,
                .init_pwm_brightness = 255,
                .flag = FIX_BRIGHTNESS,
        },
        {
                .name = "amber",
                .bank = PMIC8029_GPIO2,
                .init_pwm_brightness = 240,
                .flag = FIX_BRIGHTNESS,
        },
};

static struct pm8029_led_platform_data pm8029_leds_data = {
        .led_config = pm_led_config,
        .num_leds = ARRAY_SIZE(pm_led_config),
};

static struct platform_device pm8029_leds = {
        .name   = "leds-pm8029",
        .id     = -1,
        .dev    = {
                .platform_data  = &pm8029_leds_data,
        },
};
#endif 

static struct htc_headset_gpio_platform_data htc_headset_gpio_data = {
	.hpin_gpio		= G3U_AUD_HP_INz,
	.key_enable_gpio	= 0,
	.mic_select_gpio	= 0,
};

static struct platform_device htc_headset_gpio = {
	.name	= "HTC_HEADSET_GPIO",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_gpio_data,
	},
};

static struct htc_headset_pmic_platform_data htc_headset_pmic_data = {
	.driver_flag		= DRIVER_HS_PMIC_RPC_KEY,
	.hpin_gpio		= 0,
	.hpin_irq		= 0,
	.key_gpio		= G3U_AUD_REMO_PRESz,
	.key_irq		= 0,
	.key_enable_gpio	= 0,
	.adc_mic		= 0,
	.adc_remote		= {0, 2057, 2058, 5332, 5333, 12360},
	.hs_controller		= 0,
	.hs_switch		= 0,
};

static struct platform_device htc_headset_pmic = {
	.name	= "HTC_HEADSET_PMIC",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_pmic_data,
	},
};

static struct htc_headset_1wire_platform_data htc_headset_1wire_data = {
	.tx_level_shift_en      = G3U_AUD_UART_OEz,
	.uart_sw                = G3U_AUD_UART_SEL,
	.remote_press	       = 0,
	.one_wire_remote        ={0x7E, 0x7F, 0x7D, 0x7F, 0x7B, 0x7F},
	.onewire_tty_dev	= "/dev/ttyMSM0",
};

static struct platform_device htc_headset_one_wire = {
       .name   = "HTC_HEADSET_1WIRE",
       .id     = -1,
       .dev    = {
               .platform_data  = &htc_headset_1wire_data,
       },
};

static struct platform_device *headset_devices[] = {
	&htc_headset_pmic,
	&htc_headset_gpio,
	&htc_headset_one_wire,
	
};

static struct headset_adc_config htc_headset_mgr_config[] = {
	{
		.type = HEADSET_MIC,
		.adc_max = 55723,
		.adc_min = 44510,
	},
	{
		.type = HEADSET_BEATS,
		.adc_max = 44509,
		.adc_min = 33333,
	},
	{
		.type = HEADSET_BEATS_SOLO,
		.adc_max = 33332,
		.adc_min = 20590,
	},
	{
		.type = HEADSET_MIC,
		.adc_max = 20589,
		.adc_min = 9285,
	},
	{
		.type = HEADSET_NO_MIC,
		.adc_max = 9284,
		.adc_min = 0,
	},
};

static uint32_t headset_cpu_gpio[] = {
	GPIO_CFG(G3U_AUD_UART_OEz, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_HP_INz, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_REMO_PRESz, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_2V85_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_UART_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static uint32_t headset_1wire_gpio[] = {
	GPIO_CFG(G3U_AUD_UART_RX, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_UART_TX, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_UART_RX, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	GPIO_CFG(G3U_AUD_UART_TX, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
 };

static void uart_tx_gpo(int mode)
{
	switch (mode) {
		case 0:
			gpio_tlmm_config(headset_1wire_gpio[1], GPIO_CFG_ENABLE);
			gpio_set_value_cansleep(G3U_AUD_UART_TX, 0);
			pr_info("[HS_BOARD]UART TX GPO 0\n");
			break;
		case 1:
			gpio_tlmm_config(headset_1wire_gpio[1], GPIO_CFG_ENABLE);
			gpio_set_value_cansleep(G3U_AUD_UART_TX, 1);
			pr_info("[HS_BOARD]UART TX GPO 1\n");
			break;
		case 2:
			gpio_tlmm_config(headset_1wire_gpio[3], GPIO_CFG_ENABLE);
			pr_info("[HS_BOARD]UART TX alternative\n");
			break;
	}
}

static void uart_lv_shift_en(int enable)
{
	gpio_set_value_cansleep(G3U_AUD_UART_OEz, enable);
	pr_info("[HS_BOARD]level shift %d\n", enable);
}

static void headset_init(void)
{
	int i,ret;
	for (i = 0; i < ARRAY_SIZE(headset_cpu_gpio); i++) {
		ret = gpio_tlmm_config(headset_cpu_gpio[i], GPIO_CFG_ENABLE);
		pr_info("[HS_BOARD]Config gpio[%d], ret = %d\n", (headset_cpu_gpio[i] & 0x3FF0) >> 4, ret);
	}
	gpio_set_value(G3U_AUD_UART_OEz, 1); 
	gpio_set_value(G3U_AUD_2V85_EN, 0);
	gpio_set_value(G3U_AUD_UART_SEL, 0);
}

static void headset_power(int hs_enable)
{
	gpio_set_value(G3U_AUD_UART_OEz, 1);
	gpio_set_value(G3U_AUD_2V85_EN, hs_enable);
}

static struct htc_headset_mgr_platform_data htc_headset_mgr_data = {
	.driver_flag		= DRIVER_HS_MGR_FLOAT_DET,
	.headset_devices_num	= ARRAY_SIZE(headset_devices),
	.headset_devices	= headset_devices,
	.headset_config_num	= ARRAY_SIZE(htc_headset_mgr_config),
	.headset_config		= htc_headset_mgr_config,
	.headset_power		= headset_power,
	.headset_init		= headset_init,
	.uart_tx_gpo		= uart_tx_gpo,
	.uart_lv_shift_en	= uart_lv_shift_en,
};

static struct platform_device htc_headset_mgr = {
	.name	= "HTC_HEADSET_MGR",
	.id	= -1,
	.dev	= {
		.platform_data	= &htc_headset_mgr_data,
	},
};



static struct platform_device *msm7627a_surf_ffa_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&msm_device_uart1,
#if 0
	&msm_device_uart_dm1,
#endif
	&msm_device_uart_dm2,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
	&msm_kgsl_3d0,
};

static struct platform_device *common_devices[] __initdata = {
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device,
	&fmem_device,
	
	
	&msm_device_snd,
	&msm_device_cad,
	&msm_device_adspdec,
	&asoc_msm_pcm,
	&asoc_msm_dai0,
	&asoc_msm_dai1,
#ifdef CONFIG_BATTERY_MSM
	&msm_batt_device,
#endif
	&msm_adc_device,
#ifdef CONFIG_LEDS_PM8029
	&pm8029_leds,
#endif 
#ifdef CONFIG_LEDS_GPIO
	&gpio_leds,
#endif 
	&htc_headset_mgr,
	&cable_detect_device,
#ifdef CONFIG_ION_MSM
	&ion_dev,
#endif
#ifdef CONFIG_PERFLOCK
	&msm7x27a_device_perf_lock,
#endif
	&htc_battery_pdev,
	&max17050_battery_pdev,
};

static struct platform_device *msm8625_surf_devices[] __initdata = {
	&ram_console_device,
	&msm8625_device_dmov,
	&msm8625_device_uart1,
#if 0
	&msm8625_device_uart_dm1,
#endif
	&msm8625_device_uart_dm2,
	&msm8625_gsbi0_qup_i2c_device,
	&msm8625_gsbi1_qup_i2c_device,
	&msm8625_device_smd,
	&msm8625_device_otg,
	&msm8625_kgsl_3d0,
};

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;
static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);

static void fix_sizes(void)
{
#if 0
	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		pmem_mdp_size = MSM7x25A_MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = MSM7x25A_MSM_PMEM_ADSP_SIZE;
	} else {
		pmem_mdp_size = MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
	}
#else
	pmem_mdp_size = MSM_PMEM_MDP_SIZE;
	pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
#endif
	if (get_ddr_size() > SZ_512M)
		pmem_adsp_size = CAMERA_ZSL_SIZE;
#ifdef CONFIG_ION_MSM
	msm_ion_camera_size = pmem_adsp_size;
	msm_ion_audio_size = (MSM_PMEM_AUDIO_SIZE + PMEM_KERNEL_EBI1_SIZE);
	msm_ion_sf_size = pmem_mdp_size;
#endif
}

#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_co_heap_pdata co_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
};
#endif

struct ion_platform_heap msm7x27a_heaps[] = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
		
		{
			.id	= ION_CAMERA_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_CAMERA_HEAP_NAME,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *)&co_ion_pdata,
		},
		
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *)&co_ion_pdata,
		},
		
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *)&co_ion_pdata,
		},
#endif
};

static struct ion_platform_data ion_pdata = {
	.nr = MSM_ION_HEAP_NUM,
	.has_outer_cache = 1,
	.heaps = msm7x27a_heaps,
};

static struct platform_device ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &ion_pdata },
};
#endif

static struct memtype_reserve msm7x27a_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data *pmem_pdata_array[] __initdata = {
		&android_pmem_adsp_pdata,
		&android_pmem_audio_pdata,
		&android_pmem_pdata,
};
#endif
#endif

static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	unsigned int i;
	unsigned int reusable_count = 0;

	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_pdata.size = pmem_mdp_size;
	android_pmem_audio_pdata.size = pmem_audio_size;

	fmem_pdata.size = 0;
	fmem_pdata.align = PAGE_SIZE;

	for (i = 0; i < ARRAY_SIZE(pmem_pdata_array); ++i) {
		struct android_pmem_platform_data *pdata = pmem_pdata_array[i];

		if (!reusable_count && pdata->reusable)
			fmem_pdata.size += pdata->size;

		reusable_count += (pdata->reusable) ? 1 : 0;

		if (pdata->reusable && reusable_count > 1) {
			pr_err("%s: Too many PMEM devices specified as reusable. PMEM device %s was not configured as reusable.\n",
				__func__, pdata->name);
			pdata->reusable = 0;
		}
	}
#endif
#endif
}

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm7x27a_reserve_table[p->memory_type].size += p->size;
}
#endif
#endif

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(pmem_pdata_array); ++i)
		reserve_memory_for(pmem_pdata_array[i]);

	msm7x27a_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif
#endif
}

static void __init size_ion_devices(void)
{
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
	ion_pdata.heaps[1].size = msm_ion_camera_size;
	ion_pdata.heaps[2].size = msm_ion_audio_size;
	ion_pdata.heaps[3].size = msm_ion_sf_size;
#endif
}

static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	msm7x27a_reserve_table[MEMTYPE_EBI1].size += msm_ion_camera_size;
	msm7x27a_reserve_table[MEMTYPE_EBI1].size += msm_ion_audio_size;
	msm7x27a_reserve_table[MEMTYPE_EBI1].size += msm_ion_sf_size;
#endif
}

static void __init msm7x27a_calculate_reserve_sizes(void)
{
	fix_sizes();
	size_pmem_devices();
	reserve_pmem_memory();
	size_ion_devices();
	reserve_ion_memory();
}

static int msm7x27a_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

static struct reserve_info msm7x27a_reserve_info __initdata = {
	.memtype_reserve_table = msm7x27a_reserve_table,
	.calculate_reserve_sizes = msm7x27a_calculate_reserve_sizes,
	.paddr_to_memtype = msm7x27a_paddr_to_memtype,
};

static void __init msm7x27a_reserve(void)
{
	reserve_info = &msm7x27a_reserve_info;
	msm_reserve();
}

static void __init msm8625_reserve(void)
{
	msm7x27a_reserve();
	memblock_remove(MSM8625_CPU_PHYS, SZ_8);
	memblock_remove(MSM8625_WARM_BOOT_PHYS, SZ_32);
	memblock_remove(MSM8625_NON_CACHE_MEM, SZ_2K);
}
#if 0
static void __init msm7x27a_device_i2c_init(void)
{
	msm_gsbi0_qup_i2c_device.dev.platform_data = &msm_gsbi0_qup_i2c_pdata;
	msm_gsbi1_qup_i2c_device.dev.platform_data = &msm_gsbi1_qup_i2c_pdata;
}
#endif
static void __init msm8625_device_i2c_init(void)
{
	msm8625_gsbi0_qup_i2c_device.dev.platform_data =
		&msm_gsbi0_qup_i2c_pdata;
	msm8625_gsbi1_qup_i2c_device.dev.platform_data =
		&msm_gsbi1_qup_i2c_pdata;
}
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_3K

static ssize_t syn_vkeys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, 200,
	__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":105:887:103:137"
	
	":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":384:887:103:137"
	
	"\n");
}

static struct kobj_attribute syn_vkeys_attr = {
	.attr = {
		.name = "virtualkeys.synaptics-rmi-touchscreen",
		.mode = S_IRUGO,
	},
	.show = &syn_vkeys_show,
};

static struct attribute *syn_properties_attrs[] = {
	&syn_vkeys_attr.attr,
	NULL
};

static struct attribute_group syn_properties_attr_group = {
	.attrs = syn_properties_attrs,
};
static int synaptic_rmi_tp_power(int on)
{
	return 0;
}

static struct synaptics_i2c_rmi_platform_data syn_ts_3k_data[] = {
	{
		.version = 0x3332,							
		.packrat_number = 1473052,
		.abs_x_min       = 0,
		.abs_x_max       = 712,
		.abs_y_min       = 7,
		.abs_y_max       = 1072,
		.display_width   = 480,
		.display_height  = 800,
		.gpio_irq        = MSM_TP_ATTz,
		.gpio_reset 		= MSM_TP_RSTz,
		.tw_pin_mask     = 0x0080,
		.sensor_id = 0x00 | SENSOR_ID_CHECKING_EN,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.psensor_detection = 1,
		.reduce_report_level = {0, 0, 0, 0, 0},
		.default_config = 1,
		.i2c_err_handler_en = 1,
		.config = {
			0x47, 0x33, 0x31, 0x31, 0x00, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0xD0,
			0x02, 0xB0, 0x04, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0x89, 0x20, 0xA7, 0x02, 0x01, 0x3C, 0x23, 0x01,
			0x26, 0x01, 0x9C, 0x58, 0x6F, 0x53, 0xAE, 0xE2,
			0xBA, 0xD8, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x04, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x0B, 0x13, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x96, 0x07, 0xF4, 0xC8,
			0xC0, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x1E, 0x05, 0x00, 0x02, 0x7C,
			0x01, 0x80, 0x03, 0x0E, 0x1F, 0x12, 0x35, 0x00,
			0x13, 0x04, 0x1B, 0x00, 0x10, 0x0A, 0x60, 0x68,
			0x60, 0x68, 0x60, 0x68, 0x68, 0x60, 0x32, 0x32,
			0x31, 0x30, 0x2F, 0x2E, 0x2D, 0x2B, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xDC,
			0xFF, 0x00, 0x64, 0x00, 0xC8, 0x00, 0x80, 0x14,
			0xB3, 0x88, 0x13, 0x00, 0xC0, 0x80, 0x02, 0x02,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x55, 0x58,
			0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x65, 0x00, 0x96,
			0x00, 0x64, 0xC8, 0x00, 0x00, 0x00, 0x02, 0x04,
			0x06, 0x08, 0x0A, 0x0C, 0x0D, 0x00, 0x31, 0x04,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x01, 0x17, 0x15, 0x18, 0x16, 0x19, 0x13, 0x1B,
			0x12, 0x1A, 0x14, 0x11, 0xFF, 0xFF, 0xFF, 0x09,
			0x0F, 0x08, 0x0A, 0x0D, 0x11, 0x13, 0x10, 0x01,
			0x0C, 0x04, 0x05, 0x12, 0x0B, 0x0E, 0x07, 0x06,
			0x02, 0x03, 0xFF, 0x00, 0x10, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x86, 0x80, 0x00, 0x0F, 0x00,
			0x02, 0x36, 0x44, 0x04, 0xA4, 0x10, 0x20, 0x00,
			0x98, 0x07, 0x46, 0x03, 0x06,
		},
	},
	{
		.version = 0x3332,							
		.packrat_number = 1473052,
		.abs_x_min       = 0,
		.abs_x_max       = 712,
		.abs_y_min       = 7,
		.abs_y_max       = 1072,
		.display_width   = 480,
		.display_height  = 800,
		.gpio_irq        = MSM_TP_ATTz,
		.gpio_reset 		= MSM_TP_RSTz,
		.tw_pin_mask     = 0x0080,
		.sensor_id = 0x80 | SENSOR_ID_CHECKING_EN,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.psensor_detection = 1,
		.reduce_report_level = {0, 0, 0, 0, 0},
		.default_config = 1,
		.i2c_err_handler_en = 1,
		.config = {
			0x47, 0x33, 0x31, 0x31, 0x00, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0xD0,
			0x02, 0xB0, 0x04, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0x89, 0x20, 0xA7, 0x02, 0x01, 0x3C, 0x23, 0x01,
			0x26, 0x01, 0x9C, 0x58, 0x6F, 0x53, 0xAE, 0xE2,
			0xBA, 0xD8, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x04, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x0B, 0x13, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x96, 0x07, 0xF4, 0xC8,
			0xC0, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x1E, 0x05, 0x00, 0x02, 0x7C,
			0x01, 0x80, 0x03, 0x0E, 0x1F, 0x12, 0x35, 0x00,
			0x13, 0x04, 0x1B, 0x00, 0x10, 0x0A, 0x60, 0x68,
			0x60, 0x68, 0x60, 0x68, 0x68, 0x60, 0x32, 0x32,
			0x31, 0x30, 0x2F, 0x2E, 0x2D, 0x2B, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xDC,
			0xFF, 0x00, 0x64, 0x00, 0xC8, 0x00, 0x80, 0x14,
			0xB3, 0x88, 0x13, 0x00, 0xC0, 0x80, 0x02, 0x02,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x55, 0x58,
			0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x65, 0x00, 0x96,
			0x00, 0x64, 0xC8, 0x00, 0x00, 0x00, 0x02, 0x04,
			0x06, 0x08, 0x0A, 0x0C, 0x0D, 0x00, 0x31, 0x04,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x01, 0x17, 0x15, 0x18, 0x16, 0x19, 0x13, 0x1B,
			0x12, 0x1A, 0x14, 0x11, 0xFF, 0xFF, 0xFF, 0x09,
			0x0F, 0x08, 0x0A, 0x0D, 0x11, 0x13, 0x10, 0x01,
			0x0C, 0x04, 0x05, 0x12, 0x0B, 0x0E, 0x07, 0x06,
			0x02, 0x03, 0xFF, 0x00, 0x10, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x86, 0x80, 0x00, 0x0F, 0x00,
			0x02, 0x36, 0x44, 0x04, 0xA4, 0x10, 0x20, 0x00,
			0x98, 0x07, 0x46, 0x03, 0x06,
		},
	},
	{
		.packrat_number  = 1293984,						
		.abs_x_min       = 0,
		.abs_x_max       = 720,
		.abs_y_min       = 0,
		.abs_y_max       = 1100,
		.display_width   = 480,
		.display_height  = 800,
		.gpio_irq        = MSM_TP_ATTz,
		.gpio_reset 		= MSM_TP_RSTz,
		.default_config  = 1,
		.tw_pin_mask     = 0x0080,
		.sensor_id = 0x00 | SENSOR_ID_CHECKING_EN,
		.report_type     = SYN_AND_REPORT_TYPE_B,
		.psensor_detection = 1,
		.reduce_report_level = {65, 65, 50, 0, 0},
		.default_config = 1,
		.i2c_err_handler_en = 1,
		.config = {
			0x43, 0x41, 0x30, 0x35, 0x00, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0xD0,
			0x02, 0xB0, 0x04, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0xE5, 0x11, 0x07, 0x01, 0x01, 0x3C, 0x16, 0x03,
			0x18, 0x03, 0x0A, 0x57, 0x00, 0x54, 0xCD, 0xBB,
			0x4F, 0xBB, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x04, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x0B, 0x13, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x64, 0x07, 0xF4, 0x96,
			0xD2, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x1E, 0x05, 0x00, 0x02, 0x5E,
			0x01, 0x80, 0x03, 0x0E, 0x1F, 0x12, 0x36, 0x00,
			0x13, 0x04, 0x1B, 0x00, 0x10, 0xC8, 0x60, 0x68,
			0x60, 0x68, 0x60, 0x68, 0x60, 0x68, 0x32, 0x31,
			0x30, 0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x88,
			0x13, 0x00, 0x64, 0x00, 0xC8, 0x00, 0x80, 0x0A,
			0x80, 0xB8, 0x0B, 0x00, 0xC0, 0x80, 0x02, 0x02,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x56, 0x59,
			0x5B, 0x5D, 0x5F, 0x61, 0x63, 0x66, 0x00, 0x8C,
			0x00, 0x64, 0xC8, 0x00, 0x00, 0x00, 0x02, 0x04,
			0x06, 0x08, 0x0A, 0x0C, 0x0D, 0x00, 0x31, 0x04,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x01, 0x17, 0x15, 0x18, 0x16, 0x19, 0x13, 0x1B,
			0x12, 0x1A, 0x14, 0x11, 0xFF, 0xFF, 0xFF, 0x09,
			0x0F, 0x08, 0x0A, 0x0D, 0x11, 0x13, 0x10, 0x01,
			0x0C, 0x04, 0x05, 0x12, 0x0B, 0x0E, 0x07, 0x06,
			0x02, 0x03, 0xFF, 0x00, 0x10, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x0A, 0x00,
			0x4C, 0x53,
		},
	},
	{
		.packrat_number  = 1293984,						
		.abs_x_min       = 0,
		.abs_x_max       = 720,
		.abs_y_min       = 0,
		.abs_y_max       = 1100,
		.display_width   = 480,
		.display_height  = 800,
		.gpio_irq        = MSM_TP_ATTz,
		.gpio_reset 		= MSM_TP_RSTz,
		.default_config  = 1,
		.tw_pin_mask     = 0x0080,
		.sensor_id = 0x80 | SENSOR_ID_CHECKING_EN,
		.report_type     = SYN_AND_REPORT_TYPE_B,
		.psensor_detection = 1,
		.reduce_report_level = {65, 65, 50, 0, 0},
		.default_config = 1,
		.i2c_err_handler_en = 1,
		.config = {
			0x43, 0x41, 0x30, 0x35, 0x00, 0x7F, 0x03, 0x1E,
			0x05, 0x09, 0x00, 0x01, 0x01, 0x00, 0x10, 0xD0,
			0x02, 0xB0, 0x04, 0x02, 0x14, 0x1E, 0x05, 0x50,
			0xE5, 0x11, 0x07, 0x01, 0x01, 0x3C, 0x16, 0x03,
			0x18, 0x03, 0x0A, 0x57, 0x00, 0x54, 0xCD, 0xBB,
			0x4F, 0xBB, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x04, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x19, 0x01, 0x00, 0x0A, 0x0B, 0x13, 0x0A,
			0x00, 0x14, 0x0A, 0x40, 0x64, 0x07, 0xF4, 0x96,
			0xD2, 0x43, 0x2A, 0x05, 0x00, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x3C, 0x32, 0x00, 0x00, 0x00,
			0x4C, 0x6C, 0x74, 0x1E, 0x05, 0x00, 0x02, 0x5E,
			0x01, 0x80, 0x03, 0x0E, 0x1F, 0x12, 0x36, 0x00,
			0x13, 0x04, 0x1B, 0x00, 0x10, 0xC8, 0x60, 0x68,
			0x60, 0x68, 0x60, 0x68, 0x60, 0x68, 0x32, 0x31,
			0x30, 0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x88,
			0x13, 0x00, 0x64, 0x00, 0xC8, 0x00, 0x80, 0x0A,
			0x80, 0xB8, 0x0B, 0x00, 0xC0, 0x80, 0x02, 0x02,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x56, 0x59,
			0x5B, 0x5D, 0x5F, 0x61, 0x63, 0x66, 0x00, 0x8C,
			0x00, 0x64, 0xC8, 0x00, 0x00, 0x00, 0x02, 0x04,
			0x06, 0x08, 0x0A, 0x0C, 0x0D, 0x00, 0x31, 0x04,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x51, 0x51, 0x51,
			0x51, 0x51, 0x51, 0x51, 0x51, 0xCD, 0x0D, 0x04,
			0x01, 0x17, 0x15, 0x18, 0x16, 0x19, 0x13, 0x1B,
			0x12, 0x1A, 0x14, 0x11, 0xFF, 0xFF, 0xFF, 0x09,
			0x0F, 0x08, 0x0A, 0x0D, 0x11, 0x13, 0x10, 0x01,
			0x0C, 0x04, 0x05, 0x12, 0x0B, 0x0E, 0x07, 0x06,
			0x02, 0x03, 0xFF, 0x00, 0x10, 0x00, 0x10, 0x00,
			0x10, 0x00, 0x10, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x0A, 0x00,
			0x4C, 0x53,
		},
	},
	{
		.version = 0x3330,
		.packrat_number = 974353,
		.abs_x_min = 0,
		.abs_x_max = 1100,
		.abs_y_min = 0,
		.abs_y_max = 1770,
		.display_width = 544,
		.display_height = 960,
		.flags = SYNAPTICS_FLIP_X,
		.gpio_irq = MSM_TP_ATTz,
		.large_obj_check = 1,
		.tw_pin_mask = 0x0080,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.segmentation_bef_unlock = 0x50,
		.config = {
			0x32, 0x30, 0x30, 0x31, 0x04, 0x0F, 0x03, 0x1E,
			0x05, 0x20, 0xB1, 0x00, 0x0B, 0x19, 0x19, 0x00,
			0x00, 0x4C, 0x04, 0x6C, 0x07, 0x1E, 0x05, 0x28,
			0xF5, 0x28, 0x1E, 0x05, 0x01, 0x30, 0x00, 0x30,
			0x00, 0x00, 0x48, 0x00, 0x48, 0x44, 0xA0, 0xD3,
			0xA1, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0A,
			0x04, 0xC0, 0x00, 0x02, 0xA1, 0x01, 0x80, 0x02,
			0x0D, 0x1E, 0x00, 0x8C, 0x00, 0x19, 0x04, 0x1E,
			0x00, 0x10, 0x0A, 0x01, 0x11, 0x14, 0x1A, 0x12,
			0x1B, 0x13, 0x19, 0x16, 0x18, 0x15, 0x17, 0xFF,
			0xFF, 0xFF, 0x09, 0x0F, 0x08, 0x0A, 0x0D, 0x11,
			0x13, 0x10, 0x01, 0x0C, 0x04, 0x05, 0x12, 0x0B,
			0x0E, 0x07, 0x06, 0x02, 0x03, 0xFF, 0x40, 0x40,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x24, 0x23,
			0x21, 0x20, 0x1F, 0x1D, 0x1C, 0x1A, 0x00, 0x07,
			0x0F, 0x18, 0x21, 0x2B, 0x37, 0x43, 0x00, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0x00, 0xFF, 0xFF, 0x00, 0xC0, 0x80, 0x00, 0x10,
			0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			0x80, 0x80, 0x02, 0x02, 0x02, 0x07, 0x02, 0x03,
			0x09, 0x03, 0x10, 0x10, 0x10, 0x40, 0x10, 0x10,
			0x40, 0x10, 0x59, 0x5D, 0x61, 0x74, 0x6A, 0x4A,
			0x68, 0x52, 0x30, 0x30, 0x00, 0x1E, 0x19, 0x05,
			0x00, 0x00, 0x3D, 0x08
		}
	},
	{
		.version = 0x3330,
		.packrat_number = 7788,
		.abs_x_min = 0,
		.abs_x_max = 1100,
		.abs_y_min = 0,
		.abs_y_max = 1770,
		.display_width = 544,
		.display_height = 960,
		.flags = SYNAPTICS_FLIP_X,
		.gpio_irq = MSM_TP_ATTz,
		.large_obj_check = 1,
		.tw_pin_mask = 0x0080,
		.report_type = SYN_AND_REPORT_TYPE_B,
		.segmentation_bef_unlock = 0x50,
		.power = synaptic_rmi_tp_power,
	},
};

static struct i2c_board_info i2c_touch_device[] = {
	{
		I2C_BOARD_INFO(SYNAPTICS_3200_NAME, 0x40 >> 1),
		.platform_data = &syn_ts_3k_data,
		.irq = MSM_GPIO_TO_INT(MSM_TP_ATTz)
	},
};

static void syn_init_vkeys_8x25(void)
{
	int rc = 0;
	static struct kobject *syn_properties_kobj;

	syn_properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (syn_properties_kobj)
		rc = sysfs_create_group(syn_properties_kobj, &syn_properties_attr_group);
	if (!syn_properties_kobj || rc)
		pr_err("%s: failed to create board_properties\n", __func__);
	else {
		for (rc = 0; rc < ARRAY_SIZE(syn_ts_3k_data); rc++) {
			syn_ts_3k_data[rc].vk_obj = syn_properties_kobj;
			syn_ts_3k_data[rc].vk2Use = &syn_vkeys_attr;
		}
	}

	return;
}

#endif

int msm8625_init_keypad(void);

#ifdef CONFIG_FLASHLIGHT_TPS61310
#ifdef CONFIG_MSM_CAMERA_FLASH
int flashlight_control(int mode)
{
	int	rc;
	
	static int	backlight_off = 0;

	if (mode != FL_MODE_PRE_FLASH && mode != FL_MODE_OFF) {
		led_brightness_switch("lcd-backlight", FALSE);
		backlight_off = 1;
	}

	rc = tps61310_flashlight_control(mode);

	if(mode == FL_MODE_PRE_FLASH || mode == FL_MODE_OFF) {
		if(backlight_off) {
			led_brightness_switch("lcd-backlight", TRUE);
			backlight_off = 0;
		}
	}

	return rc;
}
#endif

static void config_flashlight_gpios(void)
{
	static uint32_t flashlight_gpio_table[] = {
		GPIO_CFG(G3U_GPIO_FLASH_ENABLE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
		GPIO_CFG(G3U_GPIO_FLASH_SWITCH, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	};

	gpio_tlmm_config(flashlight_gpio_table[0], GPIO_CFG_ENABLE);
	gpio_tlmm_config(flashlight_gpio_table[1], GPIO_CFG_ENABLE);
}

static struct TPS61310_flashlight_platform_data flashlight_data = {
	.gpio_init = config_flashlight_gpios,
	.tps61310_strb0 = G3U_GPIO_FLASH_ENABLE,
	.tps61310_strb1 = G3U_GPIO_FLASH_SWITCH,
	.led_count = 1,
	.flash_duration_ms = 600,
};

static struct i2c_board_info i2c_tps61310_flashlight[] = {
	{
		I2C_BOARD_INFO("TPS61310_FLASHLIGHT", 0x66 >> 1),
		.platform_data = &flashlight_data,
	},
};
#endif

#define MSM_EBI2_PHYS			0xa0d00000
#define MSM_EBI2_XMEM_CS2_CFG1		0xa0d10030

static void __init msm7x27a_init_ebi2(void)
{
	uint32_t ebi2_cfg;
	void __iomem *ebi2_cfg_ptr;

	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_PHYS, sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (machine_is_msm7x27a_rumi3() || machine_is_msm7x27a_surf() ||
		machine_is_msm7625a_surf() || machine_is_msm8625_surf() || machine_is_g3u() || machine_is_protodug() || machine_is_magnids() || machine_is_protodcg())
		ebi2_cfg |= (1 << 4); 

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);

	
	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_XMEM_CS2_CFG1,
							 sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (machine_is_msm7x27a_surf() || machine_is_msm7625a_surf())
		ebi2_cfg |= (1 << 31);

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);
}

static struct platform_device msm_proccomm_regulator_dev = {
	.name   = PROCCOMM_REGULATOR_DEV_NAME,
	.id     = -1,
	.dev    = {
		.platform_data = &msm7x27a_proccomm_regulator_data
	}
};

static void msm_adsp_add_pdev(void)
{
	int rc = 0;
	struct rpc_board_dev *rpc_adsp_pdev;

	rpc_adsp_pdev = kzalloc(sizeof(struct rpc_board_dev), GFP_KERNEL);
	if (rpc_adsp_pdev == NULL) {
		pr_err("%s: Memory Allocation failure\n", __func__);
		return;
	}
	rpc_adsp_pdev->prog = ADSP_RPC_PROG;

	if (cpu_is_msm8625())
		rpc_adsp_pdev->pdev = msm8625_device_adsp;
	else
		rpc_adsp_pdev->pdev = msm_adsp_device;
	rc = msm_rpc_add_board_dev(rpc_adsp_pdev, 1);
	if (rc < 0) {
		pr_err("%s: return val: %d\n",	__func__, rc);
		kfree(rpc_adsp_pdev);
	}
}

#if 0 
#define UART1DM_RX_GPIO		45
#endif

static int __init msm_qrd_init_ar6000pm(void)
{
	msm_wlan_ar6000_pm_device.dev.platform_data = &ar600x_wlan_power;
	return platform_device_register(&msm_wlan_ar6000_pm_device);
}

static void __init msm7x27a_init_regulators(void)
{
	int rc = platform_device_register(&msm_proccomm_regulator_dev);
	if (rc)
		pr_err("%s: could not register regulator device: %d\n",
				__func__, rc);
}

static void __init msm7x27a_add_footswitch_devices(void)
{
	platform_add_devices(msm_footswitch_devices,
			msm_num_footswitch_devices);
}

static void __init msm7x27a_add_platform_devices(void)
{
	if (machine_is_msm8625_surf() || machine_is_msm8625_ffa() || machine_is_g3u() || machine_is_protodug() || machine_is_magnids() || machine_is_protodcg()) {
		platform_add_devices(msm8625_surf_devices,
			ARRAY_SIZE(msm8625_surf_devices));
	} else {
		platform_add_devices(msm7627a_surf_ffa_devices,
			ARRAY_SIZE(msm7627a_surf_ffa_devices));
	}

	platform_add_devices(common_devices,
			ARRAY_SIZE(common_devices));
}
#if 0
static void __init msm7x27a_uartdm_config(void)
{
	msm7x27a_cfg_uart2dm_serial();
#if 0
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(UART1DM_RX_GPIO);
	if (cpu_is_msm8625())
		msm8625_device_uart_dm1.dev.platform_data =
			&msm_uart_dm1_pdata;
	else
		msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif
}

#endif
static void __init msm7x27a_otg_gadget(void)
{
	if (cpu_is_msm8625()) {
#if 1
		msm_otg_pdata.swfi_latency =
		msm8625_pm_data[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].latency;
#endif
		msm8625_device_otg.dev.platform_data = &msm_otg_pdata;
		msm8625_device_gadget_peripheral.dev.platform_data =
			&msm_gadget_pdata;
	} else {
#if 1
		msm_otg_pdata.swfi_latency =
		msm7x27a_pm_data[
		MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
#endif
		msm_device_otg.dev.platform_data = &msm_otg_pdata;
		msm_device_gadget_peripheral.dev.platform_data =
			&msm_gadget_pdata;
	}
}

static void __init msm7x27a_pm_init(void)
{
	if (machine_is_msm8625_surf() || machine_is_msm8625_ffa() || machine_is_g3u() || machine_is_protodug() || machine_is_magnids() || machine_is_protodcg()) {
		msm_pm_set_platform_data(msm8625_pm_data,
				ARRAY_SIZE(msm8625_pm_data));
		BUG_ON(msm_pm_boot_init(&msm_pm_8625_boot_pdata));
		msm8x25_spm_device_init();
		msm_pm_register_cpr_ops();
	} else {
		msm_pm_set_platform_data(msm7x27a_pm_data,
				ARRAY_SIZE(msm7x27a_pm_data));
		BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	}

	msm_pm_register_irqs();
}

static void proto_reset(void)
{
	gpio_set_value(G3U_GPIO_PS_HOLD, 0);
}
static void __init msm7x2x_init(void)
{
	struct proc_dir_entry *entry = NULL;
	int status;
	msm_hw_reset_hook = proto_reset;

	msm7x2x_misc_init();

	
	msm7x27a_init_regulators();
	msm_adsp_add_pdev();
	msm8625_device_i2c_init();
	msm7x27a_init_ebi2();
	

	msm7x27a_otg_gadget();

	msm7x27a_add_footswitch_devices();
	#ifdef CONFIG_HTC_HEADSET_ONE_WIRE
	if (get_kernel_flag() & 0x02)
	#endif
	{
		htc_headset_mgr_data.headset_devices_num--;
		htc_headset_mgr_data.headset_devices[2] = NULL;
	}
	msm7x27a_add_platform_devices();

#ifdef CONFIG_SERIAL_MSM_HS
	printk(KERN_INFO "bt config UART1DM \n");
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(UART1DM_RX_GPIO);
	msm8625_device_uart_dm1.name = "msm_serial_hs_qct"; 
	msm8625_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif

#ifdef CONFIG_BT
	bt_export_bd_address();
	platform_device_register(&msm8625_device_uart_dm1);
#endif

#ifdef CONFIG_PERFLOCK_SCREEN_POLICY
	
	perf_lock_init(&screen_off_ceiling_lock, TYPE_CPUFREQ_CEILING,
					PERF_LOCK_LOW, "screen_off_scaling_max");
	perflock_screen_policy_init(&msm7x27a_screen_off_policy);
#endif

	
	msm_qrd_init_ar6000pm();
	g3u_wifi_init();

#ifdef CONFIG_MMC_MSM
	printk(KERN_ERR "%s: start init mmc\n", __func__);
	msm7627a_init_mmc();
	printk(KERN_ERR "%s: msm7627a_init_mmc()\n", __func__);
	entry = create_proc_read_entry("emmc", 0, NULL, emmc_partition_read_proc, NULL);
	printk(KERN_ERR "%s: create_proc_read_entry()\n", __func__);
	if (!entry)
		printk(KERN_ERR"Create /proc/emmc failed!\n");

	entry = create_proc_read_entry("dying_processes", 0, NULL, dying_processors_read_proc, NULL);
	if (!entry)
		printk(KERN_ERR"Create /proc/dying_processes FAILED!\n");

#endif

	
	g3_init_panel();
	printk(KERN_ERR "%s: g3u_init_panel\n", __func__);

	msm7x2x_init_host();
	printk(KERN_ERR "%s: msm7x2x_init_host\n", __func__);
	msm7x27a_pm_init();
	printk(KERN_ERR "%s: msm7x27a_pm_init\n", __func__);
	register_i2c_devices();
	printk(KERN_ERR "%s: register_i2c_devices\n", __func__);

	msm7627a_bt_power_init();

	g3u_camera_init();

	
	
	
	
	msm8x25_kgsl_3d0_init();
	g3u_add_usb_devices();
#if defined(CONFIG_INPUT_CAPELLA_CM3629)
#define GPIO_PS_2V8 13


	status = gpio_request(GPIO_PS_2V8, "PS_2V85_EN");
	if (status) {
	pr_err("%s:Failed to request GPIO %d\n", __func__, GPIO_PS_2V8);
	} else {
		status = gpio_direction_output(GPIO_PS_2V8,1);
		pr_info("PS_2V85 status = %d\n",status);
		if (!status)
			status = gpio_get_value(GPIO_PS_2V8);
		gpio_free(GPIO_PS_2V8);
		pr_info("PS_2V85 status = %d\n",status);
	}

	status = gpio_request(G3U_GPIO_PROXIMITY_INT, "PS_INT");
	if (status) {
		pr_err("%s: Failed to request GPIO %d\n", __func__,G3U_GPIO_PROXIMITY_INT);
	} else {
		status = gpio_tlmm_config(GPIO_CFG(G3U_GPIO_PROXIMITY_INT, 0,
			GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
			GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		pr_info("G3U_GPIO_PROXIMITY_INT status = %d\n",status);
	gpio_free(G3U_GPIO_PROXIMITY_INT);
	}

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
		i2c_cm36282_devices, ARRAY_SIZE(i2c_cm36282_devices));

#endif

#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_3K
	
	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				i2c_touch_device,
				ARRAY_SIZE(i2c_touch_device));
#endif

#ifdef CONFIG_FLASHLIGHT_TPS61310
	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				i2c_tps61310_flashlight,
				ARRAY_SIZE(i2c_tps61310_flashlight));
#endif

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
			i2c_bma250_devices, ARRAY_SIZE(i2c_bma250_devices));

	#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_3K
		syn_init_vkeys_8x25();
	#endif

	
	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
			i2c_tps65200_devices, ARRAY_SIZE(i2c_tps65200_devices));
#ifdef CONFIG_MSM_RPC_VIBRATOR
	msm_init_pmic_vibrator(3000);
#endif
	msm8625_init_keypad();

	if (~get_kernel_flag() & KERNEL_FLAG_TEST_PWR_SUPPLY) {
		htc_monitor_init();
		htc_PM_monitor_init();
	}
}


static void __init g3u_fixup(struct tag *tags, char **cmdline, struct meminfo *mi)
{

	mi->nr_banks = 2;
	mi->bank[0].start = 0x03100000;
	mi->bank[0].size = 0x0CF00000;
	mi->bank[1].start = 0x20000000;

	
		mi->bank[1].size = 0x0FB9B000;
}

static void __init msm7x2x_init_early(void)
{
	
}

MACHINE_START(G3U, "g3u")
	.atag_offset	= PHYS_OFFSET + 0x100,
	.fixup = g3u_fixup,
	.map_io		= msm8625_map_io,
	.reserve	= msm8625_reserve,
	.init_irq	= msm8625_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END

MACHINE_START(PROTODUG, "protodug")
	.atag_offset	= PHYS_OFFSET + 0x100,
	.fixup = g3u_fixup,
	.map_io		= msm8625_map_io,
	.reserve	= msm8625_reserve,
	.init_irq	= msm8625_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= gic_handle_irq,
MACHINE_END

