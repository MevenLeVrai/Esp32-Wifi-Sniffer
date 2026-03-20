# 🛰️ ESP32 WiFi Sniffer

![ESP32](https://img.shields.io/badge/Board-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-green)
![License](https://img.shields.io/badge/Use-Educational-lightgrey)

---

## 🇫🇷 Version Française

### 🔧 Présentation

Ce projet implémente un **sniffer Wi-Fi 802.11** basé sur un **ESP32**.
Il capture les paquets sans fil en mode promiscuité et permet d'analyser le trafic ou de suivre les appareils à proximité.
Outil idéal pour l'apprentissage de la **cybersécurité**, du **Wi-Fi bas niveau** ou des **protocoles IoT**.

---

### ⚙️ Configuration des Modes

Active ou désactive les fonctionnalités souhaitées dans le code source :

#### 🔹 Mode 1 — Afficher Chaque Paquet

```cpp
#define SHOW_ALL_PACKETS 1
#define TRACK_UNIQUE_MACS 0
```

Affiche chaque paquet capturé avec ses adresses source et destination.

#### 🔹 Mode 2 — Tracker les Appareils Uniques

```cpp
#define SHOW_ALL_PACKETS 0
#define TRACK_UNIQUE_MACS 1
```

Affiche des statistiques toutes les 10 s avec la liste des appareils détectés.

#### 🔹 Mode 3 — Combinaison Complète

```cpp
#define SHOW_ALL_PACKETS 1
#define TRACK_UNIQUE_MACS 1
```

Combine les deux modes : affichage en direct **+** statistiques périodiques.

---

### 🖥️ Exemple de Sortie Console

```
╔════════════════════════════════════════════════════════════╗
║  📊 STATISTIQUES EN TEMPS RÉEL
╚════════════════════════════════════════════════════════════╝
Total paquets:     1542
  → Management:    892
  → Data:          650
  → Miscellaneous: 0
Canal actuel:       5

╔════════════════════════════════════════════════════════════╗
║  📱 APPAREILS DÉTECTÉS À PROXIMITÉ
╚════════════════════════════════════════════════════════════╝
Total unique: 8 appareils

[ 1] MAC: a4:05:48:5f:c3:e8 | RSSI: -45 dBm | CHAN: 6  | Pkts: 235 | Type: MGMT
[ 2] MAC: 50:3e:aa:24:12:50 | RSSI: -62 dBm | CHAN: 6  | Pkts: 178 | Type: DATA
[ 3] MAC: 74:aa:8a:9e:f1:02 | RSSI: -72 dBm | CHAN: 11 | Pkts: 45  | Type: MGMT
```

---

### 🧩 Fonctions Avancées et Filtres

#### 1️⃣ Filtrer par Adresse MAC

```cpp
uint8_t target_mac[] = {0xa4, 0x05, 0x17, 0x5f, 0xc3, 0xe8};
if (memcmp(hdr->addr2, target_mac, 6) == 0) {
    digitalWrite(LED_GPIO_PIN, HIGH);
}
```

#### 2️⃣ Filtrer par Puissance du Signal

```cpp
if (ppkt->rx_ctrl.rssi > -70) {  // Signaux forts uniquement
    // Traiter le paquet
}
```

#### 3️⃣ Capturer Uniquement les Trames de Gestion

```cpp
#define SNIFF_MGMT_ONLY 1
if (SNIFF_MGMT_ONLY && type != WIFI_PKT_MGMT) return;
```

---

### 📉 Analyse des Données

#### 🧭 Niveaux RSSI

| Niveau     | Interprétation | Portée approx. |
|:-----------|:---------------|:---------------|
| -30 dBm    | Très fort       | < 5 m          |
| -67 dBm    | Bon             | 5 – 10 m       |
| -70 dBm    | Acceptable      | 10 – 15 m      |
| -80 dBm    | Faible          | > 15 m         |
| -90 dBm    | Très faible     | Portée limite  |

#### 📡 Types de Paquets 802.11

| Type     | Description                                        |
|:---------|:---------------------------------------------------|
| **MGMT** | Trames de gestion (beacon, probe, association)     |
| **DATA** | Données utilisateur                                |
| **MISC** | Paquets divers                                     |

---

### 🚀 Utilisation Rapide

- **Matériel** : ESP32 DevKitC / NodeMCU‑32S
- **IDE recommandé** : Arduino IDE ou PlatformIO
- **Baud rate** : `115200`
- **Bibliothèque** : `esp_wifi.h`

**Étapes :**

1. Flasher le code sur votre ESP32
2. Ouvrir le moniteur série
3. Observer les trames Wi‑Fi capturées

---

### ⚠️ Avertissement

> 🔒 Ce projet est uniquement destiné à un usage **éducatif et expérimental**.
> La capture de trames sans autorisation est **illégale** selon le RGPD et la loi européenne.

---

## 🇬🇧 English Version

### 🔧 About

This project implements an **802.11 Wi-Fi sniffer** based on the **ESP32** microcontroller.
It captures wireless packets in monitor mode, allowing analysis of nearby traffic and device behavior.
Perfect for learning **cybersecurity**, **wireless protocols**, or **IoT signal analysis**.

---

### ⚙️ Mode Configuration

#### 🔹 Mode 1 — Show Every Packet

```cpp
#define SHOW_ALL_PACKETS 1
#define TRACK_UNIQUE_MACS 0
```

Displays all captured packets with MAC source and destination.

#### 🔹 Mode 2 — Track Unique Devices

```cpp
#define SHOW_ALL_PACKETS 0
#define TRACK_UNIQUE_MACS 1
```

Displays statistics every 10 s with the list of detected devices.

#### 🔹 Mode 3 — Full Combination

```cpp
#define SHOW_ALL_PACKETS 1
#define TRACK_UNIQUE_MACS 1
```

Combines live packet display **and** device statistics.

---

### 🖥️ Example Console Output

```
╔════════════════════════════════════════════════════════════╗
║  📊 REAL‑TIME STATISTICS
╚════════════════════════════════════════════════════════════╝
Total packets:     1542
  → Management:    892
  → Data:          650
  → Miscellaneous: 0
Current channel:    5

╔════════════════════════════════════════════════════════════╗
║  📱 DETECTED NEARBY DEVICES
╚════════════════════════════════════════════════════════════╝
Total unique: 8 devices

[ 1] MAC: a4:05:48:5f:c3:e8 | RSSI: -45 dBm | CHAN: 6  | Pkts: 235 | Type: MGMT
[ 2] MAC: 50:3e:aa:24:12:50 | RSSI: -62 dBm | CHAN: 6  | Pkts: 178 | Type: DATA
[ 3] MAC: 74:aa:8a:9e:f1:02 | RSSI: -72 dBm | CHAN: 11 | Pkts: 45  | Type: MGMT
```

---

### 🧩 Advanced Filters

#### 1️⃣ Filter by MAC Address

```cpp
uint8_t target_mac[] = {0xa4, 0x05, 0x17, 0x5f, 0xc3, 0xe8};
if (memcmp(hdr->addr2, target_mac, 6) == 0) {
    digitalWrite(LED_GPIO_PIN, HIGH);
}
```

#### 2️⃣ Filter by Signal Strength (RSSI)

```cpp
if (ppkt->rx_ctrl.rssi > -70) {
    // Process only strong signals
}
```

#### 3️⃣ Capture Only Management Frames

```cpp
#define SNIFF_MGMT_ONLY 1
if (SNIFF_MGMT_ONLY && type != WIFI_PKT_MGMT) return;
```

---

### 📉 Data Interpretation

#### 🧭 Signal Level (RSSI)

| Level      | Meaning      | Approx. range  |
|:-----------|:-------------|:---------------|
| -30 dBm    | Very strong  | < 5 m          |
| -67 dBm    | Good         | 5 – 10 m       |
| -70 dBm    | Fair         | 10 – 15 m      |
| -80 dBm    | Weak         | > 15 m         |
| -90 dBm    | Very weak    | Edge of range  |

#### 📡 802.11 Packet Types

| Type     | Description                                            |
|:---------|:-------------------------------------------------------|
| **MGMT** | Management frames (beacon, probe, association)         |
| **DATA** | User data packets                                      |
| **MISC** | Miscellaneous frames                                   |

---

### 🚀 Quick Start

- **Board:** ESP32 DevKitC / NodeMCU‑32S
- **Recommended IDE:** Arduino IDE / PlatformIO
- **Baud Rate:** `115200`
- **Library:** `esp_wifi.h`

**Steps:**

1. Flash the firmware onto your ESP32
2. Open the serial monitor
3. Observe real‑time Wi‑Fi traffic

---

### ⚠️ Legal Notice

> This project is for **educational and research purposes only**.
> Unauthorized interception of Wi‑Fi traffic is **illegal under GDPR and EU law**.

---