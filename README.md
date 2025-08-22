# Ultrasonic_Distance_Measurement_STM32F4
A complete STM32CubeIDE project demonstrating **distance measurement** with the **HC-SR04 ultrasonic sensor** and display on an **I2C LCD (LCD1602)** using **HAL libraries** and **TIM2 Input Capture**.

[![STM32CubeIDE](https://img.shields.io/badge/STM32CubeIDE-Build-blue?logo=stmicroelectronics)](https://www.st.com/en/development-tools/stm32cubeide.html)
[![HAL](https://img.shields.io/badge/HAL-Driver-green)](https://www.st.com/en/embedded-software/stm32cube-mcu-packages.html)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## ✨ Features

* High-precision timing using **TIM2 Input Capture** (microsecond resolution)
* Interrupt-driven measurement, non-blocking operation
* Automatic **overflow handling** for reliable readings
* Easy integration with `Get_Distance()` function
* I2C LCD display support for showing the distance

---

## 📖 How It Works

1. TRIG pin is set HIGH for **10 µs** to trigger the sensor.
2. HC-SR04 emits **8 cycles of 40 kHz ultrasound**.
3. ECHO pin goes HIGH for a duration proportional to the distance.
4. STM32 Timer Input Capture measures the **pulse width**.
5. Distance is calculated:

```c
distance = (difference * 0.0343) / 2; // in cm
```

---

## 🛠 Hardware Setup

### Pin Connections

| Component | Pin  | STM32 Pin       | Description                |
| --------- | ---- | --------------- | -------------------------- |
| HC-SR04   | VCC  | 5V              | Power                      |
| HC-SR04   | GND  | GND             | Ground                     |
| HC-SR04   | TRIG | PE10 (GPIO)     | Trigger signal             |
| HC-SR04   | ECHO | PA0 (TIM2\_CH1) | Echo input (Timer capture) |
| I2C LCD   | GND  | GND             | Ground                     |
| I2C LCD   | VCC  | 3.3V / 5V       | Power                      |
| I2C LCD   | SDA  | PB7 (I2C1\_SDA) | Data line                  |
| I2C LCD   | SCL  | PB6 (I2C1\_SCL) | Clock line                 |

---

### Wiring Diagram (ASCII)

```
      +-----------------+          +-----------------+          +-------------+
      |    HC-SR04      |          |    STM32        |          |   I2C LCD   |
      |                 |          |                 |          |             |
      | VCC ------------+--------->| 5V              |          |             |
      | GND ------------+--------->| GND             |<---------+ GND         |
      | TRIG -----------+--------->| PE10            |          |             |
      | ECHO <----------+----------| PA0 (TIM2_CH1)  |          |             |
      |                 |          | PB7 (I2C1_SDA) <-----------+ SDA         |
      |                 |          | PB6 (I2C1_SCL) <-----------+ SCL         |
      |                 |          | 3.3V/5V -------+---------->| VCC         |
      +-----------------+          +-----------------+          +-------------+
```

> Replace this ASCII diagram with a real photo for professional documentation.

---

## 💻 Code Overview

### Global Variables

```c
uint32_t ic_val1 = 0;
uint32_t ic_val2 = 0;
uint8_t is_first_captured = 0;
uint32_t difference = 0;
float distance = 0;
```

### Trigger Function

```c
void Ultrasonic_Trigger(void) {
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_Delay(0.1); // 10 us pulse
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
}
```

### Input Capture Callback

```c
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        if (!is_first_captured) {
            ic_val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            is_first_captured = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        } else {
            ic_val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            difference = (ic_val2 > ic_val1) ? ic_val2 - ic_val1 : (0xffff - ic_val1) + ic_val2;

            distance = (difference * 0.0343)/2; // Distance in cm
            is_first_captured = 0;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
        }
    }
}
```

### Distance Function

```c
float Get_Distance(void) {
    Ultrasonic_Trigger();
    __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
    HAL_Delay(100); // wait for measurement
    return distance;
}
```

### Main Loop

```c
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();

    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    lcd_init();

    char buffer[50];
    while (1) {
        float dist = Get_Distance();
        int dist_int = (int)(dist + 0.5); // Rounded integer cm

        sprintf(buffer, "Distance=%d", dist_int);
        lcd_put_cur(0,0);
        lcd_send_string("Ultrasonic:");
        lcd_put_cur(1,0);
        lcd_send_string(buffer);

        HAL_Delay(100);
    }
}
```

---

## ⚠ Tips for Accuracy

* Use **accurate microsecond delays** for TRIG pulse.
* Handle **timer overflow** correctly (0xFFFF).
* Provide **stable 5V power** to HC-SR04.
* Wait at least **60 ms** between consecutive readings.

---

## 📁 Project Structure

```
STM32-HC-SR04-I2C_LCD/
│
├── Core/
│   ├── Inc/
│   │   └── main.h
│   └── Src/
│       ├── main.c          # Main application
│       ├── stm32f4xx_it.c  # Interrupt routines
│       └── lcd_i2c.c       # I2C LCD driver
│
├── Drivers/
│   └── STM32F4xx_HAL_Driver/
│
└── README.md
```

---

## 📜 License

MIT License – free to use, modify, and distribute.


---

If you want, I can also **generate a clean visual schematic (PNG)** showing your STM32 board, HC-SR04, and I2C LCD connections. This will make the README fully professional.

Do you want me to create that schematic?
