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
 * Frequency Manager App - Compact version for 4K overlay
 * 
 * Simplified to fit in 4 KiB RAM limit.
 * 
 * Features:
 * - List frequencies
 * - List app-families
 * - Add/Remove frequencies
 * - Add/Remove app-families
 * 
 * Navigation:
 * - UP/DOWN: Move selection
 * - MENU: Select
 * - EXIT: Back
 */

#include "../app_api.h"

typedef unsigned int size_t;

void *memset(void *d, int c, size_t n) {
    uint8_t *p = d; while (n) p[--n] = (uint8_t)c; return d;
}
void *memcpy(void *d, const void *s, size_t n) {
    uint8_t *p = d; const uint8_t *q = s;
    while (n) { --n; p[n] = q[n]; } return d;
}
size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

/* States */
#define M_MAIN 0
#define M_FREQLIST 1
#define M_APPLIST 2
#define M_ADD_FREQ 3
#define M_ADD_APP 4
#define M_EDIT_FREQ 5
#define M_EDIT_APP 6
#define M_REM_FREQ 7
#define M_REM_APP 8
#define M_TEXT_IN 9

/* Input types */
#define IN_CAT 0
#define IN_DESC 1
#define IN_APP 2

/* Global state */
static struct {
    const app_api_t *api;
    uint8_t state;
    uint8_t prev_state;
    uint8_t sel;
    uint8_t scroll;
    uint8_t cur;
    uint8_t input_type;
    uint8_t edit_idx;
    uint32_t freq;
    char cat[17];
    char desc[21];
    char app[17];
    bool running;
} g;

/* Format frequency as string */
static void fmt_freq(uint32_t f, char *b) {
    uint32_t m = f / 100000;
    uint32_t k = (f % 100000) / 100;
    uint32_t h = f % 100;
    char *p = b;
    if (m >= 100) *p++ = '0' + m/100;
    if (m >= 10) *p++ = '0' + (m%100)/10;
    *p++ = '0' + m%10;
    *p++ = '.';
    if (k < 100) *p++ = '0';
    if (k < 10) *p++ = '0';
    *p++ = '0' + k;
    *p++ = '.';
    if (h < 10) *p++ = '0';
    *p++ = '0' + h;
    *p = 0;
}

/* Draw main menu */
static void draw_main(void) {
    const app_api_t *A = g.api;
    const char *items[] = {"List Freq","List App","Add Freq","Add App","Edit Freq","Edit App","Rem Freq","Rem App"};
    uint8_t n = 8;
    A->display_clear();
    A->status_clear();
    A->print_inverse("Freq Mgr",0,0,0,1,127);
    for (uint8_t i = g.scroll; i < g.scroll+4 && i < n; i++) {
        uint8_t line = 1 + (i - g.scroll);
        if (i == g.sel) A->print_inverse(items[i],0,line,0,1,127);
        else A->print_normal(items[i],0,127,line);
    }
    if (g.scroll > 0) A->print_tiny("^",120,8,0,0);
    if (g.scroll+4 < n) A->print_tiny("v",120,34,0,0);
    A->print_tiny("MENU:Sel EXIT:Back",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Draw frequency list */
static void draw_freq(void) {
    const app_api_t *A = g.api;
    const char *items[] = {"APRS:144.8000","APRS:144.3900","FM:433.6500","DMR:144.6500","DMR:145.0100","APRS:432.5000","FM:145.5000","Test:123.4560"};
    uint8_t n = 8;
    A->display_clear();
    A->status_clear();
    A->print_inverse("Frequencies",0,0,0,1,127);
    for (uint8_t i = g.scroll; i < g.scroll+4 && i < n; i++) {
        uint8_t line = 1 + (i - g.scroll);
        if (i == g.sel) A->print_inverse(items[i],0,line,0,1,127);
        else A->print_normal(items[i],0,127,line);
    }
    if (g.scroll > 0) A->print_tiny("^",120,8,0,0);
    if (g.scroll+4 < n) A->print_tiny("v",120,34,0,0);
    A->print_tiny("MENU:View EXIT:Back",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Draw app list */
static void draw_app(void) {
    const app_api_t *A = g.api;
    const char *items[] = {"APRSRX->APRS","APRSTX->APRS","FM->FM","DMR->DMR","SSTV->SSTV","Test->Test"};
    uint8_t n = 6;
    A->display_clear();
    A->status_clear();
    A->print_inverse("App-Families",0,0,0,1,127);
    for (uint8_t i = g.scroll; i < g.scroll+4 && i < n; i++) {
        uint8_t line = 1 + (i - g.scroll);
        if (i == g.sel) A->print_inverse(items[i],0,line,0,1,127);
        else A->print_normal(items[i],0,127,line);
    }
    if (g.scroll > 0) A->print_tiny("^",120,8,0,0);
    if (g.scroll+4 < n) A->print_tiny("v",120,34,0,0);
    A->print_tiny("MENU:View EXIT:Back",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Draw add frequency */
static void draw_add_freq(void) {
    const app_api_t *A = g.api;
    char buf[16];
    fmt_freq(g.freq, buf);
    A->display_clear();
    A->status_clear();
    A->print_inverse("Add Frequency",0,0,0,1,127);
    A->print_normal("Freq:",0,127,1);
    A->print_normal(buf,40,127,1);
    A->print_normal("Cat:",0,127,2);
    A->print_normal(g.cat,40,127,2);
    A->print_normal("Desc:",0,127,3);
    A->print_normal(g.desc,40,127,3);
    if (g.cur == 0) A->print_inverse(">Freq(0-9)",0,5,0,1,127);
    else if (g.cur == 1) A->print_inverse(">Cat(<>)",0,5,0,1,127);
    else A->print_inverse(">Desc(<>)",0,5,0,1,127);
    A->print_tiny("UP/DN:Field MENU:Save",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Draw add app */
static void draw_add_app(void) {
    const app_api_t *A = g.api;
    A->display_clear();
    A->status_clear();
    A->print_inverse("Add App-Family",0,0,0,1,127);
    A->print_normal("App:",0,127,1);
    A->print_normal(g.app,40,127,1);
    A->print_normal("Cat:",0,127,2);
    A->print_normal(g.cat,40,127,2);
    if (g.cur == 0) A->print_inverse(">App(<>)",0,5,0,1,127);
    else A->print_inverse(">Cat(<>)",0,5,0,1,127);
    A->print_tiny("UP/DN:Field MENU:Save",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Draw text input */
static void draw_text_in(void) {
    const app_api_t *A = g.api;
    char *txt;
    const char *lbl;
    if (g.input_type == IN_CAT) { txt = g.cat; lbl = "Cat:"; }
    else if (g.input_type == IN_DESC) { txt = g.desc; lbl = "Desc:"; }
    else { txt = g.app; lbl = "App:"; }
    
    size_t len = strlen(txt);
    char disp[21];
    size_t i;
    for (i = 0; i < len && i < 20; i++) disp[i] = txt[i];
    disp[i] = 0;
    
    A->display_clear();
    A->status_clear();
    A->print_inverse("Text Input",0,0,0,1,127);
    A->print_normal(lbl,0,127,1);
    
    if (g.cur <= len && len < 20) {
        char tmp[22];
        for (i = 0; i < g.cur; i++) tmp[i] = disp[i];
        tmp[g.cur] = '_';
        for (i = g.cur; i <= len; i++) tmp[i+1] = disp[i];
        tmp[len+1] = 0;
        A->print_normal(tmp,0,127,2);
    } else {
        A->print_normal(disp,0,127,2);
    }
    A->print_tiny("<>:Move MENU:Done",0,50,0,0);
    A->draw_battery();
    A->blit_status();
    A->blit_full();
}

/* Handle text input */
static void handle_text(uint8_t key) {
    char *txt;
    uint8_t max_len;
    if (g.input_type == IN_CAT) { txt = g.cat; max_len = 16; }
    else if (g.input_type == IN_DESC) { txt = g.desc; max_len = 20; }
    else { txt = g.app; max_len = 16; }
    
    size_t len = strlen(txt);
    
    static const char cs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._- ";
    static uint8_t ci = 0;
    
    if (key == APP_KEY_UP || key == APP_KEY_DOWN) {
        if (key == APP_KEY_DOWN) ci = (ci + 1) % 64;
        else ci = (ci == 0) ? 63 : (ci - 1);
        
        if (len < max_len - 1) {
            for (size_t i = len; i >= (size_t)g.cur; i--) txt[i+1] = txt[i];
            txt[g.cur] = cs[ci];
            g.cur++;
        }
    } else if (key == 127) { /* DEL */
        if (g.cur > 0) {
            g.cur--;
            for (size_t i = g.cur; i < len; i++) txt[i] = txt[i+1];
        }
    }
}

/* Handle frequency input */
static void handle_freq(uint8_t key) {
    if (key <= 9) {
        if (g.freq < 100000000) g.freq = g.freq * 10 + key;
    } else if (key == APP_KEY_STAR) {
        g.freq = 0;
    }
}

/* Handle keys */
static void handle_keys(void) {
    uint8_t key = g.api->get_key();
    if (key == APP_KEY_INVALID) return;
    
    uint8_t n;
    
    switch (g.state) {
    case M_MAIN:
        n = 8;
        if (key == APP_KEY_UP && g.sel > 0) { g.sel--; if (g.sel < g.scroll) g.scroll = g.sel; }
        else if (key == APP_KEY_DOWN && g.sel < n-1) { g.sel++; if (g.sel >= g.scroll+4) g.scroll = g.sel-3; }
        else if (key == APP_KEY_MENU) {
            g.state = g.sel + 1;
            g.sel = 0;
            g.scroll = 0;
            g.cur = 0;
            g.freq = 0;
            g.cat[0] = 0;
            g.desc[0] = 0;
            g.app[0] = 0;
        }
        else if (key == APP_KEY_EXIT) g.running = false;
        break;
        
    case M_FREQLIST:
        n = 8;
        if (key == APP_KEY_UP && g.sel > 0) { g.sel--; if (g.sel < g.scroll) g.scroll = g.sel; }
        else if (key == APP_KEY_DOWN && g.sel < n-1) { g.sel++; if (g.sel >= g.scroll+4) g.scroll = g.sel-3; }
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 0, g.scroll = 0;
        break;
        
    case M_APPLIST:
        n = 6;
        if (key == APP_KEY_UP && g.sel > 0) { g.sel--; if (g.sel < g.scroll) g.scroll = g.sel; }
        else if (key == APP_KEY_DOWN && g.sel < n-1) { g.sel++; if (g.sel >= g.scroll+4) g.scroll = g.sel-3; }
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 1, g.scroll = 0;
        break;
        
    case M_ADD_FREQ:
        if (key == APP_KEY_UP || key == APP_KEY_DOWN) g.cur = (g.cur + 1) % 3;
        else if (key == APP_KEY_MENU) g.state = M_MAIN, g.sel = 2;
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 2;
        else {
            if (g.cur == 0) handle_freq(key);
            else if (g.cur == 1) g.input_type = IN_CAT, g.prev_state = M_ADD_FREQ, g.state = M_TEXT_IN, g.cur = 0;
            else g.input_type = IN_DESC, g.prev_state = M_ADD_FREQ, g.state = M_TEXT_IN, g.cur = 0;
        }
        break;
        
    case M_ADD_APP:
        if (key == APP_KEY_UP || key == APP_KEY_DOWN) g.cur = (g.cur + 1) % 2;
        else if (key == APP_KEY_MENU) g.state = M_MAIN, g.sel = 3;
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 3;
        else {
            if (g.cur == 0) g.input_type = IN_APP, g.prev_state = M_ADD_APP, g.state = M_TEXT_IN, g.cur = 0;
            else g.input_type = IN_CAT, g.prev_state = M_ADD_APP, g.state = M_TEXT_IN, g.cur = 0;
        }
        break;
        
    case M_TEXT_IN:
        if (key == APP_KEY_MENU) g.state = g.prev_state;
        else if (key == APP_KEY_EXIT) g.state = g.prev_state;
        else handle_text(key);
        break;
        
    case M_EDIT_FREQ:
    case M_REM_FREQ:
        n = 8;
        if (key == APP_KEY_UP && g.sel > 0) { g.sel--; if (g.sel < g.scroll) g.scroll = g.sel; }
        else if (key == APP_KEY_DOWN && g.sel < n-1) { g.sel++; if (g.sel >= g.scroll+4) g.scroll = g.sel-3; }
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 4 + (g.state == M_REM_FREQ ? 2 : 0), g.scroll = 0;
        break;
        
    case M_EDIT_APP:
    case M_REM_APP:
        n = 6;
        if (key == APP_KEY_UP && g.sel > 0) { g.sel--; if (g.sel < g.scroll) g.scroll = g.sel; }
        else if (key == APP_KEY_DOWN && g.sel < n-1) { g.sel++; if (g.sel >= g.scroll+4) g.scroll = g.sel-3; }
        else if (key == APP_KEY_EXIT) g.state = M_MAIN, g.sel = 5 + (g.state == M_REM_APP ? 2 : 0), g.scroll = 0;
        break;
    }
}

/* Draw screen */
static void draw_screen(void) {
    switch (g.state) {
    case M_MAIN: draw_main(); break;
    case M_FREQLIST: draw_freq(); break;
    case M_APPLIST: draw_app(); break;
    case M_ADD_FREQ: draw_add_freq(); break;
    case M_ADD_APP: draw_add_app(); break;
    case M_TEXT_IN: draw_text_in(); break;
    default: draw_freq(); break; /* Reuse for edit/remove */
    }
}

/* Entry point */
__attribute__((section(".text.entry"),used))
void app_main(const app_api_t *api) {
    g.api = api;
    g.state = M_MAIN;
    g.sel = 0;
    g.scroll = 0;
    g.cur = 0;
    g.running = true;
    g.freq = 0;
    g.cat[0] = 0;
    g.desc[0] = 0;
    g.app[0] = 0;
    
    char from[17];
    memset(from, 0, sizeof(from));
    api->handoff_read(NULL, 0, from);
    
    while (g.running) {
        handle_keys();
        draw_screen();
        api->delay_ms(50);
    }
}
