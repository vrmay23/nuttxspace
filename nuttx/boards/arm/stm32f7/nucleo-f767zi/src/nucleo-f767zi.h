/****************************************************************************
 * boards/arm/stm32f7/nucleo-f767zi/src/nucleo-f767zi.h
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
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_STM32F7_NUCLEO_767ZI_SRC_NUCLEO_767ZI_H
#define __BOARDS_ARM_STM32F7_NUCLEO_767ZI_SRC_NUCLEO_767ZI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* procfs File System */

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* nucleo-f767zi GPIO Pin Definitions ***************************************/

/* LED
 *
 * The nucleo-f767zi board has numerous LEDs but only three, LD1 a Green LED,
 * LD2 a Blue LED and LD3 a Red LED, that can be controlled by software.
 * The following definitions assume the default Solder Bridges are installed.
 */

#define GPIO_LD1       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_CLEAR | GPIO_PORTB | GPIO_PIN0)

#define GPIO_LD2       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_CLEAR | GPIO_PORTB | GPIO_PIN7)

#define GPIO_LD3       (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_CLEAR | GPIO_PORTB | GPIO_PIN14)

#define GPIO_LED_GREEN GPIO_LD1
#define GPIO_LED_BLUE  GPIO_LD2
#define GPIO_LED_RED   GPIO_LD3

#define LED_DRIVER_PATH "/dev/userleds"

/* BUTTONS
 *
 * The Blue pushbutton B1, labeled "User", is connected to GPIO PC13.
 * On this context, this button is called 'GPIO_BTN_BUILT_IN'.
 *
 * The other buttons (GPIO_BNT_EXERN_X) are the external buttons already
 * available for the user.
 *
 * A high value will be sensed when the button is depressed.
 * Note:
 *    1) That the EXTI is included in the definition to enable an interrupt
 *       on this IO.
 *    2) The following definitions assume the default Solder Bridges are
 *       installed.
 */

#define GPIO_BTN_BUILT_IN  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                            GPIO_PORTC | GPIO_PIN13)

#define GPIO_BTN_EXTERN_1  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                            GPIO_PORTF | GPIO_PIN15)

#define GPIO_BTN_EXTERN_2  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                            GPIO_PORTG | GPIO_PIN14)

#define GPIO_BTN_EXTERN_3  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                            GPIO_PORTG | GPIO_PIN9)

#define GPIO_BTN_EXTERN_4  (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                            GPIO_PORTE | GPIO_PIN0)

#define MIN_IRQBUTTON  BUTTON_BUILT_IN
#define MAX_IRQBUTTON  BUTTON_EXTERN_4
#define NUM_IRQBUTTONS (MIN_IRQBUTTON - MAX_IRQBUTTON + 1)

#define BUTTONS_DRIVER_PATH "/dev/buttons"

/* SPI **********************************************************************/

#define GPIO_SPI_CS    (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                        GPIO_OUTPUT_SET)

#define GPIO_SPI1_CS0  (GPIO_SPI_CS | GPIO_PORTA | GPIO_PIN15)
#define GPIO_SPI1_CS1  (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN15)
#define GPIO_SPI1_CS2  (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN14)
#define GPIO_SPI1_CS3  (GPIO_SPI_CS | GPIO_PORTC | GPIO_PIN2)
#define GPIO_SPI2_CS0  (GPIO_SPI_CS | GPIO_PORTD | GPIO_PIN7)
#define GPIO_SPI2_CS1  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN1)
#define GPIO_SPI2_CS2  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN2)
#define GPIO_SPI2_CS3  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN3)
#define GPIO_SPI3_CS0  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN4)
#define GPIO_SPI3_CS1  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN5)
#define GPIO_SPI3_CS2  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN6)
#define GPIO_SPI3_CS3  (GPIO_SPI_CS | GPIO_PORTG | GPIO_PIN7)

#if defined(CONFIG_STM32F7_SDMMC1) || defined(CONFIG_STM32F7_SDMMC2)
#  define HAVE_SDIO
#endif

#if defined(CONFIG_DISABLE_MOUNTPOINT) || !defined(CONFIG_MMCSD_SDIO)
#  undef HAVE_SDIO
#endif

#define SDIO_SLOTNO 0  /* Only one slot */

#ifdef HAVE_SDIO
#  if defined(CONFIG_STM32F7_SDMMC1)
#    define GPIO_SDMMC1_NCD (GPIO_INPUT|GPIO_FLOAT|GPIO_EXTI | GPIO_PORTC | GPIO_PIN6)
#  endif

#  if defined(CONFIG_NSH_MMCSDSLOTNO) && (CONFIG_NSH_MMCSDSLOTNO != 0)
#    warning "Only one MMC/SD slot, slot 0"
#    define CONFIG_NSH_MMCSDSLOTNO SDIO_SLOTNO
#  endif

#  if defined(CONFIG_NSH_MMCSDMINOR)
#    define SDIO_MINOR CONFIG_NSH_MMCSDMINOR
#  else
#    define SDIO_MINOR 0
#  endif
#endif

/* USB OTG FS
 *
 * PA9  OTG_FS_VBUS VBUS sensing (also connected to the green LED)
 * PC0  OTG_FS_PowerSwitchOn
 * PD5  OTG_FS_Overcurrent
 */

#define GPIO_OTGFS_VBUS   (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_OPENDRAIN|GPIO_PORTA|GPIO_PIN9)

#define GPIO_OTGFS_PWRON  (GPIO_OUTPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTG|GPIO_PIN6)

#ifdef CONFIG_USBHOST
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_EXTI|GPIO_FLOAT|\
                           GPIO_SPEED_100MHz|GPIO_PUSHPULL|\
                           GPIO_PORTG|GPIO_PIN7)

#else
#  define GPIO_OTGFS_OVER (GPIO_INPUT|GPIO_FLOAT|GPIO_SPEED_100MHz|\
                           GPIO_PUSHPULL|GPIO_PORTG|GPIO_PIN7)
#endif

/* GPIO pins used by the GPIO Subsystem */

#define BOARD_NGPIOIN     4   /* Amount of GPIO Input pins */
#if defined(CONFIG_STM32F7_TIM1_CH1NOUT) && defined (CONFIG_STM32F7_TIM1_CH2NOUT)
#define BOARD_NGPIOOUT    8   /* Amount of GPIO Output pins */
#elif defined(CONFIG_STM32F7_TIM1_CH1NOUT) || defined (CONFIG_STM32F7_TIM1_CH2NOUT)
#define BOARD_NGPIOOUT    9   /* Amount of GPIO Output pins */
#else
#define BOARD_NGPIOOUT    10   /* Amount of GPIO Output pins */
#endif
#define BOARD_NGPIOINT    1   /* Amount of GPIO Input w/ Interruption pins */

#define GPIO_INT1         (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTB | GPIO_PIN2)

#define GPIO_IN1          (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTE | GPIO_PIN7)
#define GPIO_IN2          (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTE | GPIO_PIN12)
#define GPIO_IN3          (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTE | GPIO_PIN14)
#define GPIO_IN4          (GPIO_INPUT | GPIO_FLOAT | GPIO_PORTE | GPIO_PIN15)

#define GPIO_OUT1         (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN4)
#define GPIO_OUT2         (GPIO_OUTPUT |  GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN5)
#define GPIO_OUT3         (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN6)
#define GPIO_OUT4          (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTA |GPIO_PIN5)
#define GPIO_OUT5         (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTF | GPIO_PIN12)
#if !defined(CONFIG_STM32F7_TIM1_CH1NOUT)
#define GPIO_OUT6         (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN8)
#endif
#if !defined(CONFIG_STM32F7_TIM1_CH2NOUT)
#define GPIO_OUT7         (GPIO_OUTPUT | GPIO_SPEED_50MHz | \
                                GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN10)
#endif


/* MCP2515 IRQ line: PB.0 */

#define GPIO_MCP2515_IRQ (GPIO_INPUT | GPIO_FLOAT | GPIO_EXTI | \
                          GPIO_PORTB | GPIO_PIN0)

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_bringup
 *
 * Description:
 *   Perform architecture specific initialization
 *
 *   CONFIG_BOARDCTL=y:
 *     If CONFIG_NSH_ARCHINITIALIZE=y:
 *       Called from the NSH library (or other application)
 *     Otherwise, assumed to be called from some other application.
 *
 *   Otherwise CONFIG_BOARD_LATE_INITIALIZE=y:
 *     Called from board_late_initialize().
 *
 *   Otherwise, bad news:  Never called
 *
 ****************************************************************************/

int stm32_bringup(void);

/****************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the nucleo-f767zi
 *   board.
 *
 ****************************************************************************/

#if defined(CONFIG_SPI)
void stm32_spidev_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_spidev_bus_test
 *
 * Description:
 *   Called to create the defined SPI buses and test them by initializing
 *   them and sending the NUCLEO_SPI_TEST (no chip select).
 *
 ****************************************************************************/

#if defined(CONFIG_NUCLEO_F722ZE_SPI_TEST)
int stm32_spidev_bus_test(void);
#endif

/****************************************************************************
 * Name: stm32_dma_alloc_init
 *
 * Description:
 *   Called to create a FAT DMA allocator
 *
 * Returned Value:
 *   0 on success or -ENOMEM
 *
 ****************************************************************************/

void stm32_dma_alloc_init(void);

#if defined (CONFIG_FAT_DMAMEMORY)
int stm32_dma_alloc_init(void);
#endif

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Called at application startup time to initialize the SCMMC
 *   functionality.
 *
 ****************************************************************************/

#ifdef CONFIG_MMCSD
int stm32_sdio_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_usbinitialize
 *
 * Description:
 *   Called from stm32_usbinitialize very early in initialization to setup
 *   USB-related GPIO pins for the nucleo-f767zi board.
 *
 ****************************************************************************/

#ifdef CONFIG_STM32F7_OTGFS
void stm32_usbinitialize(void);
#endif

/****************************************************************************
 * Name: stm32_pwm_setup
 *
 * Description:
 *   Initialize PWM and register the PWM device.
 *
 ****************************************************************************/

#ifdef CONFIG_PWM
int stm32_pwm_setup(void);
#endif

/****************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/

#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/****************************************************************************
 * Name: stm32_bbsram_int
 ****************************************************************************/

#ifdef CONFIG_STM32F7_BBSRAM
int stm32_bbsram_int(void);
#endif

/****************************************************************************
 * Name: stm32_qencoder_initialize
 ****************************************************************************/

#ifdef CONFIG_SENSORS_QENCODER
int stm32_qencoder_initialize(const char *devpath, int timer);
#endif

/****************************************************************************
 * Name: stm32f7_gpio_initialize
 ****************************************************************************/

#ifdef CONFIG_DEV_GPIO
int stm32_gpio_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_mcp2515initialize
 ****************************************************************************/

#ifdef CONFIG_CAN_MCP2515
int stm32_mcp2515initialize(const char *devpath);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_STM32F7_NUCLEO_767ZI_SRC_NUCLEO_767ZI_H */
