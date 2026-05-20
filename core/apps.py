import os
from django.apps import AppConfig

class CoreConfig(AppConfig):
    name = 'core'

    def ready(self):
        if os.environ.get('RUN_MAIN') == 'true':
            from .mqtt_client import start_mqtt
            start_mqtt()
