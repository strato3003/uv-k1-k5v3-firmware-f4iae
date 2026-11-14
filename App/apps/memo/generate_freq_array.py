#!/usr/bin/env python3
"""
Generate the freq_items array for freqmanager_app.c from uvk5-freq.csv

Usage:
    python3 generate_freq_array.py > freq_items.txt
    Then paste into freqmanager_app.c
"""

import csv
import sys

# Path to CSV file
CSV_FILE = "../../../uvk5-freq.csv"
# Maximum items for overlay app (will filter out HF)
NUM_ITEMS = 40

def main():
    try:
        with open(CSV_FILE, 'r') as f:
            reader = csv.DictReader(f)
            rows = list(reader)
            
            if len(rows) < NUM_ITEMS:
                print(f"Warning: CSV has only {len(rows)} entries, but we need {NUM_ITEMS}")
            
            # Filter out HF frequencies (< 30 MHz) and extract first NUM_ITEMS
            filtered_items = []
            for row in rows:
                freq_str = row.get('Frequency', '0.000')
                try:
                    freq_mhz = float(freq_str)
                    if freq_mhz >= 30.0:  # Only VHF/UHF and above
                        filtered_items.append(row)
                except ValueError:
                    filtered_items.append(row)
            
            items = filtered_items[:NUM_ITEMS]
            
            # Generate C array
            print(f"// Generated from {CSV_FILE} - Non-HF frequencies ({len(items)} entries)")
            print(f"static const char *freq_items[{len(items)}] = {{")
            
            for i, row in enumerate(items):
                name = row.get('Name', 'MR_' + str(i+1))
                freq = row.get('Frequency', '0.000')
                
                # Remove .000 from frequency if present (145.475000 -> 145.475)
                if freq.endswith('.000'):
                    freq = freq[:-4]
                elif freq.endswith('.00'):
                    freq = freq[:-3]
                
                # Truncate name if too long (max ~15 chars to fit on display)
                if len(name) > 15:
                    name = name[:15]
                
                # Format: "Name Freq" (space separator, no comma)
                item = f'    "{name} {freq}",
                
                # Add comment with position
                if i < NUM_ITEMS - 1:
                    item += f"  // {i+1}"
                else:
                    item += f"  // {i+1}"
                
                print(item)
            
            print("};")
            
    except FileNotFoundError:
        print(f"Error: CSV file not found at {CSV_FILE}")
        print(f"Please ensure uvk5-freq.csv exists in the project root directory")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
