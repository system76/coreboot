/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <option.h>
#include <string.h>

#include <common/touchpad.h>

static void buf_set_le16(uint8_t *buf, size_t offset, uint16_t value)
{
	buf[offset] = value & 0xff;
	buf[offset + 1] = value >> 8;
}

static size_t starlabs_touchpad_encode_command(uint8_t *buf, uint8_t opcode,
					       uint8_t report_type,
					       uint8_t report_id)
{
	size_t length = 0;

	if (report_id < 0x0f) {
		buf[length++] = (report_type << 4) | report_id;
		buf[length++] = opcode;
	} else {
		buf[length++] = (report_type << 4) | 0x0f;
		buf[length++] = opcode;
		buf[length++] = report_id;
	}

	return length;
}

static int starlabs_touchpad_set_report(unsigned int bus, uint16_t cmd_reg,
					uint16_t data_reg, uint8_t report_id,
					const uint8_t *payload,
					size_t payload_len)
{
	uint8_t buf[16];
	size_t cmd_len = 0;
	size_t report_len = sizeof(uint16_t);

	if (report_id)
		report_len++;
	report_len += payload_len;

	buf_set_le16(buf, cmd_len, cmd_reg);
	cmd_len += sizeof(uint16_t);
	cmd_len += starlabs_touchpad_encode_command(buf + cmd_len,
						    I2C_HID_OPCODE_SET_REPORT,
						    I2C_HID_REPORT_TYPE_FEATURE,
						    report_id);
	buf_set_le16(buf, cmd_len, data_reg);
	cmd_len += sizeof(uint16_t);
	buf_set_le16(buf, cmd_len, report_len);
	cmd_len += sizeof(uint16_t);

	if (report_id)
		buf[cmd_len++] = report_id;

	memcpy(buf + cmd_len, payload, payload_len);
	cmd_len += payload_len;

	return i2c_write_raw(bus, STARLABS_TOUCHPAD_I2C_ADDR, buf, cmd_len);
}

static int starlabs_touchpad_reset(unsigned int bus, uint16_t cmd_reg)
{
	uint8_t buf[4];
	size_t cmd_len = 0;

	buf_set_le16(buf, cmd_len, cmd_reg);
	cmd_len += sizeof(uint16_t);
	cmd_len += starlabs_touchpad_encode_command(buf + cmd_len,
				    I2C_HID_OPCODE_RESET,
				    0, 0);

	return i2c_write_raw(bus, STARLABS_TOUCHPAD_I2C_ADDR, buf, cmd_len);
}

static int starlabs_touchpad_set_haptics(unsigned int bus, uint16_t cmd_reg,
				 uint16_t data_reg, uint8_t level)
{
	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
				    STARLABS_TOUCHPAD_HAPTICS_REPORT_ID,
				    &level, sizeof(level));
}

static int starlabs_touchpad_set_force(unsigned int bus, uint16_t cmd_reg,
				       uint16_t data_reg, uint16_t press,
				       uint16_t release)
{
	uint8_t payload[4];

	buf_set_le16(payload, 0, press);
	buf_set_le16(payload, 2, release);

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_FORCE_REPORT_ID,
					    payload, sizeof(payload));
}

static int starlabs_touchpad_write_user_reg(unsigned int bus, uint16_t cmd_reg,
					    uint16_t data_reg, uint8_t bank,
					    uint8_t addr, uint8_t value)
{
	const uint8_t payload[] = { addr, bank, value };

	return starlabs_touchpad_set_report(bus, cmd_reg, data_reg,
					    STARLABS_TOUCHPAD_USER_REG_REPORT_ID,
					    payload, sizeof(payload));
}

static int starlabs_touchpad_set_haptics_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_set_haptics(ctx->bus, ctx->cmd_reg,
				     ctx->data_reg, ctx->level);
}

static int starlabs_touchpad_set_force_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_set_force(ctx->bus, ctx->cmd_reg,
					   ctx->data_reg,
					   ctx->press, ctx->release);
}

static int starlabs_touchpad_write_user_reg_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_write_user_reg(ctx->bus, ctx->cmd_reg,
						ctx->data_reg, ctx->bank,
						ctx->addr, ctx->value);
}

static int starlabs_touchpad_reset_op(void *arg)
{
	const struct starlabs_touchpad_op_ctx *ctx = arg;

	return starlabs_touchpad_reset(ctx->bus, ctx->cmd_reg);
}

static int starlabs_touchpad_retry(int (*op)(void *arg), void *arg)
{
	int ret;
	int attempt;

	for (attempt = 0; attempt < STARLABS_TOUCHPAD_RETRIES; attempt++) {
		ret = op(arg);
		if (ret == 0)
			return 0;
		mdelay(STARLABS_TOUCHPAD_RETRY_DELAY_MS);
	}

	return ret;
}

static void starlabs_touchpad_apply_settings(void *arg)
{
	int ret;
	uint8_t wake_byte;
	struct starlabs_touchpad_op_ctx op_ctx = {
		.bus = STARLABS_TOUCHPAD_I2C_BUS,
		.cmd_reg = STARLABS_TOUCHPAD_FALLBACK_CMD_REG,
		.data_reg = STARLABS_TOUCHPAD_FALLBACK_DATA_REG,
		.level = get_uint_option("touchpad_haptics",
					 STARLABS_TOUCHPAD_HAPTICS_DEFAULT),
		.press = get_uint_option("touchpad_force_press",
					 STARLABS_TOUCHPAD_PRESS_FORCE_DEFAULT),
		.release = get_uint_option("touchpad_force_release",
					   STARLABS_TOUCHPAD_RELEASE_FORCE_DEFAULT),
		.bank = STARLABS_TOUCHPAD_USER_REG_BANK,
		.addr = STARLABS_TOUCHPAD_USER_REG_ADDR_RATE,
		.value = get_uint_option("touchpad_report_rate",
					 STARLABS_TOUCHPAD_REPORT_RATE_DEFAULT),
	};

	(void)arg;

	(void)i2c_read_raw(STARLABS_TOUCHPAD_I2C_BUS, STARLABS_TOUCHPAD_I2C_ADDR, &wake_byte, 1);
	mdelay(STARLABS_TOUCHPAD_WAKE_DELAY_MS);

	ret = starlabs_touchpad_retry(starlabs_touchpad_write_user_reg_op, &op_ctx);
	if (ret != 0) {
		printk(BIOS_ERR, "Touchpad settings: failed to set report rate %u: %d\n",
		       op_ctx.value, ret);
		return;
	}

	mdelay(STARLABS_TOUCHPAD_SETTLE_DELAY_MS);

	ret = starlabs_touchpad_retry(starlabs_touchpad_set_force_op, &op_ctx);
	if (ret != 0) {
		printk(BIOS_ERR,
		       "Touchpad settings: failed to set force thresholds %u/%u: %d\n",
		       op_ctx.press, op_ctx.release, ret);
		return;
	}

	mdelay(STARLABS_TOUCHPAD_SETTLE_DELAY_MS);

	ret = starlabs_touchpad_retry(starlabs_touchpad_reset_op, &op_ctx);
	if (ret != 0) {
		printk(BIOS_ERR, "Touchpad settings: failed to reset device before haptics write: %d\n",
		       ret);
		return;
	}

	mdelay(STARLABS_TOUCHPAD_RESET_DELAY_MS);

	ret = starlabs_touchpad_retry(starlabs_touchpad_set_haptics_op, &op_ctx);
	if (ret != 0) {
		printk(BIOS_ERR, "Touchpad settings: failed to set haptics level %u: %d\n",
		       op_ctx.level, ret);
		return;
	}

	mdelay(STARLABS_TOUCHPAD_SETTLE_DELAY_MS);

	printk(BIOS_INFO,
	       "Touchpad settings: applied via regs 0x%04x/0x%04x; "
	       "haptics=%u click=%u release=%u rate=%u\n",
	       op_ctx.cmd_reg, op_ctx.data_reg, op_ctx.level,
	       op_ctx.press, op_ctx.release, op_ctx.value);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, starlabs_touchpad_apply_settings,
		      NULL);
