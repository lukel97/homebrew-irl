from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='beer'),
    path('refresh/', views.refresh, name='refresh'),
    path('nuke/', views.nuke, name='nuke'),
    path('temp_chart_json', views.temp_chart_json, name='temp_chart_json'),
    path('alcohol_chart_json', views.alcohol_chart_json, name='alcohol_chart_json')
]
