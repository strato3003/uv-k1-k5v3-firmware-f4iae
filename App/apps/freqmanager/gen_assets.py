#!/usr/bin/env python3
# Copyright 2026 JN Martineau F4IAE
# https://github.com/jnmartineau
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.

import sys, os

# Generate empty assets for freqmanager app

def main():
    # Create empty assets.bin
    with open('freqmanager_assets.bin', 'wb') as f:
        pass  # Empty file
    
    # Generate empty assets.h
    with open('freqmanager_assets.h', 'w') as f:
        f.write("""/* Copyright 2026 JN Martineau F4IAE
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

#ifndef FREQMANAGER_ASSETS_H
#define FREQMANAGER_ASSETS_H

#define FREQMANAGER_ASSETS_SIZE 0

#endif /* FREQMANAGER_ASSETS_H */
""")

    print("Generated empty assets for freqmanager app")

if __name__ == '__main__':
    main()
