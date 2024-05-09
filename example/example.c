#include <stdio.h>
#include "esp_log.h"
#include "esp_task.h"
#include "driver/i2c_master.h"
#include "msc342x.h"

#define TEST_I2C_PORT 0
#define I2C_MASTER_SCL_IO 19
#define I2C_MASTER_SDA_IO 18

void app_main(void)
{
	config_mcp3424_t mcp_conf = {
		._address = 0x6e,
		._resolution = 16,
		._mode = 1,
		._PGA = 3,
		._channel = 2,
	};
	///////////////////////////////////////////////config for i2c
	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = TEST_I2C_PORT,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = true,
	};
	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x6e,
		.scl_speed_hz = 100000,
	};
	i2c_master_dev_handle_t dev_handle;

	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
	ESP_LOGI("I2C", "bus add device");
///////////////////////////////////////////////////////////////

	mcp342x_init(&mcp_conf, dev_handle);
	while(1)
	{
		mcp342x_new_conversion(dev_handle, &mcp_conf);
		ESP_LOGI("I2C - meas", "%lf", mcp342x_measure(dev_handle,&mcp_conf));
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}///////////end of while
}

