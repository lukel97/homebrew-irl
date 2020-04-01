from django.shortcuts import render
from django.http import HttpResponse
import datetime
import dateutil.parser
from google.cloud import firestore
from django.views.decorators.csrf import csrf_protect

import json

# Handles stuff on the index page 
def index(request):
# 	BELOW IS THE EXAMPLE CODE FROM GOOGLE-CLOUD-FIRESTORE DOCUMENTATION
# 	TODO: CHANGE IT TO USE INFO FROM OUR OWN FIRESTORE THINGAMAJIG

	context = get_context()
	# print(context)

	# when you're getting stuff... get_object_or_404 maybe
	return render(request, 'beer/index.html', context) 

def refresh(request):
	print("HI")
	
	context = get_context()
	print(context)
	return HttpResponse(json.dumps(context))

def nuke(request):
	db = firestore.Client()
	doc_ref = db.collection(u'spam').document()

	doc_ref.set({
		u'message':'Cheese it, it\'s the feds!',
        u'timestamp': datetime.datetime.now()
	})
	return HttpResponse(json.dumps({"message":"Sent message to DB"}))


def get_context():
	db = firestore.Client()
	doc_ref = db.collection(u'device-config').order_by(u'timestamp', direction = firestore.Query.DESCENDING).limit(1).stream()  #.stream() #(u'beerTemp')
	last_beer_temp = 0 
	last_ambient_temp = 0 
	last_timestamp = 0 

	for doc in doc_ref:
		beer_dict = doc.to_dict()
		print(beer_dict)
		last_beer_temp = beer_dict['beerTemp'] 
		last_ambient_temp = beer_dict['ambientTemp']
		last_timestamp = beer_dict['timestamp']

	print("*****************************************")


	context = {
		"beer_temperature_string": last_beer_temp,
		"ambient_temperature_string": last_ambient_temp,
		"timestamp": f"{last_timestamp}".split(".")[0]
	}

	return context