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
- 💬 `Example4_CustomCommand` - exemple de réception/traitement de commandes (format attendu et handler).
- 💾 `Example5_Preferences`   - montre comment lire/écrire des préférences via `Preferences_Helper`.
- 📊 `Example6_Teleplot`      - démonstration de la sortie Teleplot/télémétrie.
- 📂 `Example7_SPIFFS`        - opérations sur fichiers (liste, lecture, écriture). Compile l'environnement qui active `SIMULATOR` pour utiliser `MockSPIFFS` si nécessaire.
- 📄 `Example8_JSON`          - lecture, deserialisation, modification et serialisation d'un fichier JSON simple situé dans la SPIFFS.

## Aide et syntaxe des commandes

Envoyer `Help` pour afficher la légende et les aides des commandes enregistrées.
La liste dépend des modules initialisés et des options de compilation, notamment
`WITH_WIFI`. `Help` ne modifie aucun paramètre.

Toutes les aides commencent par la syntaxe, suivie de ` - Description` :

```text
Commande:<nom:type@unite>(:nom:type@unite=defaut) - Description
```

| Notation | Lecture |
|---|---|
| `<nom:type>` | Paramètre obligatoire ; les crochets `[]` ne sont pas utilisés. |
| `(:nom:type)` | Paramètre facultatif ; omettre aussi son séparateur `:`. |
| `@unite` | Unité attendue, omise si sans objet. |
| `=defaut` | Valeur utilisée en l'absence du paramètre, si applicable. |
| `int` | Entier signé 32 bits, de -2147483648 à 2147483647, sauf contrainte plus restrictive. |
| `string` | Texte non vide, dans les limites du parseur ci-dessous. |
| `bool` | Entier `0` (faux/désactivé) ou `1` (vrai/activé), pas `true`/`false`. |
| `int\|string` | Accepte un entier ou du texte ; le contenu détermine le type. |
| `float` | Nombre décimal, uniquement si le gestionnaire le convertit explicitement ; non pris en charge nativement. |

Unités : `ms` (millisecondes), `s` (secondes), `%` (pourcentage),
`deg` (degrés), `mm` (millimètres), `mm/s`, `bit/s` (débit série),
`byte` (octets). Une valeur se transmet **sans son unité**.
Les plages, valeurs particulières et autres contraintes figurent dans la description.
Ces annotations documentent le gestionnaire ; elles ne valident pas automatiquement
les arguments et ne changent pas le comportement des commandes.

Exemples :

```text
Delay:<duration:int@ms> - Delay execution; duration > 0.
PrefShow:<key:string>:<defaultValue:int|string> - Read a preference.
SPIFFSListFiles(:filter:string) - List files; omit filter to list all files.
SPIFFSAppendFile:<fileName:string>:<msg:string>(:createFileIfNotExists:bool=0) - Append a short message.
```

Commandes correspondantes à saisir, sans chevrons, parenthèses ni annotations :

```text
Delay:1000
PrefShow:Speed:50
SPIFFSListFiles
SPIFFSListFiles:.txt
SPIFFSAppendFile:hello.txt:Bonjour:1
```

Pour une commande ayant plusieurs usages, écrire plusieurs signatures dans la même
aide, séparées par un retour à la ligne. Cela évite de laisser croire qu'un argument
peut être fourni sans celui dont il dépend. Par exemple :

```text
AX12Torque - Show raw torque limits of all servos (0..1023).
AX12Torque:<id:int> - Show raw torque limit of one logical servo id (0..1023).
AX12Torque:<id:int>:<torque:int@%> - Show raw torque limit, then set it in percent; torque: 0..100.
```

### Déclarer une aide

```cpp
ESP32_Helper::RegisterCommand(
    "MyCommand",
    "MyCommand:<value:int> - Set a value; value >= 0. Example: MyCommand:5",
    FunctionCommand(
        if (cmd.hasData() && cmd.data[0] >= 0) {
            Printer::println("Value: %ld", static_cast<long>(cmd.data[0]));
        } else {
            Printer::println("MyCommand requires a non-negative integer.");
        }
    ));
```

Utiliser la même convention dans les modules, bibliothèques et exemples.
Ne déclarer `float` que si le gestionnaire sait convertir et vérifier une chaîne
décimale : le parseur actuel classe `12.5` comme chaîne, pas comme donnée numérique.

### Transport et limites actuelles

- Séparer les arguments par `:` et terminer une commande par un retour à la ligne
  (`\n`) ou `;`. Exemple d'enchaînement : `Ping;Delay:1000;Ping;`.
- Les noms de commandes et chaque argument texte sont limités à 24 caractères.
  Le tampon d'entrée est de 64 caractères, terminateur compris : une commande
  complète hors terminateur ne doit pas dépasser 63 caractères.
- Une commande peut contenir au maximum six entiers et deux chaînes. Ils sont
  stockés séparément ; les signatures documentent l'ordre à employer.
- Une valeur entièrement numérique est un entier, même si l'aide attend `string`.
  Ainsi, `PrefSave:Code:123` enregistre un entier, pas la chaîne `"123"`.
- Aucun mécanisme de guillemets ou d'échappement n'est prévu. Les guillemets
  seraient des caractères du texte ; `:` et `;` restent des séparateurs.
  Ne pas ajouter d'espaces autour des entiers : ils seraient classés comme texte.
- Les paramètres `string` ne peuvent donc pas contenir `:`, `;` ou un retour à la
  ligne. Pour un contenu de fichier plus long, utiliser le transfert série
  `SPIFFS_ASCII` décrit ci-dessous.
- Les aides Wi-Fi annoncent la contrainte de configuration de 8 à 63 caractères
  pour le SSID et le mot de passe, mais la commande console reste limitée à
  24 caractères par argument texte.

### Ajouter du texte dans un fichier SPIFFS

La commande `SPIFFSAppendFile:<fileName:string>:<msg:string>(:createFileIfNotExists:bool=0)`
convient aux messages courts. Pour ajouter un texte plus long contenant `:`, `;`,
des virgules ou des retours à la ligne, utiliser :

```text
SPIFFS_ASCII:<fileName:string>:<size:int@byte>(:overwrite:bool=0)
```

1. Envoyez l'en-tête terminé par newline (`\n`), par exemple `SPIFFS_ASCII:hello.txt:11:1`.
2. Le firmware affiche `Starting SPIFFS ASCII Append session for file: ... with size ...`.
3. Envoyez exactement le nombre annoncé d'octets, sans terminateur supplémentaire
   (par exemple `Hello World`, soit 11 octets ASCII).

`overwrite=0` (ou omis) ajoute le contenu ; `overwrite=1` remplace le fichier.
Le fichier est créé s'il n'existe pas. Le message de démarrage contient actuellement
le mot `Append` dans les deux cas ; le message précédent indique le mode retenu.

Le contenu est lu comme données, pas comme séparateurs. Le protocole est prévu
pour du texte ASCII ; **le firmware actuel ne valide pas les caractères non ASCII**.
La taille doit être comprise entre 1 et 1024 octets. Le timeout est de 10000 ms
d'inactivité : à son expiration, le texte reçu jusque-là est abandonné et la
réception normale des commandes reprend. Tout octet restant de l'ancien transfert
peut alors être interprété comme une commande ; en cas de timeout, arrêtez l'envoi
et resynchronisez la liaison avant de réessayer.

Cette implémentation fonctionne sur la liaison série UART; le transfert ASCII sur TCP Wi-Fi n'est pas activé.

## Compilation des exemples

Pour compiler un exemple, lancez la ligne de commande PlatformIO en ciblant l'environnement correspondant :
```
pio run -e Example1_FastStartup
```
Vous pouvez également compiler tous les exemples avec cette ligne de commande :
```
pio run
```