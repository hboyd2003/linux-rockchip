/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 *
 */

#ifndef __MAXIM4C_DRV_H__
#define __MAXIM4C_DRV_H__

#include <linux/i2c.h>
#include <linux/i2c-mux.h>
#include <linux/videodev2.h>
#include <linux/workqueue.h>
#include <linux/regulator/consumer.h>
#include <linux/rk-camera-module.h>
#include <media/media-entity.h>
#include <media/v4l2-async.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-fwnode.h>

#include "maxim4c_i2c.h"
#include "maxim4c_link.h"
#include "maxim4c_video_pipe.h"
#include "maxim4c_mipi_txphy.h"
#include "maxim4c_pattern.h"

// max96712/max96722 chip id register and value
#define MAXIM4C_REG_CHIP_ID		0x0D
#define MAX96712_CHIP_ID		0xA0
#define MAX96722_CHIP_ID		0xA1

/* power supply numbers */
#define MAXIM4C_NUM_SUPPLIES		2

/* Private v4l2 event */
#define V4L2_EVENT_HOT_PLUG	\
		(V4L2_EVENT_PRIVATE_START + 0x10)

enum {
	MAXIM4C_HOT_PLUG_OUT = 0,
	MAXIM4C_HOT_PLUG_IN,
};

struct maxim4c_hot_plug_work {
	struct workqueue_struct *state_check_wq;
	struct delayed_work state_d_work;
	u32 hot_plug_state;
};

struct maxim4c_vc_info {
	u32 enable; // 0: disable, !0: enable

	u32 width;
	u32 height;
	u32 bus_fmt;

	/*
	 * the following are optional parameters, user-defined data types
	 *   default 0: invalid parameter
	 */
	u32 data_type;
	u32 data_bit;
};

struct maxim4c_mode {
	u32 width;
	u32 height;
	struct v4l2_fract max_fps;
	u32 hts_def;
	u32 vts_def;
	u32 exp_def;
	u32 link_freq_idx;
	u32 bus_fmt;
	u32 bpp;
	const struct regval *reg_list;
	u32 vc[PAD_MAX];
	struct maxim4c_vc_info vc_info[PAD_MAX];
	struct v4l2_rect crop_rect;
};

typedef struct maxim4c {
	struct i2c_client *client;
	struct maxim4c_i2c_mux i2c_mux;
	struct clk *xvclk;
	struct regulator_bulk_data supplies[MAXIM4C_NUM_SUPPLIES];
	struct regulator *pwdn_regulator;
	struct gpio_desc *lock_gpio;

	struct mutex mutex;

	struct v4l2_subdev subdev;
	struct media_pad pad;
	struct v4l2_ctrl_handler ctrl_handler;
	struct v4l2_ctrl *exposure;
	struct v4l2_ctrl *anal_gain;
	struct v4l2_ctrl *digi_gain;
	struct v4l2_ctrl *hblank;
	struct v4l2_ctrl *vblank;
	struct v4l2_ctrl *pixel_rate;
	struct v4l2_ctrl *link_freq;
	struct v4l2_fwnode_endpoint bus_cfg;

	u32 chipid;

	bool remote_routing_to_isp;

	bool streaming;
	bool power_on;
	bool hot_plug;
	u8 is_reset;
	int hot_plug_irq;
	u32 hot_plug_state;
	u32 link_lock_state;
	struct maxim4c_hot_plug_work hot_plug_work;

	struct maxim4c_mode supported_mode;
	const struct maxim4c_mode *cur_mode;
	u32 cfg_modes_num;

	u32 module_index;
	const char *module_facing;
	const char *module_name;
	const char *len_name;
	const char *sensor_name;

	maxim4c_gmsl_link_t gmsl_link;
	maxim4c_video_pipe_t video_pipe;
	maxim4c_mipi_txphy_t mipi_txphy;

	struct maxim4c_pattern pattern;

	struct maxim4c_i2c_init_seq extra_init_seq;

	struct dentry *dbgfs_root;
} maxim4c_t;

#endif /* __MAXIM4C_DRV_H__ */
