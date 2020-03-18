/* Simple compatibility headers for AVR code used with ARM chips
 * Copyright (c) 2015 Paul Stoffregen <paul@pjrc.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Guidelines for editing this file:
// https://forum.pjrc.com/threads/34537-Teensy-LC-Increase-EEPROM-Size/page2

#ifndef _AVR_EEPROM_H_
#define _AVR_EEPROM_H_ 1

#include <stddef.h>
#include <stdint.h>

#include "avr_functions.h"

#define E2END 0x3AC5

#endif

uint8_t eeprom_read_byte(const uint8_t *addr_ptr);
void eeprom_write_byte(uint8_t *addr_ptr, uint8_t data);

uint16_t eeprom_read_word(const uint16_t *addr);
uint32_t eeprom_read_dword(const uint32_t *addr);
void eeprom_read_block(void *buf, const void *addr, uint32_t len);

int eeprom_is_ready(void);

void eeprom_write_word(uint16_t *addr, uint16_t value);
void eeprom_write_dword(uint32_t *addr, uint32_t value);
void eeprom_write_block(const void *buf, void *addr, uint32_t len);