#pragma once

#include <stdio.h>

/* BALI notes:
 * make size template arguments?
 * add 32bit version
 */
static void print_4bit_hex(uint8_t i) {
        if (i < 10) putchar('0' + i);
        else putchar ('a' + i - 10);
}
static void print_uint8_hex(uint8_t i) {
        print_4bit_hex(i >> 4 & 0x0F);
        print_4bit_hex(i & 0xF);
}

static void print_uint16_hex(uint16_t i) {
        print_uint8_hex(i >> 8 & 0xFF);
        print_uint8_hex(i & 0xFF);
}
