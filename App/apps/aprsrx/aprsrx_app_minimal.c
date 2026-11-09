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
 * APRS RX - Minimal version for frequency selection
 * 
 * This minimal version only displays frequency information and allows
 * selecting a frequency via the FreqManager app.
 * 
 * Navigation:
 * - MENU: Open FreqManager to select frequency
 * - EXIT: Return to app menu
 */

#include <stdint.h>
#include <stdbool.h>
#include "../app_api.h"
#include "aprsrx_assets.h"

#define APRSRX_VERSION "v1.1"

/* Global state */
static struct {
    const app_api_t *api;
    bool running;
    uint32_t selected_freq;
    char freq_desc[20];
} g;

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

/* Draw the screen */
static void draw_screen(void) {
    const app_api_t *A = g.api;
    
    A->display_clear();
    A->status_clear();
    
    /* Title */
    A->print_inverse("APRS RX F4IAE", 0, 0, false, true, 127);
    
    /* Frequency info */
    char freq_str[20];
    if (g.selected_freq > 0) {
        format_freq(g.selected_freq, freq_str);
        char line[30];
        char *p = line;
        char *f = freq_str;
        while (*f && p < line + sizeof(line) - 1) *p++ = *f++;
        *p++ = ' ';
        *p++ = 'M';
        *p++ = 'H';
        *p++ = 'z';
        *p = '\0';
        A->print_normal(line, 0, 127, 2);
        A->print_normal(g.freq_desc, 0, 127, 3);
    } else {
        A->print_normal("No frequency selected", 0, 127, 2);
        A->print_normal("Press MENU to select", 0, 127, 3);
    }
    
    /* Footer */
    A->print_tiny("MENU:Select Freq", 0, 50, false, false);
    A->print_tiny("EXIT:Back", 80, 50, false, false);
    A->print_tiny(APRSRX_VERSION, 110, 50, false, false);
    
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Handle key presses */
static void handle_keys(void) {
    uint8_t key = g.api->get_key();
    
    if (key == APP_KEY_INVALID) return;
    
    switch (key) {
        case APP_KEY_MENU:
            /* Open freqmanager app to select frequency for APRS RX */
            g.api->handoff("FREQMANAGER", "APRS RX", 8);
            break;
            
        case APP_KEY_EXIT:
            g.running = false;
            break;
            
        default:
            break;
    }
}

/* Entry point */
__attribute__((section(".text.entry"),used))
void app_main(const app_api_t *api) {
    g.api = api;
    g.running = true;
    g.selected_freq = 0;
    g.freq_desc[0] = '\0';
    
    /* Try to read selected frequency from handoff (if called from freqmanager) */
    uint8_t handoff_data[32];
    uint16_t len = api->handoff_read(handoff_data, sizeof(handoff_data), NULL);
    if (len >= sizeof(uint32_t)) {
        memcpy(&g.selected_freq, handoff_data, sizeof(uint32_t));
        if (len > sizeof(uint32_t)) {
            memcpy(g.freq_desc, handoff_data + sizeof(uint32_t), 
                   sizeof(g.freq_desc) - 1);
            g.freq_desc[sizeof(g.freq_desc) - 1] = '\0';
        } else {
            const char *sel = "Selected";
            for (int i = 0; i < sizeof(g.freq_desc) - 1 && sel[i]; i++) {
                g.freq_desc[i] = sel[i];
            }
            g.freq_desc[sizeof(g.freq_desc) - 1] = '\0';
        }
    }
    
    /* Main loop */
    while (g.running) {
        handle_keys();
        draw_screen();
        api->delay_ms(50);
    }
}
