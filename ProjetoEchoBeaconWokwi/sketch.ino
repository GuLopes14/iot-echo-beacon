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
const char* TOPIC_SUBSCRIBE = "fiap/iot/echobeacon/comando";

// ====== Pinos ======
const int LED_PIN = 2;
const int BUZZER_PIN = 4;
const int BUTTON_PIN = 15;

// ====== Controle ======
int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool localizadorAtivo = false;

String placaAtual = "";
String modeloAtual = "";

// ====== Objetos WiFi e MQTT ======
WiFiClient espClient;
PubSubClient MQTT(espClient);

// ====== Funções ======
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
  MQTT.setCallback(mqttCallback);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (MQTT.connect(ID_MQTT)) {
      Serial.println(" conectado!");
      MQTT.subscribe(TOPIC_SUBSCRIBE);
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

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) {
    Serial.println("Erro no JSON. Tentando deserializar como string JSON interna...");

    // Tenta parsear se for string JSON dentro de string
    String mensagem((char*)payload, length);
    StaticJsonDocument<256> doc2;
    error = deserializeJson(doc2, mensagem);

    if (error) {
      Serial.println("Falha ao decodificar string JSON interna.");
      return;
    }

    doc = doc2; // Usa a estrutura parseada com sucesso
  }

  const char* comando = doc["comando"];
  placaAtual = doc["moto"]["placa"] | "N/A";
  modeloAtual = doc["moto"]["modelo"] | "N/A";

  if (String(comando) == "ativar") {
    ativarLocalizador();
  }
}

void ativarLocalizador() {
  if (!localizadorAtivo) {
    localizadorAtivo = true;
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 500);

    Serial.println("🚨 Localizador ATIVADO");
    Serial.print("Placa: ");
    Serial.println(placaAtual);
    Serial.print("Modelo: ");
    Serial.println(modeloAtual);
    Serial.println("Buzzer: Ligado");
    Serial.println("LED: Ligado");
  }
}

void desativarLocalizador() {
  if (localizadorAtivo) {
    localizadorAtivo = false;
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);

    Serial.println("✅ Localizador DESATIVADO");
    Serial.println("Buzzer: Desligado");
    Serial.println("LED: Desligado");
  }
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
}

void loop() {
  checkWiFIAndMQTT();
  MQTT.loop();

  // Lógica do botão físico para desligar
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW && localizadorAtivo) {
        desativarLocalizador();
      }
    }
  }

  lastButtonState = reading;

  if (localizadorAtivo) {
    static unsigned long previousMillis = 0;
    static bool buzzerOn = false;
    unsigned long currentMillis = millis();

    if (buzzerOn && currentMillis - previousMillis >= 300) {
      // Desliga o buzzer após 300ms
      noTone(BUZZER_PIN);
      buzzerOn = false;
      previousMillis = currentMillis;
    } else if (!buzzerOn && currentMillis - previousMillis >= 700) {
      // Liga o buzzer por 300ms a cada 1s
      tone(BUZZER_PIN, 400);  
      buzzerOn = true;
      previousMillis = currentMillis;
    }
  }

  delay(10);
}

