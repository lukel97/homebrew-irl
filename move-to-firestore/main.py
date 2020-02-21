import base64
import json
from google.cloud import firestore

db = firestore.Client()

def move_to_firestore(event, context):
    """Triggered from a message on a Cloud Pub/Sub topic.
    Args:
         event (dict): Event payload.
         context (google.cloud.functions.Context): Metadata for the event.
    """
    msg_data = base64.b64decode(event['data']).decode('utf-8')
    msg = json.loads(msg_data)

    doc_ref = db.collection(u'device-config').document()
    doc_ref.set({
        u'beerTemp': msg['beerTemp'],
        u'ambientTemp': msg['ambientTemp'],
        # do we need to convert this to a timestamp?
        u'timestamp': event['publishTime']
        })
