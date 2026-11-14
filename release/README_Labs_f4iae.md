# Labs_f4iae Firmware v6.3.0

## Description

Firmware Labs v6.3.0 modifié pour F4IAE avec gestion des fréquences et applications.

## Contenu

### Firmware
- `Labs_f4iae_firmware.bin` - Firmware principal basé sur Labs v6.3.0
  - Intègre le module `freq_manager` pour la gestion des listes de fréquences
  - Compatible avec les applications overlay

### Applications Overlay
- `freqmanager.app` (3452B, 84% de 4K) - Gestion complète des fréquences et applications
  - Navigation : Menu/Exit/</> pour sélection
  - Saisie : Numpad pour les fréquences, </> pour le texte (ASCII 127)
  - Fonctions : Add/Remove/Edit/List pour les listes category,freq,desc et category,appname

- `memo.app` (1680B, 41% de 4K) - Sélecteur de fréquences simple
  - Affiche 40 fréquences personnalisées depuis uvk5-freq.csv
  - Navigation : UP/DOWN pour se déplacer
  - Sélection : MENU pour choisir et changer le VFO
  - Affichage : 4 items visibles, scroll normal, footer minimal

- `aprsrx.app` - Application APRS RX avec menu de sélection de fréquence

## Installation

### Méthode 1 : Via UV Studio
1. Lancer UV Studio
2. Choisir "Flash Firmware"
3. Sélectionner `Labs_f4iae_firmware.bin`
4. Flasher le firmware
5. Installer les applications .app via "Install App"

### Méthode 2 : Via DFU
1. Brancher le radio en mode DFU
2. Utiliser dfu-util pour flasher le firmware
3. Installer les applications via l'outil approprié

## Utilisation

### Sélection de fréquence (memo.app)
- Appuyez sur **F+7** pour ouvrir le menu des applications
- Sélectionnez **memo**
- Utilisez **UP/DOWN** pour naviguer dans la liste
- Appuyez sur **MENU** pour sélectionner une fréquence et changer le VFO
- Appuyez sur **EXIT** pour quitter

### Gestion avancée (freqmanager.app)
- Appuyez sur **F+7** pour ouvrir le menu des applications
- Sélectionnez **freqmanager**
- Suivez les instructions à l'écran pour :
  - Ajouter une nouvelle fréquence
  - Modifier une fréquence existante
  - Supprimer une fréquence
  - Lister toutes les fréquences

## Structure des données

### Liste des fréquences (category,freq,desc)
Format : `NOM FREQUENCY`
Exemple : `APRS 144.800`

40 fréquences préconfigurées (sans HF) :
- RASEC85 145.475 / 145.212
- TACTUHF 444.625
- INTERSV 157.575
- F5ZWP_U 439.612
- F5ZWQ_U 430.212
- F1ZOQRX 430.475
- APRS 144.800
- DETRESSE 121.500
- MIL243 243.000
- SAR406A 406.025 / SAR406B 406.037
- VHF16 156.800 / VHF13 156.650 / VHF70 156.525 / VHF9 156.450 / VHF80 156.025
- AIS1 161.975 / AIS2 162.025
- NT-ATIS 126.930 / NT-TWR 118.655 / NT-SOL 121.655 / NT-APP 124.250
- RY-AFIS 119.905 / LS-AFIS 123.350 / FC-AFIS 119.765 / TR-TWR 118.100
- SA-AFIS 120.605
- ULM1235 123.500 / ULM1253 125.335
- FIS1228 122.800
- ACARS1-5 131.725 / 131.525 / 136.900 / 130.025 / 129.125
- VDL2 136.975

### Liste des applications (category,appname)
À compléter selon les besoins.

## Compilation

Pour recompiler le firmware :
```bash
./build_labs_f4iae.sh
```

Pour recompiler les applications :
```bash
cd App/apps/memo && ./build.sh
cd App/apps/freqmanager && ./build.sh
```

## Notes

- Basé sur Labs v6.3.0
- Éditon spécial F4IAE
- Taille firmware : 114 Ko
- Taille applications : 4K max chacune

## Auteur

JN Martineau F4IAE
https://github.com/jnmartineau

## Licence

Apache License 2.0
