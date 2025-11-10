# 🛠️ Librairie ESP32_Helper

Librairie d'utilitaires pour projets ESP32 : 
- ⚡ tâches/threads
- 📟 impression/console
- \>_ systeme de commande 
- 📊 télémetrie (Teleplot)
- 🔍 debugger pas à pas
- 📡 gestion Wi‑Fi/OTA
- 💾 enregistrement de préférences (NVS)
- 📂 gestion du système de fichiers (SPIFFS)

Ce dépôt contient la bibliothèque source (dossiers `include/` et `src/`) et plusieurs exemples prêts à l'emploi pour PlatformIO.

## 📁 Structure de la bibliothèque

```bash
ESP32_Helper/
├── .github/                    # Configuration GitHub Actions
├── .wokwi/                     # Configuration su simulateur wokwi
├── bin/                        # Dossier de sortie des firmwares compilés pour wokwi
├── data/                       # Fichiers exemples pour SPIFFS
├── examples/                   # Exemples d'utilisation
├── include/                    # En-têtes publics (.h)
├── src/                        # Implémentations (.cpp)
├── scripts/                    # Extra Script PlatformIO
├── platformio.ini              # Configuration PlatformIO
└── README.md                   # Documentation
```

## 🔧 Intégration dans un projet PlatformIO (ESP32)

1. Copier la lib dans le projet

📌 Option A - en tant que bibliothèque locale
Copier le dossier `ESP32_Helper` dans le dossier `lib/` de votre projet PlatformIO (ou l'ajouter comme submodule)
   - `lib/ESP32_Helper` (la structure `include/` et `src/` sera conservé à l'interieur de la lib).

📌 Option B - en tant que bibliothèque distante : Ajouter via `lib_deps` dans `platformio.ini` :
```
lib_deps =
    https://github.com/Mecapitronic/ESP32_Helper.git
```

2. Inclure dans votre sketch :

```cpp
#include "ESP32_Helper.h"
```

3. Appeler l'initialisation dans `setup()` :

```cpp
ESP32_Helper::Initialisation();
```

### 🔨 Flags de compilation utiles

- `-D WITH_WIFI` : active le module Wi‑Fi (implémentation dans `Wifi_Helper`).
- `-D WITH_OTA` : active l'OTA si supporté par l'exemple.
- `-D SIMULATOR` : active `MockSPIFFS` (utile pour exécuter les exemples SPIFFS sans support matériel SPIFFS).

Ces flags sont généralement définis dans les environnements de `platformio.ini` fournis dans ce dépôt.

## 📚 Exemples fournis

Chaque sous-dossier de `examples/` contient un projet PlatformIO minimal :

- 🚀 `Example1_FastStartup`   - démarrage minimal, utile pour tests rapides sans wifi.
- 🔍 `Example2_Debug`         - démonstration des fonctions de debug/Logger.
- 📡 `Example3_WithWifi`      - montre la configuration Wi‑Fi (nécessite `WITH_WIFI`) et l'usage d'OTA si activé.
- 💬 `Example4_HandleCommand` - exemple de réception/traitement de commandes (format attendu et handler).
- 💾 `Example5_Preferences`   - montre comment lire/écrire des préférences via `Preferences_Helper`.
- 📊 `Example6_Teleplot`      - démonstration de la sortie Teleplot/télémétrie.
- 📂 `Example7_SPIFFS`        - opérations sur fichiers (liste, lecture, écriture). Compile l'environnement qui active `SIMULATOR` pour utiliser `MockSPIFFS` si nécessaire.

Pour compiler un exemple, lancez la ligne de commande PlatformIO en ciblant l'environnement correspondant :
```
pio run -e Example1_FastStartup
```
Vous pouvez également compiler tous les exemples avec cette ligne de commande :
```
pio run
```