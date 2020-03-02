![Deploy Websocket Server](https://github.com/bubba/homebrew-irl/workflows/Deploy%20Websocket%20Server/badge.svg)
![unix-epoch-gmt on GCF](https://github.com/bubba/homebrew-irl/workflows/Deploy%20unix-epoch-gmt/badge.svg)
![move-to-firestore on GCF](https://github.com/bubba/homebrew-irl/workflows/Deploy%20Google%20Cloud%20Function/badge.svg)

# Using app.rb

- [Setup a service account key for yourself if you don't have one](https://console.cloud.google.com/apis/credentials/serviceaccountkey)
- `export GOOGLE_APPLICATION_CREDENTIALS=<key.json>`

# Generating devices

```
openssl req -x509 -newkey rsa:2048 -keyout rsa_private.pem -nodes \
    -out rsa_cert.pem -subj "/CN=unused"
```
