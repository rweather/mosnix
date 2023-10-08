/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#ifndef MOSNIX_DRIVERS_SPI_H
#define MOSNIX_DRIVERS_SPI_H

#include <sys/types.h>
#include <mosnix/attributes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Note: These routines are only available if the target defines CONFIG_SPI */

/**
 * @brief Initializes the SPI bus.
 *
 * Initializes SCLK, MOSI, MISI, and raises all chip select lines for the
 * SPI devices that are known to the target.
 */
ATTR_LEAF void spi_init(void);

/**
 * @brief Sends a buffer of bytes on the SPI bus, while discarding
 * anything that is received.
 *
 * @param[in] data Points to the data to be sent.
 * @param[in] size The number of bytes to be sent.
 *
 * It is assumed that the chip select for the SPI device has already been
 * lowered by the calling code.
 */
ATTR_LEAF void spi_send(const void *data, size_t size);

/**
 * @brief Sends a single byte on the SPI bus, which discarding
 * anything that is received.
 *
 * @param[in] value The byte value to be sent.
 *
 * It is assumed that the chip select for the SPI device has already been
 * lowered by the calling code.
 */
ATTR_LEAF void spi_send_byte(unsigned char value);

/**
 * @brief Receives a buffer of bytes from the SPI bus, while sending
 * 0xFF bytes to the SPI device.
 *
 * @param[in] data Points to the buffer to fill with the received data.
 * @param[in] size The number of bytes to be received.
 *
 * It is assumed that the chip select for the SPI device has already been
 * lowered by the calling code.
 */
ATTR_LEAF void spi_receive(void *data, size_t size);

/**
 * @brief Receives a single byte from the SPI bus, while sending a
 * 0xFF byte to the SPI device.
 *
 * @return The byte that was received.
 *
 * It is assumed that the chip select for the SPI device has already been
 * lowered by the calling code.
 */
ATTR_LEAF unsigned char spi_receive_byte(void);

/**
 * @brief Receives a number of bytes from the SPI bus and discards them
 * while sending 0xFF bytes to the SPI device.
 *
 * @param[in] size The number of bytes to be received.
 *
 * It is assumed that the chip select for the SPI device has already been
 * lowered by the calling code.
 */
ATTR_LEAF void spi_blank(size_t size);

/**
 * @brief Lowers the chip select pin for the primary SD card interface.
 */
ATTR_LEAF void spi_sdcard_lower_cs(void);

/**
 * @brief Raises the chip select pin for the primary SD card interface.
 */
ATTR_LEAF void spi_sdcard_raise_cs(void);

#ifdef __cplusplus
}
#endif

#endif
