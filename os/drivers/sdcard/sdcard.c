/*
 * Copyright (c) 2023 Rhys Weatherley
 *
 * Licensed under the Apache License, Version 2.0 with LLVM Exceptions,
 * See https://github.com/rweater/mosnix/blob/main/LICENSE for license
 * information.
 */

#include "drivers/sdcard/sdcard.h"
#include "drivers/spi/spi.h"
#include <mosnix/config.h>
#include <mosnix/target.h>
#include <mosnix/attributes.h>
#include <mosnix/printk.h>
#include <string.h>

#if defined(CONFIG_SD) && defined(CONFIG_SPI)

/* SD card commands: http://elm-chan.org/docs/mmc/mmc_e.html */
#define SD_CMD0     0       /**< Software reset */
#define SD_CMD1     1       /**< Initiate initialization process */
#define SD_CMD8     8       /**< Check voltage range */
#define SD_CMD9     9       /**< Read card specific data */
#define SD_CMD10    10      /**< Read card identification */
#define SD_CMD12    12      /**< Stop reading */
#define SD_CMD13    13      /**< Read the card status register */
#define SD_CMD16    16      /**< Change read/write block size */
#define SD_CMD17    17      /**< Read a block */
#define SD_CMD18    18      /**< Read multiple blocks */
#define SD_CMD23    23      /**< Set number of blocks for next multi-command */
#define SD_CMD24    24      /**< Write a block */
#define SD_CMD25    25      /**< Write multiple blocks */
#define SD_CMD32    32      /**< Erase block start */
#define SD_CMD33    33      /**< Erase block end */
#define SD_CMD38    38      /**< Erase all selected blocks */
#define SD_CMD55    55      /**< Application command escape prefix */
#define SD_CMD58    58      /**< Read OCR */
#define SD_ACMD23   23      /**< Set number of blocks to pre-erase */
#define SD_ACMD41   41      /**< Activate the card's initialization process */

/* R1 status bits */
#define SD_R1_READY 0x00    /**< Ready state */
#define SD_R1_IDLE  0x01    /**< In idle state */
#define SD_R1_ERASE 0x02    /**< Erase reset */
#define SD_R1_ILL   0x04    /**< Illegal command */
#define SD_R1_CRC   0x08    /**< CRC error */
#define SD_R1_ESERR 0x10    /**< Erase sequence error */
#define SD_R1_AERR  0x20    /**< Address error */
#define SD_R1_PERR  0x40    /**< Parameter error */

/* Other status bits */
#define SD_READY    0xFF    /**< SD card is ready */
#define SD_START    0xFE    /**< Start data block */
#define SD_STOP     0xFD    /**< Stop transmission */

/* Cache modes */
#define SD_C_EMPTY  0       /**< Cache is empty */
#define SD_C_READ   1       /**< Cache contains a block cached for reading */
#define SD_C_WRITE  2       /**< Cache contains a block cached for writing */

#define SD_DEBUG 0
#if SD_DEBUG
#define sd_debug(str) kputstr((str))
ATTR_NOINLINE static void sd_debug_value(const char *name, unsigned long value)
{
    kputstr(name);
    kputstr(": ");
    kputhex(value);
    kputnl();
}
ATTR_NOINLINE static void sd_debug_block
    (const char *name, const uint8_t *block, size_t size)
{
    uint8_t count = 0;
    kputstr(name);
    kputstr(": ");
    while (size > 0) {
        kputhexbyte(*block++);
        ++count;
        if ((count & 0x0F) == 0)
            kputnl();
        --size;
    }
    kputnl();
}
#else
#define sd_debug(str) do { ; } while (0)
#define sd_debug_value(name, value) do { ; } while (0)
#define sd_debug_block(name, block, size) do { ; } while (0)
#endif

/* Amount of time to wait before timing out the SD card detect */
#define SD_INIT_TIMEOUT (2 * 256) /* 2s in ticks of 1/256'th of a second */

sd_info_t sd_info;

/**
 * @brief Structure of an SD card command header.
 */
struct sd_card_command
{
    /** Command number */
    uint8_t cmd;

    /** Bytes of the argument */
    uint8_t arg_bytes[4];

    /** CRC byte for the command */
    uint8_t crc;
};

/**
 * @brief Sends the wakeup signal to the SD card.
 */
static void sd_wakeup(void)
{
    /* Wake up the SD card - Apply 74 or more clock pulses on SCLK with CS high.
     * We send 0xFF bytes over and over for 10 bytes / 80 clock pulses. */
    uint8_t count;
    spi_sdcard_raise_cs();
    for (count = 10; count > 0; --count) {
        spi_receive_byte();
    }
}

/**
 * @brief Sends an command to the SD card and wait for the response byte.
 *
 * @param[in] cmd The command to be sent.
 *
 * @return The response byte, or 0xFF on timeout.
 *
 * Assumes that the SD chip select is low.
 */
ATTR_NOINLINE static uint8_t sd_command(const struct sd_card_command *cmd)
{
    uint8_t timeout;
    uint8_t response = 0xFF;

    /* Wait for the SD card to not be busy */
    timeout = 255;
    for (;;) {
        if (spi_receive_byte() == (uint8_t)0xFF)
            break;
        --timeout;
        if (timeout == (uint8_t)0)
            return 0xFF;
    }

    /* Send the command */
    spi_send(cmd, sizeof(struct sd_card_command));

    /* Wait for the response byte */
    timeout = 255;
    for (;;) {
        response = spi_receive_byte();
        if ((response & (uint8_t)0x80) == (uint8_t)0)
            break;
        --timeout;
        if (timeout == (uint8_t)0)
            return 0xFF;
    }
    return response;
}

/**
 * @brief Sends a simple command to the SD card and waits for the
 * response byte.
 *
 * @param[in] buf Buffer to build the full command packet to send.
 * @param[in] cmd The command to be sent.
 *
 * @return The response byte, or 0xFF on timeout.
 *
 * Assumes that the SD chip select is low.
 */
ATTR_NOINLINE static uint8_t sd_simple_command
    (struct sd_card_command *buf, uint8_t cmd)
{
    buf->cmd = cmd;
    buf->arg_bytes[0] = 0;
    buf->arg_bytes[1] = 0;
    buf->arg_bytes[2] = 0;
    buf->arg_bytes[3] = 0;
    buf->crc = 0;
    return sd_command(buf);
}

/**
 * @brief Waits for a data transfer to start.
 *
 * @return SD_START if the transfer was started, or something else on error.
 */
ATTR_NOINLINE static uint8_t sd_wait_for_start(void)
{
    uint8_t timeout = 255;
    uint8_t response = 0xFF;
    for (;;) {
        response = spi_receive_byte();
        if (response != 0xFF)
            break;
        --timeout;
        if (timeout == (uint8_t)0)
            return 0xFF;
    }
    return response;
}

void sd_init(void)
{
    spi_init();
    sd_info.detect = 0;
    sd_info.sdhc = 0;
}

ATTR_NOINLINE uint8_t sd_detect(void)
{
    static uint8_t const cluster_sizes[] = {
        /* Valid cluster size values for FAT */
        1, 2, 4, 8, 16, 32, 64, 128
    };
    struct sd_card_command cmd;
    unsigned short timeout_base;
    uint8_t status;
    uint8_t version;

    /* Have we detected the SD card previously? */
    if (sd_info.detect) {
        /*
         * Do a CID query to see if it is still there.  If it is, then we
         * can short-cut the initialization process.
         *
         * If the card was removed and another inserted, then the new card
         * will reject the CID query and we fall back to full initialization.
         * The same happens if the same card with the same CID is re-inserted.
         */
        spi_sdcard_lower_cs();
        status = sd_simple_command(&cmd, SD_CMD10 + 0x40);
        if (status == SD_R1_READY) {
            if (sd_wait_for_start()) {
                /* Receive the 16 bytes of the CID and two checksum bytes.
                 * We don't care what the CID value is, so throw it away. */
                spi_blank(18);
                spi_sdcard_raise_cs();

                /* We have confirmed the existing SD card is still inserted */
                return SD_DETECT_EXISTING;
            }
        }
        spi_sdcard_raise_cs();
    }

    /* Clear the SD state */
    sd_info.detect = 0;
    sd_info.sdhc = 0;
    sd_info.part_offset = 0;
    sd_info.part_size = 0;

    /* Wake up the SD card */
    sd_debug("\nSD wakeup\n");
    sd_wakeup();

    /* Send CMD0 to the SD card to reset it.  May have to try multiple
     * times until the SD card fully wakes up. */
    sd_debug("CMD0\n");
    spi_sdcard_lower_cs();
    cmd.cmd = SD_CMD0 + 0x40;
    cmd.arg_bytes[0] = 0;
    cmd.arg_bytes[1] = 0;
    cmd.arg_bytes[2] = 0;
    cmd.arg_bytes[3] = 0;
    cmd.crc = 0x95; /* Must supply a CRC for this command */
    timeout_base = sys_clock();
    for (;;) {
        if (sd_command(&cmd) == SD_R1_IDLE) {
            break;
        }
        if ((sys_clock() - timeout_base) >= SD_INIT_TIMEOUT) {
            goto fail;
        }
    }
    version = 1;

    /* Send CMD8 to check the SD version */
    sd_debug("CMD8\n");
    cmd.cmd = SD_CMD8 + 0x40;
    cmd.arg_bytes[2] = 0x01;
    cmd.arg_bytes[3] = 0xAA;
    cmd.crc = 0x87;
    if ((sd_command(&cmd) & SD_R1_ILL) == 0) {
        spi_receive(cmd.arg_bytes, 4);
        if (cmd.arg_bytes[3] == 0xAA) {
            version = 2;
        } else {
            goto fail;
        }
    }

    /* Send CMD55/ACMD41 to initialize the SD card */
    sd_debug("CMD41\n");
    timeout_base = sys_clock();
    for (;;) {
        sd_simple_command(&cmd, SD_CMD55 + 0x40);
        cmd.cmd = SD_ACMD41 + 0x40;
        if (version == 2) {
            /* Indicate that we want version 2 initialization */
            cmd.arg_bytes[0] = 0x40;
        }
        if (sd_command(&cmd) == SD_R1_READY) {
            break;
        }
        if ((sys_clock() - timeout_base) >= SD_INIT_TIMEOUT) {
            goto fail;
        }
    }

    /* Send CMD58 to check for SDHC support (version 2 cards only) */
    if (version == 2) {
        sd_debug("CMD58\n");
        if (sd_simple_command(&cmd, SD_CMD58 + 0x40) != SD_R1_READY) {
            goto fail;
        }
        spi_receive(cmd.arg_bytes, 4);
        if ((cmd.arg_bytes[0] & 0xC0) == 0xC0) {
            sd_info.sdhc = 1;
        }
    }

    /* Read the card-specific data (CSD) register, which is 16 bytes of
     * data followed by two bytes of CRC. */
    sd_debug("CMD9\n");
    if (sd_simple_command(&cmd, SD_CMD9 + 0x40) != SD_R1_READY) {
        goto fail;
    }
    if (!sd_wait_for_start()) {
        goto fail;
    }
    spi_receive(sd_info.block, 18);
    sd_debug_block("CSD", sd_info.block, 16);

#if 1
    /* Use an assembly replacement for the size code because we
     * can do the shifts more efficiently in assembly code. */
    extern uint32_t sd_get_size(void);
    sd_info.part_size = sd_get_size();
    if (!sd_info.part_size) {
        goto fail;
    }
#else
    /* Get the number of blocks on the SD card from the CSD */
    status = sd_info.block[0] & 0xC0;
    if (status == 0x00) {
        /* Version 1 CSD: (C_SIZE + 1) << (C_SIZE_MULT + READ_BL_LEN - 7) */
        uint8_t read_bl_len = sd_info.block[5] & (uint8_t)0x0F;
        uint8_t c_size_mult =
            (sd_info.block[9] & (uint8_t)0x03) << 1;
        if (sd_info.block[10] & (uint8_t)0x80)
            ++c_size_mult;
        sd_info.part_size =
            (((uint32_t)(sd_info.block[6] & (uint8_t)0x03)) << 10) |
            (((uint32_t)(sd_info.block[7])) << 2) |
             ((uint32_t)(sd_info.block[8] >> 6));
        sd_info.part_size =
            (sd_info.part_size + 1) << (c_size_mult + read_bl_len - (uint8_t)7);
    } else if (status == 0x40) {
        /* Version 2 CSD: (C_SIZE + 1) << 10 */
        sd_info.part_size =
            (((uint32_t)(sd_info.block[9])) << 8) |
            (((uint32_t)(sd_info.block[8])) << 16) |
            (((uint32_t)(sd_info.block[7])) << 24);
        sd_info.part_size = (sd_info.part_size + 0x100) << 2;
    } else {
        goto fail;
    }
#endif

    /* See if we have a partition table in block 0 */
    sd_debug("MBR\n");
    if (sd_read(0) != SD_R1_READY) {
        goto fail;
    }
    if (sd_info.block[510] != (uint8_t)0x55) {
        /* Last two bytes should be 0x55, 0xAA */
        goto fail;
    }
    if (sd_info.block[511] != (uint8_t)0xAA) {
        goto fail;
    }
    status = sd_info.block[450];
    if (status == 0x0C) {
        /* First partition is in FAT32 format - find its extents */
        sd_info.part_offset = *((uint32_t *)(sd_info.block + 454));
        sd_info.part_size   = *((uint32_t *)(sd_info.block + 458));

        /* Read block 0 of the partition */
        sd_debug("PBR\n");
        if (sd_read(sd_info.part_offset) != SD_R1_READY) {
            goto fail;
        }
        if (sd_info.block[510] != (uint8_t)0x55) {
            /* Last two bytes should be 0x55, 0xAA */
            goto fail;
        }
        if (sd_info.block[511] != (uint8_t)0xAA) {
            goto fail;
        }
    } else if (status == 0x0E) {
        /* First partition is in FAT16 format - not supported */
        sd_debug("FAT16\n");
        goto fail;
    } else {
        /* The entire SD card is probably a FAT32 image with no partitioning */
    }

    /* Does block 0 look like the header for a FAT32 partition? */
    sd_debug("FAT32 Check\n");
    if (sd_info.block[11] != 0 || sd_info.block[12] != 2) {
        /* Sector size is not 512 */
        goto fail;
    }
    sd_info.cluster_size = sd_info.block[13];
    if (!memchr(cluster_sizes, sd_info.cluster_size, sizeof(cluster_sizes))) {
        /* Invalid cluster size */
        goto fail;
    }
    if (sd_info.block[16] != 2) {
        /* FAT32 partitions must have two FAT's */
        goto fail;
    }
    if (sd_info.block[17] != 0 || sd_info.block[18] != 0) {
        /* Root directory entry count must be zero for FAT32 */
        goto fail;
    }
    if (sd_info.block[19] != 0 || sd_info.block[20] != 0) {
        /* Total sector count must be zero for FAT32 */
        goto fail;
    }
    status = sd_info.block[21];
    if (status != (uint8_t)0xF0 && status < (uint8_t)0xF8) {
        /* Media type must be 0xF0 or 0xF8..0xFF for FAT */
        goto fail;
    }

    /* Turn the cluster size into a shift count */
    status = 1;
    sd_info.cluster_shift = 0;
    while (status < sd_info.cluster_size) {
        status <<= 1;
        ++(sd_info.cluster_shift);
    }

    /* Find the size and position of the two FAT's */
    sd_info.fat_size = *((const uint32_t *)(sd_info.block + 36));
    sd_info.fat1 = *((const uint16_t *)(sd_info.block + 14));
    sd_info.fat2 = sd_info.fat1 + sd_info.fat_size;

    /* Find the root directory cluster number */
    sd_info.root = *((const uint32_t *)(sd_info.block + 44));

    /* Find the data section of the partition where the clusters reside */
    sd_info.cluster_offset = sd_info.fat2 + sd_info.fat_size;
    sd_info.cluster_count =
        (sd_info.part_size - sd_info.cluster_offset) >> sd_info.cluster_shift;

    /* Adjust the FAT and cluster offsets for the partition so that we
     * don't have to do this later when reading and writing blocks. */
    sd_info.fat1 += sd_info.part_offset;
    sd_info.fat2 += sd_info.part_offset;
    sd_info.cluster_offset += sd_info.part_offset;

    /* Nothing is cached yet */
    sd_info.cache_block = 0;
    sd_info.cache_block2 = 0;
    sd_info.cache_mode = SD_C_EMPTY;

    /* Dump the information about the SD card */
    sd_debug_value("version", version);
    sd_debug_value("sdhc", sd_info.sdhc);
    sd_debug_value("offset", sd_info.part_offset);
    sd_debug_value("size", sd_info.part_size);
    sd_debug_value("fatsize", sd_info.fat_size);
    sd_debug_value("fat1", sd_info.fat1);
    sd_debug_value("fat2", sd_info.fat2);
    sd_debug_value("root", sd_info.root);
    sd_debug_value("clsize", sd_info.cluster_size);
    sd_debug_value("clshift", sd_info.cluster_shift);
    sd_debug_value("cloffset", sd_info.cluster_offset);
    sd_debug_value("clcount", sd_info.cluster_count);

    /* We have a new SD card and it is ready to go */
    spi_sdcard_raise_cs();
    sd_debug("SD OK\n");
    sd_info.detect = version;
    return SD_DETECT_NEW;

fail:
    /* Failed to detect an SD card */
    spi_sdcard_raise_cs();
    sd_debug("SD Fail\n");
    return SD_DETECT_NONE;
}

uint8_t sd_redetect(void)
{
    sd_info.detect = 0;
    return sd_detect();
}

#if 0 /* Implemented in assembly code */

ATTR_NOINLINE uint32_t sd_cluster_to_block(uint32_t cluster)
{
    /* The first cluster in the data section is number 2 */
    if (cluster < 2)
        return 0;
    cluster -= 2;

    /* Verify against the cluster count */
    if (cluster >= sd_info.cluster_count)
        return 0;

    /* Convert the cluster number into a block number */
    return sd_info.cluster_offset + (cluster << sd_info.cluster_shift);
}

#endif

/**
 * @brief Adjusts the block number into a read/write offset for the SD card.
 *
 * @param[in] blknum The block number to adjust.
 *
 * @return The read/write offset.
 */
static uint32_t sd_adjust_block_number(uint32_t blknum)
{
    /* Non-SDHC cards need to multiply the block number by 512 to
     * produce a byte offset instead of a block number. */
    if (!sd_info.sdhc)
        blknum <<= 9;

    /* Block number is ready to go */
    return blknum;
}

ATTR_NOINLINE uint8_t sd_read(uint32_t blknum)
{
    /* Adjust the block number for the partition offset and SD card type */
    blknum = sd_adjust_block_number(blknum);

    /* Send the read command to the SD card and wait for it to start */
    struct sd_card_command cmd = {
        .cmd = SD_CMD17 | 0x40,
        .arg_bytes[0] = (uint8_t)(blknum >> 24),
        .arg_bytes[1] = (uint8_t)(blknum >> 16),
        .arg_bytes[2] = (uint8_t)(blknum >> 8),
        .arg_bytes[3] = (uint8_t)blknum,
        .crc = 0
    };
    spi_sdcard_lower_cs();
    uint8_t response = sd_command(&cmd);
    if (response != SD_R1_READY) {
        spi_sdcard_raise_cs();
        return response;
    }
    if (!sd_wait_for_start()) {
        spi_sdcard_raise_cs();
        return 0xFF;
    }

    /* Receive the data */
    spi_receive(sd_info.block, SD_BLKSIZE);

    /* Read the two CRC bytes and discard them */
    spi_receive_byte();
    spi_receive_byte();

    /* Lower the chip select and return */
    spi_sdcard_raise_cs();
    return SD_R1_READY;
}

ATTR_NOINLINE uint8_t sd_write(uint32_t blknum)
{
    /* TODO */
    (void)blknum;
    return 0xFF;
}

ATTR_NOINLINE uint8_t sd_cache_read(uint32_t blknum)
{
    uint8_t status;

    /* Do we already have this block in the cache? */
    if (blknum == sd_info.cache_block) {
        if (sd_info.cache_mode != SD_C_EMPTY) {
            return SD_R1_READY;
        }
    }

    /* Flush the currently cached block out */
    if (sd_info.cache_mode != SD_C_EMPTY) {
        status = sd_cache_flush();
        if (status != SD_R1_READY)
            return status;
        sd_info.cache_block = 0;
        sd_info.cache_block2 = 0;
        sd_info.cache_mode = SD_C_EMPTY;
    }

    /* Read the new block in */
    status = sd_read(blknum);
    if (status != SD_R1_READY)
        return status;

    /* Record that the block is now cached */
    sd_info.cache_block = blknum;
    sd_info.cache_block2 = 0;
    sd_info.cache_mode = SD_C_READ;
    return SD_R1_READY;
}

ATTR_NOINLINE uint8_t sd_cache_write_prepare(uint32_t blknum)
{
    // TODO
    (void)blknum;
    return 0xFF;
}

ATTR_NOINLINE uint8_t sd_cache_write_prepare_fat(uint32_t blknum)
{
    // TODO
    (void)blknum;
    return 0xFF;
}

ATTR_NOINLINE uint8_t sd_cache_write_prepare_zero(uint32_t blknum)
{
    // TODO
    (void)blknum;
    return 0xFF;
}

ATTR_NOINLINE uint8_t sd_cache_flush(void)
{
    // TODO
    return SD_R1_READY;
}

#endif /* CONFIG_SD && CONFIG_SPI */
