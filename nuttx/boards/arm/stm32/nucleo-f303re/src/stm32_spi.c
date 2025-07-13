/****************************************************************************
 * boards/arm/stm32/nucleo-f303re/src/stm32_spi.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to the Apache License, Version 2.0 (the
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/spi/spi.h>

#include "arm_internal.h"
#include "chip.h"
#include "stm32.h"
#include "nucleo-f303re.h" /* For board-specific SPIDEV definitions like SPIDEV_MCP2515 */

#ifdef CONFIG_SPI

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 * Called to configure SPI chip select GPIO pins for the board. This
 * function should be called early in the board initialization to ensure
 * that all necessary CS pins are properly set up as outputs.
 *
 ****************************************************************************/

void weak_function stm32_spidev_initialize(void)
{
#if defined(CONFIG_LCD_SSD1351)
  /* Configure GPIO for OLED chip select and command/data pins */

  stm32_configgpio(GPIO_OLED_CS);  /* OLED chip select */
  stm32_configgpio(GPIO_OLED_DC);  /* OLED Command/Data */
#endif

#if defined(CONFIG_CAN_MCP2515)
  /* Configure GPIO for MCP2515 chip select (PC9). This is done only if
   * CONFIG_CAN_MCP2515 is enabled in the NuttX configuration.
   */

  stm32_configgpio(GPIO_MCP2515_CS); /* MCP2515 chip select (PC9) */
#endif
}

/****************************************************************************
 * Name: stm32_spi1/2/3select and stm32_spi1/2/3status
 *
 * Description:
 * The external functions, stm32_spi1/2/3select and stm32_spi1/2/3status
 * must be provided by board-specific logic.  They are implementations of
 * the select and status methods of the SPI interface defined by struct
 * spi_ops_s (see include/nuttx/spi/spi.h).  All other methods (including
 * stm32_spibus_initialize()) are provided by common STM32 logic.
 * To use this common SPI logic on your board:
 *
 * 1. Provide logic in stm32_boardinitialize() to configure SPI chip select
 * pins.
 * 2. Provide stm32_spi1/2/3select() and stm32_spi1/2/3status() functions
 * in your board-specific logic.  These functions will perform chip
 * selection and status operations using GPIOs in the way your board is
 * configured.
 * 3. Add a calls to stm32_spibus_initialize() in your low level
 * application initialization logic
 * 4. The handle returned by stm32_spibus_initialize() may then be used to
 * bind the SPI driver to higher level logic (e.g., calling
 * mmcsd_spislotinitialize(), for example, will bind the SPI driver to
 * the SPI MMC/SD driver).
 *
 ****************************************************************************/

#ifdef CONFIG_STM32_SPI1
void stm32_spi1select(struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  spiinfo("devid: %d CS: %s\n",
         (int)devid, selected ? "assert" : "de-assert");

#if defined(CONFIG_LCD_SSD1351)
  /* Handle chip selection for the OLED display. The OLED CS pin is active
   * low, so we invert the 'selected' state.
   */

  if (devid == SPIDEV_DISPLAY(0))
    {
      stm32_gpiowrite(GPIO_OLED_CS, !selected);
    }
#endif

#if defined(CONFIG_CAN_MCP2515)

  /* Handle chip selection for the MCP2515 CAN controller.
   * The MCP2515 CS pin is active low.
   * When 'selected' is true, the device is being selected, so the CS pin
   * must be driven LOW (0).
   * When 'selected' is false, the device is being deselected, so the CS pin
   * must be driven HIGH (1).
   */

  if (devid == SPIDEV_MCP2515)
    {
      stm32_gpiowrite(GPIO_MCP2515_CS, !selected);
    }
#endif
}

uint8_t stm32_spi1status(struct spi_dev_s *dev, uint32_t devid)
{
  /* This function typically returns status flags for the device, such as
   * whether a memory card is inserted (for SD cards) or if the device is
   * ready. For generic devices without specific status pins, or if status
   * is not critical, returning 0 (no flags set) is common.
   */

  return 0;
}
#endif /* CONFIG_STM32_SPI1 */

#ifdef CONFIG_STM32_SPI2
void stm32_spi2select(struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  spiinfo("devid: %d CS: %s\n",
         (int)devid, selected ? "assert" : "de-assert");

  /* Add chip selection logic for devices connected to SPI2 here, if any.
   * Example:
   * if (devid == SPIDEV_MYDEVICE_ON_SPI2)
   * {
   * stm32_gpiowrite(GPIO_MYDEVICE_CS, !selected);
   * }
   */
}

uint8_t stm32_spi2status(struct spi_dev_s *dev, uint32_t devid)
{
  /* Return status for devices on SPI2. */

  return 0;
}
#endif /* CONFIG_STM32_SPI2 */

#ifdef CONFIG_STM32_SPI3
void stm32_spi3select(struct spi_dev_s *dev, uint32_t devid,
                      bool selected)
{
  spiinfo("devid: %d CS: %s\n",
         (int)devid, selected ? "assert" : "de-assert");

  /* Add chip selection logic for devices connected to SPI3 here, if any.
   * Example:
   * if (devid == SPIDEV_ANOTHERDEVICE_ON_SPI3)
   * {
   * stm32_gpiowrite(GPIO_ANOTHERDEVICE_CS, !selected);
   * }
   */
}

uint8_t stm32_spi3status(struct spi_dev_s *dev, uint32_t devid)
{
  /* Return status for devices on SPI3. */

  return 0;
}
#endif /* CONFIG_STM32_SPI3 */

/****************************************************************************
 * Name: stm32_spi1cmddata
 *
 * Description:
 * Set or clear the SSD1351 D/C n bit to select data (true) or command
 * (false).  This function must be provided by platform-specific logic.
 * This is an implementation of the cmddata method of the SPI interface
 * defined by struct spi_ops_s (see include/nuttx/spi/spi.h).
 *
 * Input Parameters:
 * spi   - SPI device that controls the bus the device that requires the
 * CMD/DATA selection.
 * devid - If there are multiple devices on the bus, this selects which one
 * to select cmd or data.  NOTE:  This design restricts, for
 * example, one SPI display per SPI bus.
 * cmd   - true: select command; false: select data
 *
 * Returned Value:
 * OK on success, -ENODEV if the device ID is not recognized.
 *
 ****************************************************************************/

#ifdef CONFIG_SPI_CMDDATA
#ifdef CONFIG_STM32_SPI1
int stm32_spi1cmddata(struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
#ifdef CONFIG_LCD_SSD1351
  /* Handle command/data selection for the OLED display. The OLED D/C pin
   * controls whether the data being sent is a command or data.
   */

  if (devid == SPIDEV_DISPLAY(0))
    {
      stm32_gpiowrite(GPIO_OLED_DC, !cmd);
      return OK;
    }
#endif

  /* The MCP2515 does not use a separate command/data pin like a display.
   * If you have other SPI devices on SPI1 that require cmddata control,
   * add their logic here. Otherwise, return -ENODEV for unsupported devices.
   */

  return -ENODEV;
}
#endif /* CONFIG_STM32_SPI1 */

#ifdef CONFIG_STM32_SPI2
int stm32_spi2cmddata(struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
  /* Add command/data logic for devices on SPI2. */

  return -ENODEV;
}
#endif /* CONFIG_STM32_SPI2 */

#ifdef CONFIG_STM32_SPI3
int stm32_spi3cmddata(struct spi_dev_s *dev, uint32_t devid,
                      bool cmd)
{
  /* Add command/data logic for devices on SPI3. */

  return -ENODEV;
}
#endif /* CONFIG_STM32_SPI3 */
#endif /* CONFIG_SPI_CMDDATA */

#endif /* CONFIG_SPI */
