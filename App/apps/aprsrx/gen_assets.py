#!/usr/bin/env python3
# APRS RX read-only assets: minimal version for frequency selection
#
# Only includes essential UI texts. The full APRS demodulator assets are
# removed to fit within the 4 KiB overlay RAM limit.
#
#   ./gen_assets.py aprsrx_assets.bin aprsrx_assets.h
import os, sys
sys.dont_write_bytecode = True
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, ".."))
from app_assets import Assets

TITLE = "APRS RX F4IAE"                     # the version is in the .app header
WAIT_CAPS = "WAIT"                          # status-bar capsule until a frame

UI = [
    ("T_TITLE",  TITLE),
    ("T_WAIT",   WAIT_CAPS),
    ("T_DBM",    "dBm"),
    ("T_KMH",    "km/h "),
    ("T_CUSTOM", "Custom-"),
]

a = Assets("APRSRX")
for name, s in UI:
    pad = -(len(s) + 1) % 4                 # keep every offset word-aligned
    a.text(name, s + "\0" * pad)

a.const("T_TITLE_CHARS", len(TITLE))
a.const("T_WAIT_CHARS", len(WAIT_CAPS))

if __name__ == "__main__":
    a.main()
