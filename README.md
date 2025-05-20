# Projeto EchoBeacon para Mottu 🏍️

# Integrantes
* **Gustavo Lopes Santos da Silva** - RM: 556859
* **Renato de Freitas David Campiteli** - RM: 555627
* **Gabriel Santos Jablonski** - RM: 555452

## Resumo

O projeto EchoBeacon visa implementar uma solução tecnológica para melhorar a organização e a localização das motos no pátio da empresa Mottu. O sistema será composto por uma série de componentes integrados, incluindo:

1. **EchoBeacon**: Dispositivos eletrônicos de tamanho reduzido instalados em cada moto para facilitar a identificação e o alarme.
2. **Sistema de Cadastro de Motos**: Desenvolvido com Java e NextJS para registrar informações detalhadas sobre as motos, incluindo chassi e problemas específicos.
3. **Aplicativo Móvel**: Conectado ao banco de dados centralizado para fornecer acesso aos colaboradores para consultar informações e localizar as motos.
4. **Banco de Dados Centralizado**: Armazena todas as informações relevantes sobre as motos, permitindo uma gestão eficiente.

### Funcionalidades do EchoBeacon ⚙️

- **Identificação Rápida**: Usando um sistema de som (pequeno alarme) e LED/farol.
- **Registro em Banco de Dados**: Quando uma moto chega ao pátio, suas informações são registradas para consulta posterior.

### Acesso aos Colaboradores

- **Consulta Detalhada**: Informações como placa, chassi e problema do veículo podem ser consultadas no aplicativo móvel.
- **Localização das Motos**: Funcionalidade para ativar o alarme e o LED da tag associada à moto selecionada, facilitando sua localização.

### Solução

Este sistema visa resolver o problema de localizar rapidamente as motos no pátio. Com a implementação desse sistema, a Mottu poderá organizar melhor suas motos e otimizar o tempo gasto na identificação e localização dos veículos dentro do pátio, garantindo uma gestão mais ágil e eficiente.

## Tecnologia Utilizada 💻

- **Microcontrolador ESP32**: Para o desenvolvimento do EchoBeacon.
- **Java**: Para o sistema de cadastro de motos.
- **React-Native**: Para a interface mobile.
- **Node-RED**: Para processamento de dados MQTT e visualização em um painel de controle.

### Configuração Wokwi 🕹️
- Lembrando que deve ser instalado as seguintes bibliotecas:
    - ArduinoJson
    - PubSubClient

```json
{
  "version": 1,
  "author": "Gustavo Lopes",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-esp32-devkit-v1", "id": "esp32", "top": 0, "left": 0, "attrs": {} },
    { "type": "wokwi-led", "id": "led1", "top": 0, "left": 160, "attrs": { "color": "red" } },
    { "type": "wokwi-buzzer", "id": "bz1", "top": 31.2, "left": -84.6, "attrs": {} },
    { "type": "wokwi-resistor", "id": "r1", "top": 50, "left": 100, "attrs": { "value": "220" } },
    {
      "type": "wokwi-pushbutton",
      "id": "btn1",
      "top": 200,
      "left": 100,
      "attrs": { "color": "blue" }
    }
  ],
  "connections": [
    [ "esp32:D2", "r1:1", "green", [ "v0" ] ],
    [ "r1:2", "led1:A", "green", [ "v0" ] ],
    [ "led1:C", "esp32:GND.1", "black", [ "v0" ] ],
    [ "esp32:D4", "bz1:+", "blue", [ "v0" ] ],
    [ "bz1:-", "esp32:GND.2", "black", [ "v0" ] ],
    [ "esp32:D15", "btn1:1.l", "yellow", [ "v0" ] ],
    [ "btn1:2.l", "esp32:GND.3", "black", [ "v0" ] ],
    [ "bz1:1", "esp32:GND.1", "black", [ "v0" ] ],
    [ "bz1:2", "esp32:D4", "green", [ "v0" ] ],
    [ "esp32:TX0", "$serialMonitor:RX", "", [] ],
    [ "esp32:RX0", "$serialMonitor:TX", "", [] ]
  ],
  "dependencies": {}
}
```
### Configuração do Node-RED
- Lembrando que deve conter as seguintes paletas:
    - node-red-contrib-mqtt-auto
    - node-red-dashboard
    - node-red-node-serialport
```json
[
  {
    "id": "mqtt_in_echobeacon",
    "type": "mqtt in",
    "z": "1",
    "name": "Receber MQTT - EchoBeacon",
    "topic": "fiap/iot/echobeacon",
    "qos": "0",
    "datatype": "auto",
    "broker": "mqtt_broker_fiap",
    "nl": false,
    "rap": true,
    "rh": 0,
    "x": 160,
    "y": 100,
    "wires": [["json_parser", "debug_raw"]]
  },
  {
    "id": "json_parser",
    "type": "json",
    "z": "1",
    "name": "Converter JSON",
    "property": "payload",
    "action": "",
    "pretty": false,
    "x": 360,
    "y": 100,
    "wires": [["ui_display", "data_formatter"]]
  },
  {
    "id": "debug_raw",
    "type": "debug",
    "z": "1",
    "name": "Debug Bruto",
    "active": false,
    "tosidebar": true,
    "console": false,
    "tostatus": false,
    "complete": "true",
    "targetType": "full",
    "x": 370,
    "y": 160,
    "wires": []
  },
  {
    "id": "ui_display",
    "type": "ui_template",
    "z": "1",
    "group": "ui_group_fiap",
    "name": "Exibir Dados Moto",
    "order": 1,
    "width": 0,
    "height": 0,
    "format": "<div>\n    <h4>🚨 Moto Localizada</h4>\n    <p><b>Placa:</b> {{msg.payload.placa}}</p>\n    <p><b>Chassi:</b> {{msg.payload.chassi}}</p>\n    <p><b>Problema:</b> {{msg.payload.problema}}</p>\n    <p><b>Timestamp:</b> {{msg.payload.timestamp}}</p>\n</div>",
    "storeOutMessages": true,
    "fwdInMessages": true,
    "resendOnRefresh": true,
    "templateScope": "local",
    "x": 600,
    "y": 80,
    "wires": [[]]
  },
  {
    "id": "data_formatter",
    "type": "function",
    "z": "1",
    "name": "Formatar Timestamp",
    "func": "let timestamp = msg.payload.timestamp;\nlet dataHora = new Date(timestamp).toLocaleString();\n\nmsg.payload.timestamp = dataHora;\n\nreturn msg;",
    "outputs": 1,
    "noerr": 0,
    "initialize": "",
    "finalize": "",
    "libs": [],
    "x": 600,
    "y": 140,
    "wires": [["ui_display"]]
  },
  {
    "id": "mqtt_broker_fiap",
    "type": "mqtt-broker",
    "z": "1",
    "name": "HiveMQ Broker",
    "broker": "broker.hivemq.com",
    "port": "1883",
    "clientid": "node_red_fiap",
    "autoConnect": true,
    "usetls": false,
    "compatmode": true,
    "keepalive": "60",
    "cleansession": true,
    "birthTopic": "",
    "birthQos": "0",
    "birthRetain": "false",
    "birthPayload": "",
    "closeTopic": "",
    "closeQos": "0",
    "closeRetain": "false",
    "closePayload": "",
    "willTopic": "",
    "willQos": "0",
    "willRetain": "false",
    "willPayload": ""
  },
  {
    "id": "ui_group_fiap",
    "type": "ui_group",
    "name": "EchoBeacon",
    "tab": "ui_tab_fiap",
    "order": 1,
    "disp": true,
    "width": "6",
    "collapse": false
  },
  {
    "id": "ui_tab_fiap",
    "type": "ui_tab",
    "name": "Painel FIAP",
    "icon": "dashboard",
    "order": 1,
    "disabled": false,
    "hidden": false
  }
]
```
