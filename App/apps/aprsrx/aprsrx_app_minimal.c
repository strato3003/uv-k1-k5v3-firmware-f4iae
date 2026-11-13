/* Copyright 2026 JN Martineau F4IAE
 * https://github.com/jnmartineau
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

/**
 * APRS RX - Minimal version with frequency selection menu
 * 
 * This version displays a frequency selection menu at startup:
 * - VFO (current frequency)
 * - MR 1-28 (memory channels) with CUSTOMIZABLE labels
 * 
 * Navigation:
 * - UP/DOWN: Select frequency
 * - MENU: Confirm selection and start app
 * - EXIT: Cancel and return to app menu
 */

#include <stdint.h>
#include <stdbool.h>
#include "../app_api.h"
#include "aprsrx_assets.h"

#define APRSRX_VERSION "v1.2"
#define NUM_MR_CHANNELS 28
#define NUM_FREQ_ITEMS (1 + NUM_MR_CHANNELS)  /* VFO + 28 MR channels */

/* Global state */
static struct {
    const app_api_t *api;
    bool running;
    bool in_freq_menu;
    uint8_t selected_item;  /* 0 = VFO, 1-28 = MR 1-28 */
    uint8_t scroll;
    uint8_t prevKey;        /* For key repeat handling */
    uint32_t selected_freq;
    char freq_desc[20];
} g;

/* Memory channel names - customize these to match your configured memories */
static const char *mr_names[NUM_MR_CHANNELS] = {
    "APRS 144.800",   // MR 1
    "APRS 144.390",   // MR 2
    "FM 433.650",     // MR 3
    "DMR 145.000",    // MR 4
    "APRS 432.500",   // MR 5
    "FM 145.500",     // MR 6
    "Test 1",         // MR 7
    "Test 2",         // MR 8
    "MR 9",           // MR 9
    "MR 10",          // MR 10
    "MR 11",          // MR 11
    "MR 12",          // MR 12
    "MR 13",          // MR 13
    "MR 14",          // MR 14
    "MR 15",          // MR 15
    "MR 16",          // MR 16
    "MR 17",          // MR 17
    "MR 18",          // MR 18
    "MR 19",          // MR 19
    "MR 20",          // MR 20
    "MR 21",          // MR 21
    "MR 22",          // MR 22
    "MR 23",          // MR 23
    "MR 24",          // MR 24
    "MR 25",          // MR 25
    "MR 26",          // MR 26
    "MR 27",          // MR 27
    "MR 28"           // MR 28
};

/* Freestanding helpers - no libc available in overlay */
void *memset(void *d, int c, size_t n) {
    uint8_t *p = d; while (n) p[--n] = (uint8_t)c; return d;
}
void *memcpy(void *d, const void *s, size_t n) {
    uint8_t *p = d; const uint8_t *q = s;
    while (n) { --n; p[n] = q[n]; } return d;
}

/* Format frequency as string */
static void format_freq(uint32_t freq_10hz, char *buf) {
    uint32_t mhz = freq_10hz / 100000;
    uint32_t khz = (freq_10hz % 100000) / 100;
    
    if (mhz >= 100) {
        buf[0] = (char)('0' + mhz / 100);
        buf[1] = (char)('0' + (mhz % 100) / 10);
        buf[2] = (char)('0' + mhz % 10);
        buf[3] = '.';
        buf[4] = (char)('0' + khz / 100);
        buf[5] = (char)('0' + (khz % 100) / 10);
        buf[6] = (char)('0' + khz % 10);
        buf[7] = '\0';
    } else if (mhz >= 10) {
        buf[0] = (char)('0' + mhz / 10);
        buf[1] = (char)('0' + mhz % 10);
        buf[2] = '.';
        buf[3] = (char)('0' + khz / 100);
        buf[4] = (char)('0' + (khz % 100) / 10);
        buf[5] = (char)('0' + khz % 10);
        buf[6] = '\0';
    } else {
        buf[0] = (char)('0' + mhz);
        buf[1] = '.';
        buf[2] = (char)('0' + khz / 100);
        buf[3] = (char)('0' + (khz % 100) / 10);
        buf[4] = (char)('0' + khz % 10);
        buf[5] = '\0';
    }
}

/* Draw frequency selection menu */
static void draw_freq_menu(void) {
    const app_api_t *A = g.api;
    
    A->display_clear();
    A->status_clear();
    
    /* Title */
    A->print_inverse("Select Frequency", 0, 0, false, true, 127);
    
    /* Display menu items */
    for (uint8_t i = g.scroll; i < g.scroll + 4 && i < NUM_FREQ_ITEMS; i++) {
        uint8_t line = 1 + (i - g.scroll);
        const char *label;
        if (i == 0) {
            label = "VFO";
        } else {
            label = mr_names[i - 1];  /* MR 1-28 are at indices 1-28 */
        }
        if (i == g.selected_item) {
            A->print_inverse(label, 0, line, false, true, 127);
        } else {
            A->print_normal(label, 0, 127, line);
        }
    }
    
    /* Scroll indicators */
    if (g.scroll > 0) {
        A->print_tiny("^", 120, 8, false, false);
    }
    if (g.scroll + 4 < (uint8_t)NUM_FREQ_ITEMS) {
        A->print_tiny("v", 120, 34, false, false);
    }
    
    /* Footer - show selection instruction */
    A->print_tiny("UP/DN:Move", 0, 50, false, false);
    A->print_tiny("MENU:Select", 50, 50, false, false);
    A->print_tiny("EXIT:Cancel", 100, 50, false, false);
    
    A->blit_status();
    A->blit_full();
}

/* Draw the main screen */
static void draw_screen(void) {
    const app_api_t *A = g.api;
    
    A->display_clear();
    A->status_clear();
    
    /* Title */
    A->print_inverse("APRS RX F4IAE", 0, 0, false, true, 127);
    
    /* Display selected frequency */
    char freq_str[20];
    if (g.selected_item == 0) {
        /* VFO selected */
        uint32_t current_freq = A->rx_freq();
        format_freq(current_freq, freq_str);
        A->print_normal(freq_str, 0, 127, 2);
        A->print_normal("VFO", 0, 127, 3);
    } else if (g.selected_item <= NUM_MR_CHANNELS) {
        /* MR channel selected */
        A->print_normal(mr_names[g.selected_item - 1], 0, 127, 2);
        /* Show MR number */
        char mr_line[10];
        if (g.selected_item < 10) {
            mr_line[0] = 'M';
            mr_line[1] = 'R';
            mr_line[2] = ' ';
            mr_line[3] = (char)('0' + g.selected_item);
            mr_line[4] = '\0';
        } else {
            mr_line[0] = 'M';
            mr_line[1] = 'R';
            mr_line[2] = ' ';
            mr_line[3] = (char)('0' + g.selected_item / 10);
            mr_line[4] = (char)('0' + g.selected_item % 10);
            mr_line[5] = '\0';
        }
        A->print_normal(mr_line, 0, 127, 3);
    } else {
        A->print_normal("No frequency selected", 0, 127, 2);
        A->print_normal("Press MENU to select", 0, 127, 3);
    }
    
    /* Footer */
    A->print_tiny("MENU:Freq Menu", 0, 50, false, false);
    A->print_tiny("EXIT:Back", 80, 50, false, false);
    A->print_tiny(APRSRX_VERSION, 110, 50, false, false);
    
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Handle key presses in frequency menu */
static void handle_freq_menu_keys(void) {
    uint8_t key = g.api->get_key();
    
    /* Skip if no key or same as previous (key repeat) */
    if (key == APP_KEY_INVALID || key == g.prevKey) {
        g.prevKey = key;
        return;
    }
    
    g.prevKey = key;
    
    if (key == APP_KEY_UP) {
        if (g.selected_item > 0) {
            g.selected_item--;
            if (g.selected_item < g.scroll) {
                g.scroll = g.selected_item;
            }
        }
    } else if (key == APP_KEY_DOWN) {
        if (g.selected_item < NUM_FREQ_ITEMS - 1) {
            g.selected_item++;
            if (g.selected_item >= g.scroll + 4) {
                g.scroll = g.selected_item - 3;
            }
        }
    } else if (key == APP_KEY_MENU) {
        /* Selection confirmed - exit menu */
        g.in_freq_menu = false;
        g.prevKey = APP_KEY_INVALID;  /* Reset for main menu */
    } else if (key == APP_KEY_EXIT) {
        /* Cancel - exit app */
        g.running = false;
        g.in_freq_menu = false;
    }
}

/* Handle key presses in main app */
static void handle_main_keys(void) {
    uint8_t key = g.api->get_key();
    
    /* Skip if no key or same as previous */
    if (key == APP_KEY_INVALID || key == g.prevKey) {
        g.prevKey = key;
        return;
    }
    
    g.prevKey = key;
    
    if (key == APP_KEY_MENU) {
        /* Open frequency selection menu */
        g.in_freq_menu = true;
        g.scroll = 0;
    } else if (key == APP_KEY_EXIT) {
        g.running = false;
    }
}

/* Entry point */
__attribute__((section(".text.entry"),used))
void app_main(const app_api_t *api) {
    g.api = api;
    g.running = true;
    g.in_freq_menu = true;  /* Start in frequency menu */
    g.selected_item = 0;    /* Default to VFO */
    g.scroll = 0;
    g.prevKey = APP_KEY_INVALID;
    g.selected_freq = 0;
    g.freq_desc[0] = '\0';
    
    while (g.running) {
        if (g.in_freq_menu) {
            handle_freq_menu_keys();
            draw_freq_menu();
        } else {
            handle_main_keys();
            draw_screen();
        }
        api->delay_ms(50);
    }
}
