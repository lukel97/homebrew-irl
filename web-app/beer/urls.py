from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='beer'),
    path('refresh/', views.refresh, name='refresh'),
    path('nuke/', views.nuke, name='nuke')
]