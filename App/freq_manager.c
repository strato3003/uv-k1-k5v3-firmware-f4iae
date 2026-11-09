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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freq_manager.h"
#include "settings.h"
#include "driver/eeprom.h"

/* Static configuration storage */
static FreqManagerConfig_t gFreqManagerConfig;

/* Flag to indicate if configuration has been modified */
static bool gFreqManagerDirty = false;

/* EEPROM address for frequency manager configuration */
/* Place it after the main EEPROM config */
#define FREQ_MANAGER_EEPROM_ADDR (sizeof(EEPROM_Config_t))

void FREQ_MANAGER_Init(void)
{
    /* Load configuration from EEPROM */
    EEPROM_ReadBuffer(FREQ_MANAGER_EEPROM_ADDR, (uint8_t *)&gFreqManagerConfig, sizeof(FreqManagerConfig_t));
    
    /* Check version - if 0 or invalid, initialize with defaults */
    if (gFreqManagerConfig.version != FREQ_MANAGER_VERSION) {
        gFreqManagerConfig.version = FREQ_MANAGER_VERSION;
        gFreqManagerConfig.num_frequencies = 0;
        gFreqManagerConfig.num_app_families = 0;
        gFreqManagerConfig.num_families = 0;
        gFreqManagerDirty = true;
    }
}

const FreqManagerConfig_t *FREQ_MANAGER_GetConfig(void)
{
    return &gFreqManagerConfig;
}

/* Helper function to find or create a family */
static int8_t find_or_create_family(const char *family_name)
{
    /* Check if family already exists */
    int8_t index = FREQ_MANAGER_FindFamily(family_name);
    if (index >= 0) {
        return index;
    }
    
    /* Create new family */
    if (gFreqManagerConfig.num_families >= FREQ_MANAGER_MAX_APP_FAMILIES) {
        return -1; /* No space */
    }
    
    index = gFreqManagerConfig.num_families;
    strncpy(gFreqManagerConfig.families[index].name, family_name, FREQ_MANAGER_FAMILY_LEN - 1);
    gFreqManagerConfig.families[index].name[FREQ_MANAGER_FAMILY_LEN - 1] = '\0';
    gFreqManagerConfig.num_families++;
    gFreqManagerDirty = true;
    
    return index;
}

int8_t FREQ_MANAGER_FindFamily(const char *family_name)
{
    if (!family_name) return -1;
    
    for (uint8_t i = 0; i < gFreqManagerConfig.num_families; i++) {
        if (strcmp(gFreqManagerConfig.families[i].name, family_name) == 0) {
            return i;
        }
    }
    return -1;
}

int8_t FREQ_MANAGER_FindAppFamily(const char *app_name)
{
    if (!app_name) return -1;
    
    for (uint8_t i = 0; i < gFreqManagerConfig.num_app_families; i++) {
        if (strcmp(gFreqManagerConfig.app_families[i].app_name, app_name) == 0) {
            return i;
        }
    }
    return -1;
}

int8_t FREQ_MANAGER_AddFamily(const char *family_name)
{
    if (!family_name || gFreqManagerConfig.num_families >= FREQ_MANAGER_MAX_APP_FAMILIES) {
        return -1;
    }
    
    int8_t existing = FREQ_MANAGER_FindFamily(family_name);
    if (existing >= 0) {
        return existing; /* Already exists */
    }
    
    uint8_t index = gFreqManagerConfig.num_families;
    strncpy(gFreqManagerConfig.families[index].name, family_name, FREQ_MANAGER_FAMILY_LEN - 1);
    gFreqManagerConfig.families[index].name[FREQ_MANAGER_FAMILY_LEN - 1] = '\0';
    gFreqManagerConfig.num_families++;
    gFreqManagerDirty = true;
    
    return (int8_t)index;
}

int8_t FREQ_MANAGER_AddFrequency(uint32_t freq_10hz, const char *family_name, const char *description)
{
    if (gFreqManagerConfig.num_frequencies >= FREQ_MANAGER_MAX_FREQS) {
        return -1; /* No space */
    }
    
    /* Find or create the family */
    int8_t family_index = find_or_create_family(family_name);
    if (family_index < 0) {
        return -1;
    }
    
    /* Check if frequency already exists */
    for (uint8_t i = 0; i < gFreqManagerConfig.num_frequencies; i++) {
        if (gFreqManagerConfig.frequencies[i].freq_10hz == freq_10hz &&
            gFreqManagerConfig.frequencies[i].family_index == family_index) {
            return (int8_t)i; /* Already exists */
        }
    }
    
    /* Add new frequency */
    uint8_t index = gFreqManagerConfig.num_frequencies;
    gFreqManagerConfig.frequencies[index].freq_10hz = freq_10hz;
    gFreqManagerConfig.frequencies[index].family_index = (uint8_t)family_index;
    strncpy(gFreqManagerConfig.frequencies[index].description, description, FREQ_MANAGER_DESCRIPTION_LEN - 1);
    gFreqManagerConfig.frequencies[index].description[FREQ_MANAGER_DESCRIPTION_LEN - 1] = '\0';
    gFreqManagerConfig.num_frequencies++;
    gFreqManagerDirty = true;
    
    return (int8_t)index;
}

bool FREQ_MANAGER_RemoveFrequency(uint8_t index)
{
    if (index >= gFreqManagerConfig.num_frequencies) {
        return false;
    }
    
    /* Shift all entries after index */
    for (uint8_t i = index; i < gFreqManagerConfig.num_frequencies - 1; i++) {
        gFreqManagerConfig.frequencies[i] = gFreqManagerConfig.frequencies[i + 1];
    }
    gFreqManagerConfig.num_frequencies--;
    gFreqManagerDirty = true;
    
    return true;
}

bool FREQ_MANAGER_AddAppFamily(const char *app_name, const char *family_name)
{
    if (!app_name || !family_name || gFreqManagerConfig.num_app_families >= FREQ_MANAGER_MAX_APP_FAMILIES) {
        return false;
    }
    
    /* Find or create the family */
    int8_t family_index = find_or_create_family(family_name);
    if (family_index < 0) {
        return false;
    }
    
    /* Check if app-family already exists */
    int8_t existing = FREQ_MANAGER_FindAppFamily(app_name);
    if (existing >= 0) {
        /* Update existing */
        gFreqManagerConfig.app_families[existing].family_index = (uint8_t)family_index;
        gFreqManagerDirty = true;
        return true;
    }
    
    /* Add new app-family association */
    uint8_t index = gFreqManagerConfig.num_app_families;
    strncpy(gFreqManagerConfig.app_families[index].app_name, app_name, FREQ_MANAGER_APP_NAME_LEN - 1);
    gFreqManagerConfig.app_families[index].app_name[FREQ_MANAGER_APP_NAME_LEN - 1] = '\0';
    gFreqManagerConfig.app_families[index].family_index = (uint8_t)family_index;
    gFreqManagerConfig.num_app_families++;
    gFreqManagerDirty = true;
    
    return true;
}

bool FREQ_MANAGER_RemoveAppFamily(const char *app_name)
{
    if (!app_name) return false;
    
    int8_t index = FREQ_MANAGER_FindAppFamily(app_name);
    if (index < 0) {
        return false;
    }
    
    /* Shift all entries after index */
    for (uint8_t i = (uint8_t)index; i < gFreqManagerConfig.num_app_families - 1; i++) {
        gFreqManagerConfig.app_families[i] = gFreqManagerConfig.app_families[i + 1];
    }
    gFreqManagerConfig.num_app_families--;
    gFreqManagerDirty = true;
    
    return true;
}

uint8_t FREQ_MANAGER_GetFrequenciesForApp(const char *app_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries)
{
    if (!app_name || !entries || max_entries == 0) {
        return 0;
    }
    
    /* Find the app-family association */
    int8_t app_family_index = FREQ_MANAGER_FindAppFamily(app_name);
    if (app_family_index < 0) {
        return 0; /* No frequencies for this app */
    }
    
    uint8_t family_index = gFreqManagerConfig.app_families[app_family_index].family_index;
    
    return FREQ_MANAGER_GetFrequenciesForFamily(gFreqManagerConfig.families[family_index].name, entries, max_entries);
}

uint8_t FREQ_MANAGER_GetFrequenciesForFamily(const char *family_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries)
{
    if (!family_name || !entries || max_entries == 0) {
        return 0;
    }
    
    /* Find the family */
    int8_t family_index = FREQ_MANAGER_FindFamily(family_name);
    if (family_index < 0) {
        return 0; /* No such family */
    }
    
    uint8_t count = 0;
    for (uint8_t i = 0; i < gFreqManagerConfig.num_frequencies && count < max_entries; i++) {
        if (gFreqManagerConfig.frequencies[i].family_index == (uint8_t)family_index) {
            entries[count].freq_10hz = gFreqManagerConfig.frequencies[i].freq_10hz;
            entries[count].family_index = gFreqManagerConfig.frequencies[i].family_index;
            strncpy(entries[count].description, gFreqManagerConfig.frequencies[i].description, FREQ_MANAGER_DESCRIPTION_LEN);
            entries[count].description[FREQ_MANAGER_DESCRIPTION_LEN - 1] = '\0';
            strncpy(entries[count].family_name, gFreqManagerConfig.families[family_index].name, FREQ_MANAGER_FAMILY_LEN);
            entries[count].family_name[FREQ_MANAGER_FAMILY_LEN - 1] = '\0';
            count++;
        }
    }
    
    return count;
}

bool FREQ_MANAGER_FrequencyExists(uint32_t freq_10hz)
{
    for (uint8_t i = 0; i < gFreqManagerConfig.num_frequencies; i++) {
        if (gFreqManagerConfig.frequencies[i].freq_10hz == freq_10hz) {
            return true;
        }
    }
    return false;
}

bool FREQ_MANAGER_GetFrequencyByIndex(uint8_t index, FreqManagerFilteredEntry_t *entry)
{
    if (index >= gFreqManagerConfig.num_frequencies || !entry) {
        return false;
    }
    
    FreqManagerEntry_t *src = &gFreqManagerConfig.frequencies[index];
    entry->freq_10hz = src->freq_10hz;
    entry->family_index = src->family_index;
    strncpy(entry->description, src->description, FREQ_MANAGER_DESCRIPTION_LEN);
    entry->description[FREQ_MANAGER_DESCRIPTION_LEN - 1] = '\0';
    
    if (src->family_index < gFreqManagerConfig.num_families) {
        strncpy(entry->family_name, gFreqManagerConfig.families[src->family_index].name, FREQ_MANAGER_FAMILY_LEN);
        entry->family_name[FREQ_MANAGER_FAMILY_LEN - 1] = '\0';
    } else {
        entry->family_name[0] = '\0';
    }
    
    return true;
}

void FREQ_MANAGER_Save(void)
{
    if (!gFreqManagerDirty) {
        return;
    }
    
    /* Save to EEPROM */
    EEPROM_WriteBuffer(FREQ_MANAGER_EEPROM_ADDR, (uint8_t *)&gFreqManagerConfig, sizeof(FreqManagerConfig_t));
    gFreqManagerDirty = false;
}
