from django.urls import path
from .views import home,get_data

urlpatterns = [
    path('', home),
    path('get-data/', get_data),

]
