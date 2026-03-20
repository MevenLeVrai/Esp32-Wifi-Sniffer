#include "WiFi.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

// ========== CONFIGURATION ==========
#define WIFI_CHANNEL_MAX 13
#define WIFI_CHANNEL_SWITCH_INTERVAL 1000  // 1 seconde entre les canaux
#define LED_GPIO_PIN 2
#define MAX_TRACKED_DEVICES 50  // Nombre max d'appareils à tracker

// ========== MODE DE SNIFFING ==========
#define SNIFF_MGMT_ONLY 0    // 0 = tous les types, 1 = management seulement
#define SHOW_ALL_PACKETS 1   // 1 = afficher chaque paquet, 0 = statistiques seul
#define TRACK_UNIQUE_MACS 1  // 1 = tracker les adresses MAC uniques

// ========== STRUCTURES ==========

typedef struct {
  uint8_t mac[6];
  int rssi;
  uint8_t channel;
  unsigned long last_seen;
  unsigned int packet_count;
  uint8_t type;  // 1 = MGMT, 2 = DATA, 3 = MISC
} tracked_device_t;

typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6];    // Destinataire
  uint8_t addr2[6];    // Émetteur
  uint8_t addr3[6];    // Filtrage
  unsigned sequence_ctrl:16;
  uint8_t addr4[6];
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0];
} wifi_ieee80211_packet_t;

// ========== VARIABLES GLOBALES ==========

static uint8_t current_channel = 1;
static unsigned int total_packets = 0;
static unsigned int mgmt_packets = 0, data_packets = 0, misc_packets = 0;
static tracked_device_t tracked_devices[MAX_TRACKED_DEVICES];
static int device_count = 0;

// ========== FONCTION POUR FORMATER MAC ==========

void print_mac(uint8_t *mac) {
  printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// ========== FONCTION POUR TROUVER ADRESSE MAC EXISTANTE ==========

int find_device_index(uint8_t *mac) {
  for (int i = 0; i < device_count; i++) {
    if (memcmp(tracked_devices[i].mac, mac, 6) == 0) {
      return i;
    }
  }
  return -1;
}

// ========== AJOUTER OU METTRE À JOUR APPAREIL ==========

void track_device(uint8_t *mac, int rssi, uint8_t channel, uint8_t type) {
  int idx = find_device_index(mac);
  
  if (idx != -1) {
    // Appareil existant : mettre à jour
    tracked_devices[idx].rssi = rssi;
    tracked_devices[idx].channel = channel;
    tracked_devices[idx].last_seen = millis();
    tracked_devices[idx].packet_count++;
  } else if (device_count < MAX_TRACKED_DEVICES) {
    // Nouvel appareil
    memcpy(tracked_devices[device_count].mac, mac, 6);
    tracked_devices[device_count].rssi = rssi;
    tracked_devices[device_count].channel = channel;
    tracked_devices[device_count].last_seen = millis();
    tracked_devices[device_count].packet_count = 1;
    tracked_devices[device_count].type = type;
    device_count++;
  }
}

// ========== AFFICHER LES APPAREILS TRACÉS ==========

void print_tracked_devices() {
  Serial.println("\n╔════════════════════════════════════════════════════════════╗");
  Serial.println("║  📱 APPAREILS DÉTECTÉS À PROXIMITÉ");
  Serial.println("╚════════════════════════════════════════════════════════════╝");
  Serial.printf("Total unique: %d appareils\n\n", device_count);
  
  for (int i = 0; i < device_count; i++) {
    printf("[%2d] MAC: ", i+1);
    print_mac(tracked_devices[i].mac);
    printf(" | RSSI: %3d dBm | CHAN: %2d | Pkts: %4d | Type: %s\n",
      tracked_devices[i].rssi,
      tracked_devices[i].channel,
      tracked_devices[i].packet_count,
      tracked_devices[i].type == 1 ? "MGMT" : (tracked_devices[i].type == 2 ? "DATA" : "MISC")
    );
  }
  Serial.println();
}

// ========== CONVERSION TYPE PAQUET ==========

const char *packet_type_to_str(wifi_promiscuous_pkt_type_t type, uint8_t *type_num) {
  switch(type) {
    case WIFI_PKT_MGMT:
      mgmt_packets++;
      *type_num = 1;
      return "MGMT";
    case WIFI_PKT_DATA:
      data_packets++;
      *type_num = 2;
      return "DATA";
    default:
    case WIFI_PKT_MISC:
      misc_packets++;
      *type_num = 3;
      return "MISC";
  }
}

// ========== CALLBACK PRINCIPAL ==========

void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type) {
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
  
  total_packets++;
  uint8_t type_num;
  const char *type_str = packet_type_to_str(type, &type_num);
  
  // ✅ Tracker les adresses MAC si activé
  if (TRACK_UNIQUE_MACS) {
    track_device(hdr->addr2, ppkt->rx_ctrl.rssi, ppkt->rx_ctrl.channel, type_num);
  }
  
  // ✅ Afficher chaque paquet si activé
  if (SHOW_ALL_PACKETS) {
    printf("[%6d] %s | Ch:%2d | RSSI:%3d | TX: ", 
      total_packets, type_str, ppkt->rx_ctrl.channel, ppkt->rx_ctrl.rssi);
    print_mac(hdr->addr2);
    printf(" → ");
    print_mac(hdr->addr1);
    printf("\n");
  }
}

// ========== INITIALISER LE SNIFFER ==========

void wifi_sniffer_init(void) {
  WiFi.mode(WIFI_MODE_NULL);
  
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  
  wifi_country_t wifi_country = {
    .cc = "CN",
    .schan = 1,
    .nchan = 13,
    .policy = WIFI_COUNTRY_POLICY_AUTO
  };
  esp_wifi_set_country(&wifi_country);
  
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  
  // 🔴 ACTIVER MODE PROMISCUOUS
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  
  // ✅ FILTRER LES TYPES DE PAQUETS (optionnel)
  wifi_promiscuous_filter_t filter = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_ALL  // Capturer tous les types
  };
  esp_wifi_set_promiscuous_filter(&filter);
  
  Serial.println("✓ Sniffer initialisé");
}

void wifi_sniffer_set_channel(uint8_t channel) {
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

// ========== SETUP ==========

void setup() {
  Serial.begin(115200);
  delay(1500);
  
  Serial.println("\n╔════════════════════════════════════════════════════════════╗");
  Serial.println("║     🔍 ESP32 WIFI PACKET SNIFFER - ADVANCED                ");
  Serial.println("╚════════════════════════════════════════════════════════════╝\n");
  
  pinMode(LED_GPIO_PIN, OUTPUT);
  wifi_sniffer_init();
  
  Serial.println("\n▶ Capture en cours...\n");
}

// ========== BOUCLE PRINCIPALE ==========

unsigned long last_stats = 0;
unsigned long last_channel_switch = 0;

void loop() {
  // Basculer LED pour indiquer l'activité
  digitalWrite(LED_GPIO_PIN, !digitalRead(LED_GPIO_PIN));
  
  // Changer le canal toutes les 1 secondes
  if (millis() - last_channel_switch > WIFI_CHANNEL_SWITCH_INTERVAL) {
    last_channel_switch = millis();
    current_channel = (current_channel % WIFI_CHANNEL_MAX) + 1;
    wifi_sniffer_set_channel(current_channel);
  }
  
  // Afficher les statistiques toutes les 10 secondes
  if (millis() - last_stats > 10000) {
    last_stats = millis();
    
    Serial.println("\n╔════════════════════════════════════════════════════════════╗");
    Serial.println("║  📊 STATISTIQUES EN TEMPS RÉEL");
    Serial.println("╚════════════════════════════════════════════════════════════╝");
    Serial.printf("Total paquets:     %d\n", total_packets);
    Serial.printf("  → Management:    %d\n", mgmt_packets);
    Serial.printf("  → Data:          %d\n", data_packets);
    Serial.printf("  → Miscellaneous: %d\n", misc_packets);
    Serial.printf("Canal actuel:      %d\n", current_channel);
    
    // Afficher les appareils tracés
    if (TRACK_UNIQUE_MACS) {
      print_tracked_devices();
    }
  }
  
  delay(100);
}