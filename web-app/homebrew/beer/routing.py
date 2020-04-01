from django.urls import re_path

from . import consumers

websocket_urlpatterns = [
    re_path(r'wss://hombrew-iot.appspot.com:8080', consumers.ChatConsumer),
]