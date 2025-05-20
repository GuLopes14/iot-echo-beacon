#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====== Configurações WiFi ======
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";

// ====== Configurações MQTT ======
const char* BROKER_MQTT = "broker.hivemq.com";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "esp32_echobeacon";
const char* TOPIC_PUBLISH = "fiap/iot/echobeacon";

// ====== Pinos ======
const int LED_PIN = 2;
const int BUZZER_PIN = 4;
const int BUTTON_PIN = 15;

// ====== Controle do botão ======
int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// ====== Controle do localizador ======
bool localizadorAtivo = false;
unsigned long tempoAtivacao = 0;
const unsigned long DURACAO_LOCALIZADOR = 5000;

// ====== Informações da moto ======
String motoPlaca = "ABC1234";
String motoChassi = "9BWZZZ377VT004251";
String motoProblema = "Problema na roda";

// ====== Objetos WiFi e MQTT ======
WiFiClient espClient;
PubSubClient MQTT(espClient);

// ====== Protótipos ======
void initWiFi();
void initMQTT();
void reconnectMQTT();
void checkWiFIAndMQTT();
void ativarLocalizador();
void desativarLocalizador();

void initWiFi() {
  Serial.print("Conectando ao Wi-Fi ");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (MQTT.connect(ID_MQTT)) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falha. Código: ");
      Serial.print(MQTT.state());
      Serial.println(" Tentando novamente em 2s");
      delay(2000);
    }
  }
}

void checkWiFIAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) initWiFi();
  if (!MQTT.connected()) reconnectMQTT();
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  initWiFi();
  initMQTT();

  Serial.println("Sistema EchoBeacon iniciado!");
  Serial.println("Aguardando comando do botão...");
}

void ativarLocalizador() {
  if (!localizadorAtivo) {
    Serial.println("Localizador ATIVADO!");
    localizadorAtivo = true;
    tempoAtivacao = millis();
    digitalWrite(LED_PIN, HIGH);
    
    StaticJsonDocument<256> doc;
    doc["placa"] = motoPlaca;
    doc["chassi"] = motoChassi;
    doc["problema"] = motoProblema;

    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);
    MQTT.publish(TOPIC_PUBLISH, jsonBuffer);

    Serial.println("Mensagem publicada no MQTT:");
    Serial.println(jsonBuffer);
  }
}

void desativarLocalizador() {
  if (localizadorAtivo) {
    Serial.println("Localizador DESATIVADO!");
    localizadorAtivo = false;
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
  }
}

void loop() {
  checkWiFIAndMQTT();
  MQTT.loop();

  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        ativarLocalizador();
      }
    }
  }

  lastButtonState = reading;

  if (localizadorAtivo) {
    static unsigned long lastBeepTime = 0;
    static bool beepState = false;

    if (millis() - lastBeepTime > 500) {
      lastBeepTime = millis();
      beepState = !beepState;
      if (beepState) {
        tone(BUZZER_PIN, 500);
      } else {
        noTone(BUZZER_PIN);
      }
    }

    if (millis() - tempoAtivacao > DURACAO_LOCALIZADOR) {
      desativarLocalizador();
    }
  }

  delay(10);
}
