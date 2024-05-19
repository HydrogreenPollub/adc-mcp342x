#ifndef msc342x_h
#define msc342x_h
#include <stdio.h>
#include "esp_log.h"
#include "esp_task.h"
#include "driver/i2c_master.h"
enum MEASURE_MODE
{
	ONE_SHOT_MODE=0,
	CONTINUOUS_MODE
};

enum RESOLUTION
{
	RESOLUTION_12_BITS=0,
	RESOLUTION_14_BITS,
	RESOLUTION_16_BITS,
	RESOLUTION_18_BITS
};

enum CHANNELS
{
	CH1=0,
	CH2,
	CH3,
	CH4
};

enum PGA
{
	PGA_X1=0,
	PGA_X2,
	PGA_X4,
	PGA_X8
};


typedef struct{
uint8_t _address;
int _resolution;
int _mode;
int _PGA;
int32_t _LSB;
uint8_t _channel;

}config_mcp3424_t;


void mcp342x_init(config_mcp3424_t *mcp_conf, i2c_master_dev_handle_t dev_handle);
uint8_t mcp342x_get_config(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf);
uint32_t mcp342x_measure(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf);
void mcp342x_new_conversion(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf);



#endif // !msc342x_h
