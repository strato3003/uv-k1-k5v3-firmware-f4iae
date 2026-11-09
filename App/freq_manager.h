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

#ifndef FREQ_MANAGER_H
#define FREQ_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

// Ensure stdint types are available
#ifndef __INT8_TYPE
#define __INT8_TYPE int8_t
#endif

/* Maximum number of frequency entries */
#define FREQ_MANAGER_MAX_FREQS 64

/* Maximum number of app-family associations */
#define FREQ_MANAGER_MAX_APP_FAMILIES 32

/* Maximum length for descriptions */
#define FREQ_MANAGER_DESCRIPTION_LEN 20

/* Maximum length for app names */
#define FREQ_MANAGER_APP_NAME_LEN 16

/* Maximum length for family names */
#define FREQ_MANAGER_FAMILY_LEN 16

/**
 * Structure for a frequency entry
 * Stored in EEPROM, so must be tightly packed
 */
typedef struct __attribute__((packed)) {
    uint32_t freq_10hz;           /* Frequency in 10Hz units (e.g., 14480000 = 144.800 MHz) */
    uint8_t  family_index;        /* Index into family names array */
    char     description[FREQ_MANAGER_DESCRIPTION_LEN];
} FreqManagerEntry_t;

/**
 * Structure for app-family association
 * Stored in EEPROM, so must be tightly packed
 */
typedef struct __attribute__((packed)) {
    char     app_name[FREQ_MANAGER_APP_NAME_LEN];
    uint8_t  family_index;        /* Index into family names array */
} FreqManagerAppFamily_t;

/**
 * Structure for family name
 * Stored in EEPROM, so must be tightly packed
 */
typedef struct __attribute__((packed)) {
    char     name[FREQ_MANAGER_FAMILY_LEN];
} FreqManagerFamily_t;

/**
 * Main configuration structure for frequency manager
 * This is stored in EEPROM
 */
typedef struct __attribute__((packed)) {
    uint8_t version; /* Configuration version, increment when structure changes */
    uint8_t num_frequencies;
    uint8_t num_app_families;
    uint8_t num_families;
    FreqManagerEntry_t frequencies[FREQ_MANAGER_MAX_FREQS];
    FreqManagerAppFamily_t app_families[FREQ_MANAGER_MAX_APP_FAMILIES];
    FreqManagerFamily_t families[FREQ_MANAGER_MAX_APP_FAMILIES];
} FreqManagerConfig_t;

/**
 * Runtime structure for filtering frequencies by app
 */
typedef struct {
    uint32_t freq_10hz;
    uint8_t family_index;
    char description[FREQ_MANAGER_DESCRIPTION_LEN];
    char family_name[FREQ_MANAGER_FAMILY_LEN];
} FreqManagerFilteredEntry_t;

/*
 * Frequency Manager Configuration Version
 * Increment this when the structure changes to force reinitialization
 */
#define FREQ_MANAGER_VERSION 1

/**
 * Initialize the frequency manager
 */
void FREQ_MANAGER_Init(void);

/**
 * Get the frequency manager configuration
 */
const FreqManagerConfig_t *FREQ_MANAGER_GetConfig(void);

/**
 * Get all frequencies for a specific app
 * 
 * @param app_name Name of the app to filter by (e.g., "APRS RX")
 * @param entries Array to fill with matching frequencies
 * @param max_entries Maximum number of entries to return
 * @return Number of entries filled
 */
uint8_t FREQ_MANAGER_GetFrequenciesForApp(const char *app_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries);

/**
 * Get all frequencies for a specific family
 * 
 * @param family_name Name of the family to filter by
 * @param entries Array to fill with matching frequencies
 * @param max_entries Maximum number of entries to return
 * @return Number of entries filled
 */
uint8_t FREQ_MANAGER_GetFrequenciesForFamily(const char *family_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries);

/**
 * Add a new frequency entry
 * 
 * @param freq_10hz Frequency in 10Hz units
 * @param family_name Family name (will be added if doesn't exist)
 * @param description Description of the frequency
 * @return true if added successfully, false if no space or error
 */
int8_t FREQ_MANAGER_AddFrequency(uint32_t freq_10hz, const char *family_name, const char *description);

/**
 * Remove a frequency entry by index
 * 
 * @param index Index of the frequency to remove
 * @return true if removed successfully
 */
bool FREQ_MANAGER_RemoveFrequency(uint8_t index);

/**
 * Add a new app-family association
 * 
 * @param app_name Name of the app
 * @param family_name Family name (will be added if doesn't exist)
 * @return true if added successfully, false if no space or error
 */
bool FREQ_MANAGER_AddAppFamily(const char *app_name, const char *family_name);

/**
 * Remove an app-family association
 * 
 * @param app_name Name of the app
 * @return true if removed successfully
 */
bool FREQ_MANAGER_RemoveAppFamily(const char *app_name);

/**
 * Add a new family
 * 
 * @param family_name Name of the family
 * @return Index of the family, or -1 if error
 */
int8_t FREQ_MANAGER_AddFamily(const char *family_name);

/**
 * Find family index by name
 * 
 * @param family_name Name of the family to find
 * @return Index of the family, or -1 if not found
 */
int8_t FREQ_MANAGER_FindFamily(const char *family_name);

/**
 * Find app-family index by app name
 * 
 * @param app_name Name of the app
 * @return Index of the app-family association, or -1 if not found
 */
int8_t FREQ_MANAGER_FindAppFamily(const char *app_name);

/**
 * Save configuration to EEPROM
 */
void FREQ_MANAGER_Save(void);

/**
 * Check if a frequency exists
 * 
 * @param freq_10hz Frequency in 10Hz units
 * @return true if exists
 */
bool FREQ_MANAGER_FrequencyExists(uint32_t freq_10hz);

/**
 * Get a frequency entry by index
 * 
 * @param index Index of the frequency
 * @param entry Pointer to fill with frequency data
 * @return true if found
 */
bool FREQ_MANAGER_GetFrequencyByIndex(uint8_t index, FreqManagerFilteredEntry_t *entry);

#endif /* FREQ_MANAGER_H */
