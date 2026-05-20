from django.db import models

class SensorData(models.Model):
    temperature = models.FloatField(null=True, blank=True)
    pressure = models.FloatField(null=True, blank=True)
    altitude = models.FloatField(null=True, blank=True)
    watertanklevel = models.FloatField(null=True, blank=True)
    sensorstatus = models.BooleanField(default=False)
    created_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"{self.temperature} °C"
