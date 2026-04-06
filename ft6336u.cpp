#include "ft6336u.h"
#include <Wire.h>

TwoWire touch_i2c = TwoWire(1);         


static bool touch_i2c_write(uint8_t driver_addr, uint8_t reg_addr, const uint8_t *data, uint32_t length)
{
    touch_i2c.beginTransmission(driver_addr);
    touch_i2c.write(reg_addr);
    touch_i2c.write(data, length);


    if (touch_i2c.endTransmission() != 0) {
//        Serial.println("The I2C transmission fails. - I2C Read\r\n");
        return false;
    }
    return true;
}

static bool touch_i2c_read(uint8_t driver_addr, uint8_t reg_addr, uint8_t *data, uint32_t length)
{
    touch_i2c.beginTransmission(driver_addr);
    touch_i2c.write(reg_addr);
    if (touch_i2c.endTransmission() != 0) {
        // Serial.println("The I2C write fails. - I2C Read\r\n");
        return false;
    }

    touch_i2c.requestFrom(driver_addr, length);
    if (touch_i2c.available() != length) {
        // Serial.println("The I2C read fails. - I2C Read\r\n");
        return false;
    }
    touch_i2c.readBytes(data, length);
    return true; // 读取成功
}

bool touch_init(int tp_sda, int tp_scl, int tp_rst, int tp_int)
{
    // 初始化i2C
    touch_i2c.begin(tp_sda, tp_scl, TP_I2C_FREQ);
    pinMode(tp_rst, OUTPUT);
    // 复位
    digitalWrite(tp_rst, LOW);
    delay(200);
    digitalWrite(tp_rst, HIGH);
    delay(300);

    uint8_t data[5] = {0};
    touch_i2c_read(FT6336U_ADDR, FT6336U_ID_REG, data, 1);
    // if (data[0] != 0){
    //     // Serial.printf("读取成功：%x\n", data[0]);
    // }
    return true;
}


bool get_touch_coords(touch_coords_t *touch_coords)
{
    uint8_t x_arr[2] = {0};
    uint8_t y_arr[2] = {0};
    uint8_t touch_num = 0;

    touch_i2c_read(FT6336U_ADDR, FT6336U_TOUCH_NUM_REG, &touch_num, 1);
    if (touch_num == 0)
        return false;

    touch_i2c_read(FT6336U_ADDR, FT6336U_TOUCH_XH_REG, &x_arr[0], 1);
    touch_i2c_read(FT6336U_ADDR, FT6336U_TOUCH_XL_REG, &x_arr[1], 1);

    touch_i2c_read(FT6336U_ADDR, FT6336U_TOUCH_YH_REG, &y_arr[0], 1);
    touch_i2c_read(FT6336U_ADDR, FT6336U_TOUCH_YL_REG, &y_arr[1], 1);

    touch_coords->x = (uint16_t)(( x_arr[0] & 0x0f) <<  8);
    touch_coords->x |= x_arr[1];
    touch_coords->y = (uint16_t)((y_arr[0] & 0x0f) <<  8);
    touch_coords->y |= y_arr[1];
        
    touch_coords->x = 319 - touch_coords->x;
    touch_coords->y = 479 - touch_coords->y;
    // uint16_t t = touch_coords->y;
    // touch_coords->y = 319 - touch_coords->x;
    // touch_coords->x = t;

    return true;
}

