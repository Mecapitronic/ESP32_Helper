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
├── .wokwi/                     # Configuration du simulateur wokwi
├── bin/                        # Dossier de sortie des firmwares compilés pour wokwi
├── data/                       # Fichiers exemples pour SPIFFS
├── examples/                   # Exemples d'utilisation
├── include/                    # En-têtes publics (.h)
├── src/                        # Implémentations (.cpp)
├── scripts/                    # Extra Script PlatformIO
├── library.json                # Description de la lib pour intégration via platformIO
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
- 📄 `Example8_JSON`          - lecture, deserialisation, modification et serialisation d'un fichier JSON simple situé dans la SPIFFS.

### Ajouter du texte dans un fichier SPIFFS

La commande historique `SPIFFSAppendFile:<fichier>:<message>` convient aux messages courts. Pour ajouter un texte plus long contenant `:`, `;`, des virgules ou des retours à la ligne, la commande série `SPIFFSAppendASCII` reçoit le corps selon une longueur annoncée :

1. Envoyez l'en-tête `SPIFFSAppendASCII:<fichier>:<nombre-char>` terminé par newline (`\n`).
2. Le firmware affiche `Starting SPIFFS ASCII Append session for file: ... with size ...`.
3. Envoyez exactement le nombre annoncé de caractères, sans terminateur supplémentaire.

Le contenu est ajouté au fichier et ces caractères sont lus comme données, pas comme séparateurs. Le protocole est prévu pour du texte ASCII; **le firmware actuel ne valide pas les caractères non ASCII**. La taille maximale est de 1024 caractères. Le timeout est de 10 secondes d'inactivité : à son expiration, le texte reçu jusque-là est abandonné et la réception normale des commandes reprend. Tout octet restant de l'ancien transfert peut alors être interprété comme une commande; en cas de timeout, arrêtez l'envoi et resynchronisez la liaison avant de réessayer.

Cette implémentation fonctionne sur la liaison série UART; le transfert ASCII sur TCP Wi-Fi n'est pas activé.

Pour compiler un exemple, lancez la ligne de commande PlatformIO en ciblant l'environnement correspondant :
```
pio run -e Example1_FastStartup
```
Vous pouvez également compiler tous les exemples avec cette ligne de commande :
```
pio run
```