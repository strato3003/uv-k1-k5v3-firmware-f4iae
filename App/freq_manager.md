# Frequency Manager System

## Overview

This system provides a centralized frequency management for overlay apps. Instead of each app managing its own frequency list (which consumes precious overlay RAM), frequencies are stored in the firmware's EEPROM and accessed through a simple API.

## Components

### 1. Firmware Module (`freq_manager.h` / `freq_manager.c`)

The core module that stores and manages:
- **Frequency entries**: freq (10Hz), family, description
- **Family names**: short identifiers for frequency groups (e.g., "APRS", "FM", "DMR")
- **App-Family associations**: which family each app belongs to

### 2. FreqManager App (`App/apps/freqmanager/`)

A dedicated overlay app for managing the frequency lists:
- Add/remove frequencies
- Organize frequencies by family
- Assign families to apps
- View all frequencies for an app

### 3. Modified Apps (e.g., `App/apps/aprsrx/`)

Apps that use the frequency manager:
- Open FreqManager via `handoff()` when MENU key is pressed
- Receive selected frequency from firmware (future enhancement)
- Store app-specific frequency index in their config

## Data Structures

### FreqManagerEntry_t
```c
typedef struct {
    uint32_t freq_10hz;           // Frequency in 10Hz units
    uint8_t  family_index;        // Index into family names array
    char     description[20];     // Description (e.g., "144.800 MHz")
} FreqManagerEntry_t;
```

### FreqManagerAppFamily_t
```c
typedef struct {
    char     app_name[16];        // Name of the app (e.g., "APRS RX")
    uint8_t  family_index;        // Index into family names array
} FreqManagerAppFamily_t;
```

### FreqManagerFamily_t
```c
typedef struct {
    char     name[16];            // Family name (e.g., "APRS")
} FreqManagerFamily_t;
```

## Current Implementation Status

### ✅ Completed
- Frequency manager data structures
- FreqManager app skeleton
- APRS RX integration (opens FreqManager via handoff)

### 🔄 In Progress
- Firmware API integration
- EEPROM storage implementation
- Complete FreqManager app UI

### 📋 Future Enhancements
- API functions in `app_api.h` for direct access
- Firmware-side frequency filtering
- Import/export frequency lists

## Usage Example

### In an App (e.g., aprsrx_app.c)

```c
// Open FreqManager to select frequency
else if(key==APP_KEY_MENU) {
    A->handoff("FREQMANAGER", NULL, 0);
}
```

### In FreqManager App

```c
// Get frequencies for current app
FreqManagerFilteredEntry_t entries[10];
uint8_t count = FREQ_MANAGER_GetFrequenciesForApp("APRS RX", entries, 10);

// Add a new frequency
FREQ_MANAGER_AddFrequency(14480000, "APRS", "144.800 MHz");

// Save changes
FREQ_MANAGER_Save();
```

## Storage Layout

The frequency manager data is stored in EEPROM at address `0x100` (after main config).

```
Offset 0x00: Version (1 byte)
Offset 0x01: num_frequencies (1 byte)
Offset 0x02: num_app_families (1 byte)
Offset 0x03: num_families (1 byte)
Offset 0x04: Frequencies array (variable)
Offset ...: App-Family associations (variable)
Offset ...: Family names (variable)
```

## Building

The FreqManager app can be built separately:

```bash
cd App/apps/freqmanager
./build.sh
```

## Integration with Firmware

To enable frequency manager in the firmware:

1. Add to `settings.h`:
```c
#include "freq_manager.h"
```

2. Initialize in `settings.c`:
```c
void SETTINGS_InitEEPROM(void) {
    FREQ_MANAGER_Init();
    // ...
}
```

3. Add API functions (optional for direct access):
```c
// In app_api.h
typedef struct app_api {
    // ... existing fields
    uint8_t (*freq_get_count)(const char *app_name);
    bool (*freq_get_by_index)(const char *app_name, uint8_t index, uint32_t *freq, char *desc);
} app_api_t;
```

## Notes

- The current implementation uses a handoff-based approach to avoid modifying the app API
- Each app can store its current frequency index in its own config
- The FreqManager app allows users to manage all frequencies centrally
- Future versions will add direct API access for better integration

## Testing

To test the current implementation:

1. Build and install the firmware with `freq_manager.c` included
2. Build the FreqManager app and install it
3. Build the modified APRS RX app
4. Run APRS RX and press MENU to open FreqManager
5. Select a frequency and return to APRS RX
