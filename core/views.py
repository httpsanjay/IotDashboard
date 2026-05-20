from django.shortcuts import render
from django.http import JsonResponse
from .models import SensorData


def get_data(request):

    data = SensorData.objects.order_by('-created_at')[:30]

    data = list(reversed(data))

    result = []

    for d in data:

        result.append({

            "temperature": d.temperature or 0,

            "pressure": d.pressure or 0,

            "altitude": d.altitude or 0,

            "waterlevel": d.watertanklevel or 0,

            "sensorstatus": d.sensorstatus,

            "time": d.created_at.strftime("%H:%M:%S")

        })

    return JsonResponse(result, safe=False)


def home(request):

    return render(request, "dashboard.html")