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
 * Memo App - Frequency Selector
 * Based on APRS RX minimal menu style
 */

#include <stdint.h>
#include <stdbool.h>
#include "../app_api.h"
#include "memo_assets.h"

#define NUM_FREQ_ITEMS 40

/* Global state */
static struct {
    const app_api_t *api;
    bool running;
    bool in_menu;
    uint8_t selected_item;
    uint8_t scroll;
    uint8_t prevKey;
} g;

/* Freestanding helpers */
void *memset(void *d, int c, size_t n) {
    uint8_t *p = d; while (n) p[--n] = (uint8_t)c; return d;
}
void *memcpy(void *d, const void *s, size_t n) {
    uint8_t *p = d; const uint8_t *q = s;
    while (n) { --n; p[n] = q[n]; } return d;
}

/* Parse frequency string to uint32_t (x10 Hz) */
static uint32_t parse_frequency(const char *freq_str) {
    uint32_t whole = 0;
    uint32_t decimal = 0;
    uint8_t decimal_places = 0;
    bool after_decimal = false;
    
    while (*freq_str) {
        if (*freq_str == '.') {
            after_decimal = true;
            freq_str++;
            continue;
        }
        if (!after_decimal) {
            whole = whole * 10 + (*freq_str - '0');
        } else {
            decimal = decimal * 10 + (*freq_str - '0');
            decimal_places++;
        }
        freq_str++;
    }
    
    while (decimal_places < 6) {
        decimal *= 10;
        decimal_places++;
    }
    
    return whole * 100000 + decimal / 10;
}

/* Frequency data from uvk5-freq.csv - Without HF */
static const char *freq_items[NUM_FREQ_ITEMS] = {
    "RASEC85 145.475",
    "RASEC85B 145.212",
    "TACTUHF 444.625",
    "INTERSV 157.575",
    "F5ZWP_U 439.612",
    "F5ZWQ_U 430.212",
    "F1ZOQRX 430.475",
    "APRS 144.800",
    "DETRESSE 121.500",
    "MIL243 243.000",
    "SAR406A 406.025",
    "SAR406B 406.037",
    "VHF16 156.800",
    "VHF13 156.650",
    "VHF70 156.525",
    "VHF9 156.450",
    "VHF80 156.025",
    "AIS1 161.975",
    "AIS2 162.025",
    "NT-ATIS 126.930",
    "NT-TWR 118.655",
    "NT-SOL 121.655",
    "NT-APP 124.250",
    "RY-AFIS 119.905",
    "LS-AFIS 123.350",
    "FC-AFIS 119.765",
    "TR-TWR 118.100",
    "SA-AFIS 120.605",
    "ULM1235 123.500",
    "ULM1253 125.335",
    "FIS1228 122.800",
    "ACARS1 131.725",
    "ACARS2 131.525",
    "ACARS3 136.900",
    "ACARS4 130.025",
    "ACARS5 129.125",
    "VDL2 136.975"
};

/* Draw frequency menu - Match standard app menu layout */
static void draw_menu(void) {
    const app_api_t *A = g.api;
    
    A->display_clear();
    A->status_clear();
    
    /* Title - inverse at top */
    A->print_inverse("Memo", 0, 0, false, true, 127);
    
    /* Display 6 items starting from line 2 (2 empty lines under title) */
    for (uint8_t i = g.scroll; i < g.scroll + 6 && i < NUM_FREQ_ITEMS; i++) {
        uint8_t line = 2 + (i - g.scroll);
        const char *item = freq_items[i];
        
        if (i == g.selected_item) {
            A->print_inverse(item, 0, line, false, true, 127);
        } else {
            A->print_normal(item, 0, 127, line);
        }
    }
    
    /* Scroll indicators - tiny font */
    if (g.scroll > 0) {
        A->print_tiny("^", 120, 16, false, false);
    }
    if (g.scroll + 6 < NUM_FREQ_ITEMS) {
        A->print_tiny("v", 120, 40, false, false);
    }
    
    /* Footer - tiny font at y=50 */
    A->print_tiny("UP/DN", 0, 50, false, false);
    A->print_tiny("MENU:Sel", 40, 50, false, false);
    A->print_tiny("EXIT:Quit", 80, 50, false, false);
    
    A->blit_status();
    A->blit_full();
}

/* Handle keys */
static void handle_menu_keys(void) {
    uint8_t key = g.api->get_key();
    
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
            if (g.selected_item >= g.scroll + 6) {
                g.scroll = g.selected_item - 5;
            }
        }
    } else if (key == APP_KEY_MENU) {
        /* Set VFO and exit */
        if (g.selected_item < NUM_FREQ_ITEMS && g.api->vfo_stage != NULL) {
            const char *item = freq_items[g.selected_item];
            const char *space_ptr = item;
            while (*space_ptr && *space_ptr != ' ') space_ptr++;
            
            if (*space_ptr == ' ') {
                app_beam_channel_t channel;
                g.api->beam_get(&channel);
                channel.rx_frequency = parse_frequency(space_ptr + 1);
                g.api->vfo_stage(&channel);
            }
        }
        g.running = false;
    } else if (key == APP_KEY_EXIT) {
        g.running = false;
    }
}

/* Entry point */
__attribute__((section(".text.entry"),used))
void app_main(const app_api_t *api) {
    g.api = api;
    g.running = true;
    g.in_menu = true;
    g.selected_item = 0;
    g.scroll = 0;
    g.prevKey = APP_KEY_INVALID;
    
    while (g.running) {
        handle_menu_keys();
        draw_menu();
        api->delay_ms(50);
    }
}
