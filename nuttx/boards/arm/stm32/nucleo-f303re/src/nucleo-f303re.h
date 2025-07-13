/****************************************************************************
 * boards/arm/stm32/nucleo-f303re/src/nucleo-f303re.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_STM32_NUCLEO_F303RE_SRC_NUCLEO_F303RE_H
#define __BOARDS_ARM_STM32_NUCLEO_F303RE_SRC_NUCLEO_F303RE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "stm32.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LED definitions **********************************************************/

/* The Nucleo F303RE board has three LEDs.  Two of these are controlled by
 * logic on the board and are not available for software control:
 *
 * LD1 COM:  LD1 default status is red.  LD1 turns to green to indicate that
 * communications are in progress between the PC and the
 * ST-LINK/V2-1.
 * LD3 PWR:  red LED indicates that the board is powered.
 *
 * And one can be controlled by software:
 *
 * User LD2: green LED is a user LED connected to the I/O PA5 of the
 * STM32F303RET6.
 *
 * If CONFIG_ARCH_LEDS is not defined, then the user can control the LED in
 * any way.  The following definition is used to access the LED.
 */

#define GPIO_LED1       (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                         GPIO_OUTPUT_CLEAR|GPIO_PORTA|GPIO_PIN5)

#define LED_DRIVER_PATH "/dev/userleds"

/* Button definitions *******************************************************/

/* The Nucleo F303RE supports two buttons; only one button is controllable
 * by software:
 *
 * B1 USER:  user button connected to the I/O PC13 of the STM32F303RET6.
 * B2 RESET: push button connected to NRST is used to RESET the
 * STM32F303RET6.
 *
 * NOTE that EXTI interrupts are configured.
 */

#define MIN_IRQBUTTON  BUTTON_USER
#define MAX_IRQBUTTON  BUTTON_USER
#define NUM_IRQBUTTONS 1

#define GPIO_BTN_USER  (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI|GPIO_PORTC|GPIO_PIN13)

/* PWM definitions **********************************************************/

/* The Nucleo F303RE has no real on-board PWM devices, but the board can be
 * configured to output a pulse train using variously unused pins on the
 * board for PWM output (see board.h for details of pins).
 */

#ifdef CONFIG_PWM
#  if defined(CONFIG_STM32_TIM2_PWM)
#    define NUCLEO_F303RE_PWMTIMER 2
#  elif defined(CONFIG_STM32_TIM3_PWM)
#    define NUCLEO_F303RE_PWMTIMER 3
#  elif defined(CONFIG_STM32_TIM4_PWM)
#    define NUCLEO_F303RE_PWMTIMER 4
#  endif
#endif

/* OLED display definitions *************************************************/

#ifdef CONFIG_LCD_SSD1351
#  define GPIO_OLED_RESET (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                           GPIO_OUTPUT_CLEAR|GPIO_PORTB|GPIO_PIN5)
#  define GPIO_OLED_CS    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                           GPIO_OUTPUT_SET|GPIO_PORTA|GPIO_PIN8)
#  define GPIO_OLED_DC    (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|\
                           GPIO_OUTPUT_CLEAR|GPIO_PORTA|GPIO_PIN9)
#endif

/* MCP2515 IRQ line: PC.8 */

#define GPIO_MCP2515_IRQ (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                          GPIO_PORTC | GPIO_PIN8)
    
/* MCP2515 CS: PC.6 */
                 
#define GPIO_MCP2515_CS (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                         GPIO_OUTPUT_SET | GPIO_PORTC | GPIO_PIN6)

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 * Called to configure SPI chip select GPIO pins for the board.
 *
 ****************************************************************************/

#ifdef CONFIG_SPI
void weak_function stm32_spidev_initialize(void);
#endif

#ifdef CONFIG_CAN_MCP2515
/*
 * SPIDEV_CAN(n) and SPIDEV_MCP2515 definitions:
 *
 * These macros define unique device IDs for the MCP2515 CAN controller
 * when connected via SPI. They follow the NuttX convention for assigning
 * device IDs to custom or board-specific SPI peripherals.
 *
 * SPIDEV_USER(0) is a base identifier provided by NuttX in
 * include/nuttx/spi/spi.h. It designates a reserved range for user-defined
 * SPI device IDs, preventing conflicts with standard NuttX SPI device types
 * (e.g., SD cards, LCDs).
 *
 * SPIDEV_CAN(n):
 * This macro defines a family of IDs for CAN controllers connected via SPI.
 * It takes an integer 'n' as an argument, allowing for multiple instances
 * of CAN devices on the same SPI bus (e.g., SPIDEV_CAN(0) for the first,
 * SPIDEV_CAN(1) for the second, and so on). The expression
 * (SPIDEV_USER(0) + (n)) calculates a unique ID within the user-defined
 * range.
 *
 * SPIDEV_MCP2515:
 * This macro specifically assigns the first ID in the SPIDEV_CAN family
 * (i.e., SPIDEV_CAN(0)) to the MCP2515 chip. This provides a clear and
 * readable identifier for the MCP2515 in the board's SPI select logic.
 *
 * int stm32_mcp2515initialize(const char *devpath):
 * This is the function prototype for the board-specific initialization
 * routine of the MCP2515 CAN controller. It is declared here to make it
 * visible to other parts of the board code (e.g., board_initialize)
 * that need to set up the MCP2515 driver.
 */
# define SPIDEV_CAN(n)         (SPIDEV_USER(0) + (n))
# define SPIDEV_MCP2515        SPIDEV_CAN(0)
int stm32_mcp2515initialize(const char *devpath);
#endif

/****************************************************************************
 * Name: stm32_timer_driver_setup
 *
 * Description:
 * Configure the timer driver.
 *
 * Input Parameters:
 * devpath - The full path to the timer device.
 * This should be of the form /dev/timer0
 * timer   - The timer's number.
 *
 * Returned Value:
 * Zero (OK) is returned on success; A negated errno value is returned
 * to indicate the nature of any failure.
 *
 ****************************************************************************/

#ifdef CONFIG_TIMER
int stm32_timer_driver_setup(const char *devpath, int timer);
#endif

/****************************************************************************
 * Name: stm32_dac_setup
 *
 * Description:
 * Configure DAC peripheral for the board.
 *
 ****************************************************************************/

#ifdef CONFIG_DAC
int stm32_dac_setup(void);
#endif

/****************************************************************************
 * Name: stm32_pwm_setup
 *
 * Description:
 * Initialize PWM and register the PWM device.
 *
 ****************************************************************************/

#ifdef CONFIG_PWM
int stm32_pwm_setup(void);
#endif

/****************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 * Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/

#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/****************************************************************************
 * Name: stm32_can_setup
 *
 * Description:
 * Initialize CAN and register the CAN device
 *
 ****************************************************************************/

#ifdef CONFIG_STM32_CAN_CHARDRIVER
int stm32_can_setup(void);
#endif

#endif /* __BOARDS_ARM_STM32_NUCLEO_F303RE_SRC_NUCLEO_F303RE_H */
