#define BLYNK_TEMPLATE_ID "TMPL5_Euk2iiv"
#define BLYNK_TEMPLATE_NAME "frigo"
#define BLYNK_AUTH_TOKEN "BuCpxnkWLHhz_TO2JgT2jDItUc8WfHJz"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20 2    // Connecté au GPIO2
#define LED_PIN 5    // LED connectée au GPIO5 (D1)
#define BUZZER_PIN 12 // Buzzer connecté au GPIO12 (D6)

OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-D560";
char pass[] = "WdricyHi3fsH3s5wTQ";

BlynkTimer timer;
unsigned long tempAboveStart[3] = {0, 0, 0}; // Chrono pour dépassement de seuil
bool isAlertSent[3] = {false, false, false}; // État de l'alerte
bool isBuzzerOn = false; // État du buzzer (évite de le désactiver trop tôt)
bool isWiFiDisconnected = false;
unsigned long lastSendTime = 0; // Chrono pour envoi à Blynk

const int alertThresholds[3] = {-15, 8, 10}; // Seuils de température
const int virtualPins[3] = {V0, V1, V2}; // Pins virtuels Blynk

// Adresses uniques des capteurs
DeviceAddress capteur1 = {0x28, 0x61, 0x64, 0x09, 0x51, 0xBF, 0x6F, 0x99};
DeviceAddress capteur2 = {0x28, 0x61, 0x64, 0x09, 0x53, 0xD5, 0xBF, 0x3C};
DeviceAddress capteur3 = {0x28, 0x61, 0x64, 0x09, 0x51, 0x94, 0xC6, 0x48};

void checkTemperature()
{
  sensors.requestTemperatures();
  int temps[3] = {
    sensors.getTempC(capteur1),
    sensors.getTempC(capteur2),
    sensors.getTempC(capteur3)
  };

  bool sensorError = false;
  bool tempExceeded = false;

  for (int i = 0; i < 3; i++) {
    Serial.print("Température Capteur ");
    Serial.print(i + 1);
    Serial.print(" : ");
    Serial.println(temps[i]);

    if (temps[i] == -127) {
      sensorError = true; // Un capteur est déconnecté
    }

    if (temps[i] > alertThresholds[i]) {
      if (tempAboveStart[i] == 0) {
        tempAboveStart[i] = millis(); // Démarrer le chrono si dépassement
      }
      
      if (!isAlertSent[i] && (millis() - tempAboveStart[i] >= 3600000)) { // 1 heure dépassée
        if (i == 0) {
          Blynk.logEvent("temp_alert_1", "Température capteur 1 au-dessus du seuil !");
        } else if (i == 1) {
          Blynk.logEvent("temp_alert_2", "Température capteur 2 au-dessus du seuil !");
        } else if (i == 2) {
          Blynk.logEvent("temp_alert_3", "Température capteur 3 au-dessus du seuil !");
        }
        isAlertSent[i] = true;
        tempExceeded = true;
        isBuzzerOn = true; // Activer le buzzer en continu
      }
    } else {
      tempAboveStart[i] = 0;
      isAlertSent[i] = false;
    }
  }

  // Envoi à Blynk toutes les 5 minutes
  if (millis() - lastSendTime >= 300000) { // 5 minutes = 300 000 ms
    for (int i = 0; i < 3; i++) {
      Blynk.virtualWrite(virtualPins[i], temps[i]);
    }
    lastSendTime = millis();
  }

  // Gestion du buzzer selon l'état
  if (isWiFiDisconnected) {
    // Problème Wi-Fi : bip toutes les secondes
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  } else if (sensorError) {
    // Sonde déconnectée : bip toutes les secondes
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  } else if (isBuzzerOn) {
    // Dépassement de température après 1h : bip en continu jusqu'à ce que la température redescende
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void checkWiFi()
{
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH);
    isWiFiDisconnected = false;
  } else {
    digitalWrite(LED_PIN, LOW);
    isWiFiDisconnected = true;
  }
}

void setup()
{
  Serial.begin(9600);
  sensors.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, ssid, pass);
  
  timer.setInterval(1000L, checkTemperature); // Vérification chaque seconde
  timer.setInterval(5000L, checkWiFi); // Vérification WiFi toutes les 5 secondes
}

void loop()
{
  Blynk.run();
  timer.run();
}