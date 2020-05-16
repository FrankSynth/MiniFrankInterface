/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2019 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// To configure the EEPROM size, edit E2END in avr/eeprom.h.
//
// Generally you should avoid editing this code, unless you really
// know what you're doing.

#define DEBUG 1

#if DEBUG == 0
#define PRINTLN(x) Serial.println(x)
#define PRINTLN2(x, y) Serial.println(x, y)
#define PRINT(x) Serial.print(x)
#define PRINT2(x, y) Serial.print(x, y)
#else
#define PRINTLN(x)
#define PRINTLN2(x, y)
#define PRINT(x)
#define PRINT2(x, y)
#endif

#include "eeprom.h"
#include "imxrt.h"
#include <arduino.h>
#include <string.h>


#define FLASH_BASEADDR 0x601F0000
#define FLASH_SECTORS 15

#define SECTORMULTIPLIER 4
#define DATASECTORS (FLASH_SECTORS * SECTORMULTIPLIER)
#define DATASECTORSIZE (4096 / SECTORMULTIPLIER)

#if E2END > (255 * DATASECTORS - 1)
#error "E2END is set larger than the maximum possible EEPROM size"
#endif

// Conversation about how this code works & what the upper limits are
// https://forum.pjrc.com/threads/57377?p=214566&viewfull=1#post214566

static void flash_write(void *addr, const void *data, uint32_t len);
static void flash_erase_sector(void *addr);

static uint8_t initialized = 0;
static uint16_t sector_index[DATASECTORS];

void eeprom_initialize(void) {
    PRINTLN("Init Memory");

    uint32_t sector;
    // printf("eeprom init\n");
    for (sector = 0; sector < DATASECTORS; sector++) {
        const uint16_t *p = (uint16_t *)(FLASH_BASEADDR + sector * DATASECTORSIZE);
        const uint16_t *end = (uint16_t *)(FLASH_BASEADDR + (sector + 1) * DATASECTORSIZE);
        uint16_t index = 0;
        do {
            if (*p++ == 0xFFFF)
                break;
            index++;
        } while (p < end);
        sector_index[sector] = index;
        PRINT("Sector ");
        PRINT(sector);
        PRINT(" has index ");
        PRINTLN(index);
    }
    initialized = 1;
}

uint8_t eeprom_read_byte(const uint8_t *addr_ptr) {
    PRINTLN("Read Byte");
    uint32_t addr = (uint32_t)addr_ptr;
    uint32_t sector, offset;
    const uint16_t *p, *end;
    uint8_t data = 0xFF;

    if (addr > E2END)
        return 0xFF;
    if (!initialized)
        eeprom_initialize();
    sector = (addr >> 2) % DATASECTORS;
    offset = (addr & 3) | (((addr >> 2) / DATASECTORS) << 2);
    // printf("ee_rd, addr=%u, sector=%u, offset=%u, len=%u\n",
    // addr, sector, offset, sector_index[sector]);
    p = (uint16_t *)(FLASH_BASEADDR + sector * DATASECTORSIZE);
    end = p + sector_index[sector];
    while (p < end) {
        uint32_t val = *p++;
        if ((val & 255) == offset)
            data = val >> 8;
    }
    return data;
}

void eeprom_write_byte(uint8_t *addr_ptr, uint8_t data) {
    PRINT("write byte, ");
    uint32_t addr = (uint32_t)addr_ptr;
    uint32_t sector, offset, index, i;
    uint16_t *p, *end;
    uint8_t olddata = 0xFF;
    uint8_t buf[SECTORMULTIPLIER][256];

    if (addr > E2END)
        return;
    if (!initialized)
        eeprom_initialize();

    sector = (addr >> 2) % DATASECTORS;
    offset = (addr & 3) | (((addr >> 2) / DATASECTORS) << 2);
    // printf("ee_wr, addr=%u, sector=%u, offset=%u, len=%u\n",
    // addr, sector, offset, sector_index[sector]);
    p = (uint16_t *)(FLASH_BASEADDR + sector * DATASECTORSIZE);
    end = p + sector_index[sector];
    while (p < end) {
        uint16_t val = *p++;
        if ((val & 255) == offset)
            olddata = val >> 8;
    }
    if (data == olddata) {
        PRINTLN("but byte written was the same");
        return;
    }
    PRINTLN("and it is new");

    if (sector_index[sector] < DATASECTORSIZE / 2) {
        // printf("ee_wr, writing\n");
        uint16_t newdata = offset | (data << 8);
        flash_write(end, &newdata, 2);
        sector_index[sector] = sector_index[sector] + 1;
        PRINT("Sector ");
        PRINT(sector);
        PRINT(" has new index ");
        PRINTLN(sector_index[sector]);
    }
    else {
        Serial.print("erase and rewrite memory, erasing datasector ");
        Serial.print(sector - sector % SECTORMULTIPLIER);
        Serial.print(" to datasector ");
        Serial.println((sector - sector % SECTORMULTIPLIER) + SECTORMULTIPLIER - 1);

        // printf("ee_wr, erase then write\n");
        memset(buf, 0xFF, sizeof(buf));

        for (int count = 0; count < SECTORMULTIPLIER; count++) {

            p = (uint16_t *)(FLASH_BASEADDR + (sector - sector % SECTORMULTIPLIER + count) * DATASECTORSIZE);
            end = p + DATASECTORSIZE / 2;
            while (p < end) {
                uint16_t val = *p++;
                buf[count][val & 255] = val >> 8;
            }
        }

        buf[sector % SECTORMULTIPLIER][offset] = data;

        p = (uint16_t *)(FLASH_BASEADDR + (sector - sector % SECTORMULTIPLIER) * DATASECTORSIZE);
        flash_erase_sector(p);

        for (int count = 0; count < SECTORMULTIPLIER; count++) {
            p = (uint16_t *)(FLASH_BASEADDR + (sector - sector % SECTORMULTIPLIER + count) * DATASECTORSIZE);
            index = 0;
            for (i = 0; i < 256; i++) {
                if (buf[count][i] != 0xFF) {
                    // TODO: combining these to larger write
                    // would (probably) be more efficient
                    uint16_t newval = i | (buf[count][i] << 8);
                    flash_write(p + index, &newval, 2);
                    index = index + 1;
                }
            }
            sector_index[sector - sector % SECTORMULTIPLIER + count] = index;
        }
    }
}

uint16_t eeprom_read_word(const uint16_t *addr) {
    const uint8_t *p = (const uint8_t *)addr;
    return eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8);
}

uint32_t eeprom_read_dword(const uint32_t *addr) {
    const uint8_t *p = (const uint8_t *)addr;
    return eeprom_read_byte(p) | (eeprom_read_byte(p + 1) << 8) | (eeprom_read_byte(p + 2) << 16) | (eeprom_read_byte(p + 3) << 24);
}

void eeprom_read_block(void *buf, const void *addr, uint32_t len) {
    const uint8_t *p = (const uint8_t *)addr;
    uint8_t *dest = (uint8_t *)buf;
    while (len--) {
        *dest++ = eeprom_read_byte(p++);
    }
}

int eeprom_is_ready(void) {
    return 1;
}

void eeprom_write_word(uint16_t *addr, uint16_t value) {
    uint8_t *p = (uint8_t *)addr;
    eeprom_write_byte(p++, value);
    eeprom_write_byte(p, value >> 8);
}

void eeprom_write_dword(uint32_t *addr, uint32_t value) {
    uint8_t *p = (uint8_t *)addr;
    eeprom_write_byte(p++, value);
    eeprom_write_byte(p++, value >> 8);
    eeprom_write_byte(p++, value >> 16);
    eeprom_write_byte(p, value >> 24);
}

void eeprom_write_block(const void *buf, void *addr, uint32_t len) {
    uint8_t *p = (uint8_t *)addr;
    const uint8_t *src = (const uint8_t *)buf;
    while (len--) {
        eeprom_write_byte(p++, *src++);
    }
}

#define LUT0(opcode, pads, operand) (FLEXSPI_LUT_INSTRUCTION((opcode), (pads), (operand)))
#define LUT1(opcode, pads, operand) (FLEXSPI_LUT_INSTRUCTION((opcode), (pads), (operand)) << 16)
#define CMD_SDR FLEXSPI_LUT_OPCODE_CMD_SDR
#define ADDR_SDR FLEXSPI_LUT_OPCODE_RADDR_SDR
#define READ_SDR FLEXSPI_LUT_OPCODE_READ_SDR
#define WRITE_SDR FLEXSPI_LUT_OPCODE_WRITE_SDR
#define PINS1 FLEXSPI_LUT_NUM_PADS_1
#define PINS4 FLEXSPI_LUT_NUM_PADS_4

static void flash_wait() {
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x05) | LUT1(READ_SDR, PINS1, 1); // 05 = read status
    FLEXSPI_LUT61 = 0;
    uint8_t status;
    do {
        FLEXSPI_IPRXFCR = FLEXSPI_IPRXFCR_CLRIPRXF; // clear rx fifo
        FLEXSPI_IPCR0 = 0;
        FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15) | FLEXSPI_IPCR1_IDATSZ(1);
        FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
        while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE)) {
            asm("nop");
        }
        FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
        status = *(uint8_t *)&FLEXSPI_RFDR0;
    } while (status & 1);
    FLEXSPI_MCR0 |= FLEXSPI_MCR0_SWRESET; // purge stale data from FlexSPI's AHB FIFO
    while (FLEXSPI_MCR0 & FLEXSPI_MCR0_SWRESET)
        ; // wait
    __enable_irq();
}

// write bytes into flash memory (which is already erased to 0xFF)
static void flash_write(void *addr, const void *data, uint32_t len) {
    __disable_irq();
    FLEXSPI_LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI_LUTCR = FLEXSPI_LUTCR_UNLOCK;
    FLEXSPI_IPCR0 = 0;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x06); // 06 = write enable
    FLEXSPI_LUT61 = 0;
    FLEXSPI_LUT62 = 0;
    FLEXSPI_LUT63 = 0;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    arm_dcache_delete(addr, len); // purge old data from ARM's cache
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE))
        ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x32) | LUT1(ADDR_SDR, PINS1, 24); // 32 = quad write
    FLEXSPI_LUT61 = LUT0(WRITE_SDR, PINS4, 1);
    FLEXSPI_IPTXFCR = FLEXSPI_IPTXFCR_CLRIPTXF; // clear tx fifo
    FLEXSPI_IPCR0 = (uint32_t)addr & 0x007FFFFF;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15) | FLEXSPI_IPCR1_IDATSZ(len);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    const uint8_t *src = (const uint8_t *)data;
    uint32_t n;
    while (!((n = FLEXSPI_INTR) & FLEXSPI_INTR_IPCMDDONE)) {
        if (n & FLEXSPI_INTR_IPTXWE) {
            uint32_t wrlen = len;
            if (wrlen > 8)
                wrlen = 8;
            if (wrlen > 0) {
                memcpy((void *)&FLEXSPI_TFDR0, src, wrlen);
                src += wrlen;
                len -= wrlen;
            }
            FLEXSPI_INTR = FLEXSPI_INTR_IPTXWE;
        }
    }
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE | FLEXSPI_INTR_IPTXWE;
    flash_wait();
}

// erase a 4K sector
static void flash_erase_sector(void *addr) {
    PRINTLN("Memory erase function");
    __disable_irq();
    FLEXSPI_LUTKEY = FLEXSPI_LUTKEY_VALUE;
    FLEXSPI_LUTCR = FLEXSPI_LUTCR_UNLOCK;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x06); // 06 = write enable
    FLEXSPI_LUT61 = 0;
    FLEXSPI_LUT62 = 0;
    FLEXSPI_LUT63 = 0;
    FLEXSPI_IPCR0 = 0;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    arm_dcache_delete((void *)((uint32_t)addr & 0xFFFFF000), 4096); // purge data from cache
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE))
        ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    FLEXSPI_LUT60 = LUT0(CMD_SDR, PINS1, 0x20) | LUT1(ADDR_SDR, PINS1, 24); // 20 = sector erase
    FLEXSPI_IPCR0 = (uint32_t)addr & 0x007FF000;
    FLEXSPI_IPCR1 = FLEXSPI_IPCR1_ISEQID(15);
    FLEXSPI_IPCMD = FLEXSPI_IPCMD_TRG;
    while (!(FLEXSPI_INTR & FLEXSPI_INTR_IPCMDDONE))
        ; // wait
    FLEXSPI_INTR = FLEXSPI_INTR_IPCMDDONE;
    flash_wait();
}
