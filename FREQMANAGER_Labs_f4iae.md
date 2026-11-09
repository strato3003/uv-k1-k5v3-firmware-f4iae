# Frequency Manager System - Labs_f4iae

## Overview

This document describes the Frequency Manager system implemented for the Labs_f4iae firmware based on Labs v6.3.0. The system provides centralized frequency management for overlay apps, allowing users to manage two lists:

1. **Frequency List**: category, frequency (10Hz units), description
2. **App-Family List**: category, app_name

## Firmware Configuration

### Labs_f4iae Edition

The Labs_f4iae firmware is based on Labs v6.3.0 with the following defines:
- Edition: `Labs_f4iae`
- Version: `v6.3.0`
- Author: `Labs F4IAE`

### Build Instructions

To build the Labs_f4iae firmware:

```bash
# Method 1: Using the build script
./build_labs_f4iae.sh

# Method 2: Manual CMake build
mkdir -p build_labs && cd build_labs
cmake -G Ninja .. \
    -D ENABLE_FEAT_F4HWN=ON \
    -D AUTHOR_STRING_1="Labs" \
    -D AUTHOR_STRING_2="F4IAE" \
    -D VERSION_STRING_1="v6.3.0" \
    -D VERSION_STRING_2="v6.3.0" \
    -D DISPLAY_VERSION_STRING_2="v6.3.0 Labs" \
    -D EDITION_STRING="Labs_f4iae" \
    -D VERSION_STRING="v6.3.0"
cmake --build .
```

## Components

### 1. Firmware Module (`App/freq_manager.h` / `App/freq_manager.c`)

The core module that stores and manages:
- **Frequency entries**: freq_10hz (frequency in 10Hz units), family_index, description
- **Family names**: short identifiers for frequency groups (e.g., "APRS", "FM", "DMR")
- **App-Family associations**: which family each app belongs to

**Data Structures:**
```c
FreqManagerEntry_t    - Stores freq_10hz, family_index, description
FreqManagerAppFamily_t - Associates app_name with family_index
FreqManagerFamily_t    - Stores family name
FreqManagerConfig_t    - Complete configuration stored in EEPROM
```

**Key Features:**
- EEPROM storage at address `sizeof(EEPROM_Config_t)`
- Maximum 64 frequencies, 32 app-family associations, 32 families
- Versioning to handle configuration changes
- Dirty flag for optimized EEPROM writes

### 2. FreqManager App (`App/apps/freqmanager/`)

A dedicated overlay app (3452 bytes, 84% of 4K limit) for managing the frequency lists.

**Navigation:**
- **UP/DOWN** (or **LEFT/RIGHT** on UV-K1): Move selection
- **MENU**: Confirm selection / enter submenu
- **EXIT**: Go back / cancel
- **STAR**: Toggle mode (when applicable)
- **0-9**: Enter digits for frequency
- **</>** (LEFT/RIGHT keys): Navigate and select characters for text input

**Menu Structure:**
1. List Frequencies - View all frequencies
2. List App-Families - View all app-family associations
3. Add Frequency - Add new frequency entry
4. Add App-Family - Add new app-family association
5. Edit Frequency - Edit existing frequency
6. Edit App-Family - Edit existing app-family association
7. Remove Frequency - Remove frequency entry
8. Remove App-Family - Remove app-family association

**Text Input Mode:**
- Use **UP/DOWN** to cycle through characters (A-Z, a-z, 0-9, ._- )
- **127** (DEL) to delete character before cursor
- **MENU** to confirm
- **EXIT** to cancel

### 3. Integration with Other Apps

Apps can use the frequency manager by:
1. Including `freq_manager.h` in the firmware
2. Calling `FREQ_MANAGER_GetFrequenciesForApp()` to get frequencies for their family
3. Using handoff to launch FreqManager app for selection

## Data Flow

### EEPROM Storage Layout

```
Offset 0x00: Version (1 byte)
Offset 0x01: num_frequencies (1 byte)
Offset 0x02: num_app_families (1 byte)
Offset 0x03: num_families (1 byte)
Offset 0x04: Frequencies array (FreqManagerEntry_t[64])
Offset ...: App-Family associations (FreqManagerAppFamily_t[32])
Offset ...: Family names (FreqManagerFamily_t[32])
```

### Example Usage

**In an App (e.g., aprsrx_app.c):**
```c
// Open FreqManager to select frequency
else if(key==APP_KEY_MENU) {
    A->handoff("FREQMANAGER", NULL, 0);
}
```

**In Firmware:**
```c
// Initialize in settings.c
FREQ_MANAGER_Init();

// Add a frequency
FREQ_MANAGER_AddFrequency(14480000, "APRS", "144.800 MHz");

// Save changes
FREQ_MANAGER_Save();

// Get frequencies for an app
FreqManagerFilteredEntry_t entries[10];
uint8_t count = FREQ_MANAGER_GetFrequenciesForApp("APRS RX", entries, 10);
```

## App Size Constraints

The FreqManager app is designed to fit within the 4 KiB overlay RAM limit:
- Current size: **3452 bytes (84% of 4 KiB)**
- Available space: 548 bytes

**Optimizations applied:**
- Compact state machine (no large arrays)
- Reused draw functions
- Minimal string usage
- Static character set for text input
- No floating point operations

## Testing

To test the FreqManager system:

1. **Build and install the Labs_f4iae firmware**
2. **Build the FreqManager app:**
   ```bash
   cd App/apps/freqmanager
   ./build.sh
   ```
3. **Install the FreqManager app** (using UV Studio or other tool)
4. **Run FreqManager:**
   - Press **F + 7** to open the apps menu
   - Select **FREQMANAGER** from the list
5. **Test functionality:**
   - Add frequencies with categories and descriptions
   - Add app-family associations
   - Edit and remove entries
   - Verify data persistence after power cycle

## Future Enhancements

1. **Direct API Access**: Add freq_manager functions to `app_api.h` for direct access from apps
2. **Import/Export**: Add functionality to import/export frequency lists via serial
3. **Frequency Validation**: Validate frequency ranges and step sizes
4. **Category Icons**: Add bitmap icons for different categories
5. **Search**: Add search functionality for large frequency lists

## API Reference

### FreqManager Functions

```c
// Initialization
void FREQ_MANAGER_Init(void);

// Get configuration
const FreqManagerConfig_t *FREQ_MANAGER_GetConfig(void);

// Frequency management
int8_t FREQ_MANAGER_AddFrequency(uint32_t freq_10hz, const char *family_name, const char *description);
bool FREQ_MANAGER_RemoveFrequency(uint8_t index);
bool FREQ_MANAGER_GetFrequencyByIndex(uint8_t index, FreqManagerFilteredEntry_t *entry);

// Family management
int8_t FREQ_MANAGER_AddFamily(const char *family_name);
int8_t FREQ_MANAGER_FindFamily(const char *family_name);

// App-Family management
bool FREQ_MANAGER_AddAppFamily(const char *app_name, const char *family_name);
bool FREQ_MANAGER_RemoveAppFamily(const char *app_name);
int8_t FREQ_MANAGER_FindAppFamily(const char *app_name);

// Query functions
uint8_t FREQ_MANAGER_GetFrequenciesForApp(const char *app_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries);
uint8_t FREQ_MANAGER_GetFrequenciesForFamily(const char *family_name, FreqManagerFilteredEntry_t *entries, uint8_t max_entries);

// Persistence
void FREQ_MANAGER_Save(void);
```

## Notes

- All frequency values are stored in 10Hz units (e.g., 144.800 MHz = 14480000)
- Category names are limited to 16 characters
- Descriptions are limited to 20 characters
- App names are limited to 16 characters
- All strings are null-terminated
- Maximum 64 frequencies, 32 app-family associations, 32 families

## License

Copyright 2026 JN Martineau F4IAE
Licensed under Apache License, Version 2.0
