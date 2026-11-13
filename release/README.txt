APRS RX with Frequency Selection Menu
======================================

This release includes a modified APRS RX app with integrated frequency selection.

Files:
------
1. Labs_f4iae_firmware.bin (111 KB) - Firmware (optional, for reference)
2. aprsrx.app (1.7 KB) - Modified APRS RX app with frequency menu
3. freqmanager.app (3.4 KB) - Original freqmanager app (kept for reference)

APRS RX App - New Features:
---------------------------
At startup, the app displays a frequency selection menu with:
- VFO (current frequency)
- MR 1 through MR 28 (28 memory channels) with CUSTOMIZABLE labels

Navigation:
- UP/DOWN: Scroll through frequency list
- MENU: Select frequency and start APRS RX
- EXIT: Cancel and return to app menu

After selection:
- The app displays the selected frequency NAME and NUMBER
- Press MENU again to reopen the frequency menu
- Press EXIT to return to app menu

Customizing Memory Names:
------------------------
The memory channel names are defined in aprsrx_app_minimal.c as:

static const char *mr_names[NUM_MR_CHANNELS] = {
    "APRS 144.800",   // MR 1
    "APRS 144.390",   // MR 2
    ...
    "MR 28"           // MR 28
};

Edit this array to match your actual memory channel names configured via
k5k1prog-mac or other tools.

Installation:
------------
1. Install aprsrx.app using UV Studio or similar tool
2. Run the app from the apps menu (F+7)
3. Select your desired frequency from the list

Technical Details:
----------------
- App size: 1684 bytes (41% of 4 KB limit)
- Menu displays 4 items at a time with scroll indicators
- Default selection: VFO (index 0)
- Memory channels: MR 1-28 (indices 1-28)
- Customizable labels for each memory channel
