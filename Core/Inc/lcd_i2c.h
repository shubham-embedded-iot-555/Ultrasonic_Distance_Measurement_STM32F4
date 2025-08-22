/*
 * lcd_i2c.h
 *
 *  Created on: Aug 21, 2025
 *      Author: shubh
 */

#ifndef INC_LCD_I2C_H_
#define INC_LCD_I2C_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

#define LCD_ADDR 0x4E   // PCF8574 I2C address (check your module: 0x27 or 0x4E)

extern I2C_HandleTypeDef hi2c1;

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_put_cur(int row, int col);

#endif /* INC_LCD_I2C_H_ */

