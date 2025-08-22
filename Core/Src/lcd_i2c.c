#include "lcd_i2c.h"
#include "stm32f4xx_hal.h"

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;  // en=1, rs=0
    data_t[1] = data_u | 0x08;  // en=0, rs=0
    data_t[2] = data_l | 0x0C;  // en=1, rs=0
    data_t[3] = data_l | 0x08;  // en=0, rs=0
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D;  // en=1, rs=1
    data_t[1] = data_u | 0x09;  // en=0, rs=1
    data_t[2] = data_l | 0x0D;  // en=1, rs=1
    data_t[3] = data_l | 0x09;  // en=0, rs=1
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col) {
    switch (row) {
        case 0:
            lcd_send_cmd(0x80 + col);
            break;
        case 1:
            lcd_send_cmd(0xC0 + col);
            break;
    }
}

void lcd_init(void) {
    HAL_Delay(50);  // wait for >40ms
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20);  // 4-bit mode
    HAL_Delay(10);

    lcd_send_cmd(0x28);  // Function set: 2 Line, 4-bit, 5x8 dots
    HAL_Delay(1);
    lcd_send_cmd(0x08);  // Display off
    HAL_Delay(1);
    lcd_send_cmd(0x01);  // Clear display
    HAL_Delay(2);
    lcd_send_cmd(0x06);  // Entry mode set
    HAL_Delay(1);
    lcd_send_cmd(0x0C);  // Display on, Cursor off, Blink off
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}
