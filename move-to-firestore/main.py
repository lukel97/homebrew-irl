import base64
from google.cloud import firestore

db = firestore.Client()

def move_to_firestore(event, context):
    """Triggered from a message on a Cloud Pub/Sub topic.
    Args:
         event (dict): Event payload.
         context (google.cloud.functions.Context): Metadata for the event.
    """
    pubsub_message = base64.b64decode(event['data']).decode('utf-8')

    doc_ref = db.collection(u'device-config').document()
    doc_ref.set({
        u'message': pubsub_message
        })

    print('Persisting a message')
    print(pubsub_message)
