import paho.mqtt.client as mqtt
import ssl
from .models import SensorData

BROKER = "mqtt.example.com"
PORT = 8883
USERNAME = "mqtt_user"
PASSWORD = "mqtt_pass"

TOPICS = {
    "esp32/bmp280/temperature": "temperature",
    "esp32/bmp280/pressure": "pressure",
    "esp32/bmp280/altitude": "altitude",
    "esp32/waterlevel": "watertanklevel",
    "esp32/sensorstatus": "sensorstatus",
}

client = mqtt.Client()

latest_data = {
    "temperature": None,
    "pressure": None,
    "altitude": None,
    "watertanklevel": None,
    "sensorstatus": None,
}

def on_connect(client, userdata, flags, rc):

    print("Connected:", rc)

    for topic in TOPICS:
        client.subscribe(topic)

def on_message(client, userdata, msg):

    payload = msg.payload.decode()

    try:

        if msg.topic == "esp32/sensorstatus":
            value = payload == "1"
        else:
            value = float(payload)

    except ValueError:

        print("Invalid Data")
        return

    if msg.topic in TOPICS:
        latest_data[TOPICS[msg.topic]] = value

    if all(v is not None for v in latest_data.values()):

        SensorData.objects.create(**latest_data)

        for key in latest_data:
            latest_data[key] = None

def start_mqtt():

    client.username_pw_set(USERNAME, PASSWORD)

    client.tls_set(tls_version=ssl.PROTOCOL_TLS)

    client.on_connect = on_connect

    client.on_message = on_message

    try:

        client.connect(BROKER, PORT)

        print("Connecting to MQTT...")

    except Exception as e:

        print("MQTT Error:", e)

    client.loop_start()