
#include "msc342x.h"
volatile const uint8_t resolutionConvert[] = {12,14,16,18};
volatile const uint8_t PGAConvert[] = {1,2,4,8};
volatile uint8_t _buffer[4];

void mcp342x_init(config_mcp3424_t *mcp_conf, i2c_master_dev_handle_t dev_handle)
{
	mcp_conf->_channel&=0b11;
	mcp_conf->_resolution&=0b11;
	mcp_conf->_mode&=0b11;
	mcp_conf->_PGA&=0b11;
	mcp_conf->_LSB = 2048000000/(1<<(resolutionConvert[mcp_conf->_resolution]-1));
	mcp_conf->_address = ((0b1101) << 3) | (mcp_conf->_address & 0b111);
	uint8_t cfgbyte = 0;
	cfgbyte |= (mcp_conf->_channel 		& 0x3) << 5;
	cfgbyte |= (mcp_conf->_mode 		& 0x1) << 4;
	cfgbyte |= (mcp_conf->_resolution 	& 0x3) << 2;
	cfgbyte |= mcp_conf->_PGA 			& 0x3;
	ESP_LOGI("I2C - debug", "%d",cfgbyte);
	ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &cfgbyte, 1, -1));
}

uint8_t mcp342x_get_config(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf)
{
	// uint8_t _buffer[4] = {0,0,0,0};

	ESP_ERROR_CHECK(i2c_master_receive(dev_handle, _buffer, mcp_conf->_resolution==RESOLUTION_18_BITS?4:3, -1));  	//getConfiguration
	if(mcp_conf->_resolution==RESOLUTION_18_BITS)
{
		ESP_LOGI("I2C - debug", "%d", _buffer[3]);
		return _buffer[3];
}
	else 
{
		ESP_LOGI("I2C - debug", "%d", _buffer[2]);
		return _buffer[2];
}

}

void mcp342x_new_conversion(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf)
{
	uint8_t cfgbyte = mcp342x_get_config(dev_handle,mcp_conf);
	cfgbyte |= 128;
	ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &cfgbyte, 1, -1));
}

double mcp342x_measure(i2c_master_dev_handle_t dev_handle, config_mcp3424_t *mcp_conf)
{
    uint8_t cfgbyte_temp1 = 0;
	union resultUnion
	{
		uint8_t asBytes[4];
		int32_t asLong;
	} result;
	do
	{
		ESP_LOGI("I2C - temp1", "%d",cfgbyte_temp1);
		//////////////////////////////////////////////////////////////////////////////////////////GetConfiguration
		cfgbyte_temp1 = mcp342x_get_config(dev_handle, mcp_conf);	
	}while((!(cfgbyte_temp1 & 0b10000000))==0);	
	
	ESP_LOGI("I2C - debug", "4");
	if(mcp_conf->_resolution==RESOLUTION_18_BITS)
	{
		result.asBytes[3] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[2] = _buffer[0];
		result.asBytes[1] = _buffer[1];
		result.asBytes[0] = _buffer[2];
	} 
	else
	{
		result.asBytes[3] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[2] = _buffer[0]&0x80?0xFF:0x00;
		result.asBytes[1] = _buffer[0];
		result.asBytes[0] = _buffer[1];
	}
	ESP_LOGI("I2C - debug0", "%d",result.asBytes[0]);
	ESP_LOGI("I2C - debug1", "%d",result.asBytes[1]);
	ESP_LOGI("I2C - debug2", "%d",result.asBytes[2]);
	ESP_LOGI("I2C - debug3", "%d",result.asBytes[3]);
	return ((double)result.asLong * 0.00158);

}