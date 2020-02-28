# Using app.rb

- [Setup a service account key for yourself if you don't have one](https://console.cloud.google.com/apis/credentials/serviceaccountkey)
- `export GOOGLE_APPLICATION_CREDENTIALS=<key.json>`

# Generating devices

```
openssl req -x509 -newkey rsa:2048 -keyout rsa_private.pem -nodes \
    -out rsa_cert.pem -subj "/CN=unused"
```
