# Memo App - Frequency Selector

A simple overlay app that displays a list of your configured frequencies from uvk5-freq.csv and allows you to select one.

## Features

- Displays 28 frequencies with short labels and MHz values
- Menu navigation with UP/DOWN keys
- Selection with MENU key
- Shows selected frequency name and MHz value on main screen
- Exit with EXIT key

## Data Source

The app uses the first 28 entries from your `uvk5-freq.csv` file located in the project root.

### CSV Format Expected:
```csv
Location,Name,Frequency,Duplex,Offset,Tone,...
1,RASEC85,145.475000,...
2,RASEC85B,145.212500,...
...
```

### Data in App:
The app extracts `Name` and `Frequency` columns and formats them as:
```
"RASEC85,145.475"
```

## How to Update Frequencies

1. Edit `/Users/jnmartineau/develop/uv-k1-k5v3-firmware-f4iae/uvk5-freq.csv`
2. Update the first 28 entries with your desired frequencies
3. Update the `freq_items` array in `freqmanager_app.c` to match your CSV data
4. Rebuild: `./build.sh`
5. Copy `freqmanager.app` to your radio

## Navigation

### In Menu:
- **UP** - Move selection up
- **DOWN** - Move selection down
- **MENU** - Confirm selection and show main screen
- **EXIT** - Exit app

### In Main Screen:
- **MENU** - Return to frequency menu
- **EXIT** - Exit app

## Display Layout

### Menu Screen:
```
+------------------+
| Freq Manager     |
| RASEC85,145.475  |  <- Selected (inverse)
| RASEC85B,145.212 |
| TACTUHF,444.625  |
| INTERSV,157.575  |
| ^                |  <- Scroll up indicator
| UP/DN:Move       |
| MENU:Select      |
| EXIT:Back        |
+------------------+
```

### Main Screen (After Selection):
```
+------------------+
| Freq Manager F4IAE|
| RASEC85          |  <- Name
| 145.475          |  <- Frequency
|                  |  <- #1 (top right)
|------------------|
| MENU:Menu        |
| EXIT:Back        |
| v1.0             |
+------------------+
```

## Technical Details

- **App size**: 1824 bytes (44% of 4 KiB limit)
- **Language**: C
- **Dependencies**: app_api.h
- **Build script**: build.sh
- **Output**: freqmanager.app

## Current Frequency List (First 28 from uvk5-freq.csv)

1. RASEC85,145.475
2. RASEC85B,145.212
3. TACTUHF,444.625
4. INTERSV,157.575
5. F5ZWP_U,439.612
6. F5ZWQ_U,430.212
7. F1ZOQRX,430.475
8. APRS,144.800
9. HF3760,3.760
10. HF7110,7.110
11. HF14300,14.300
12. DETRESSE,121.500
13. MIL243,243.000
14. SAR406A,406.025
15. SAR406B,406.037
16. VHF16,156.800
17. VHF13,156.650
18. VHF70,156.525
19. VHF9,156.450
20. VHF80,156.025
21. AIS1,161.975
22. AIS2,162.025
23. NT-ATIS,126.930
24. NT-TWR,118.655
25. NT-SOL,121.655
26. NT-APP,124.250
27. RY-AFIS,119.905
28. LS-AFIS,123.350

## Build Instructions

```bash
cd /Users/jnmartineau/develop/uv-k1-k5v3-firmware-f4iae/App/apps/freqmanager
./build.sh
```

The compiled `freqmanager.app` will be in the same directory.

## Installation

Copy `freqmanager.app` to your radio using UV Studio or similar tool.
Run the app from the apps menu (F+7).
