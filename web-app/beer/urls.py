from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='beer'),
    path('refresh/', views.refresh, name='refresh'),
    path('nuke/', views.nuke, name='nuke'),
    path('line_chart_json/', views.line_chart_json, name='line_chart_json')
]