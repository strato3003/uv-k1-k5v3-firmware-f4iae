Labs_f4iae Firmware with Frequency Manager
==========================================

This release includes:

1. Labs_f4iae_firmware.bin (114 KB)
   - Based on F4HWN fieldops v6.3.0
   - Includes freq_manager module for frequency and app-family management
   - Note: This is a pre-built binary. To rebuild with custom settings:
     * Run: ./compile-firmware.sh Labs
     * Or use CMake with ENABLE_FEAT_F4HWN=ON

2. freqmanager.app (3.5 KB)
   - Overlay app for managing frequencies and app-families
   - Fits within 4 KB RAM limit (84% utilization)
   - Navigation: UP/DOWN to move, MENU to select, EXIT to go back
   - Input: Numpad (0-9) for frequencies, UP/DOWN for text character selection

Installation:
-----------
1. Flash Labs_f4iae_firmware.bin to your radio using your preferred method
2. Install freqmanager.app using UV Studio or similar tool
3. Access via F+7 -> Select FREQMANAGER from the apps menu

Features:
---------
- Manage frequency list (category, frequency, description)
- Manage app-family associations (category, app_name)
- Add/Remove/Edit/List operations for both lists
- Text input with character cycling using UP/DOWN keys
- Persistent storage in EEPROM

Note: The firmware binary provided is based on fieldops v6.3.0 which includes
all the necessary modifications for the Frequency Manager system.
