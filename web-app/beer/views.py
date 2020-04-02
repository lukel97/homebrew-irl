from django.shortcuts import render
from django.http import HttpResponse
import datetime
import dateutil.parser
from google.cloud import firestore
from django.views.decorators.csrf import csrf_protect
from chartjs.views.lines import BaseLineChartView

import json
import datetime

db = firestore.Client()

num_points = 100
class TempChartView(BaseLineChartView):

    beer_ts = []; amb_ts = []; timestamps = []

    def __init__(self):
        self.beer_ts = []
        self.amb_ts = []
        self.timestamps = []
        doc_ref = db.collection(u'device-config') \
                    .order_by(u'timestamp', direction = firestore.Query.DESCENDING) \
                    .limit(num_points) \
                    .stream()

        for doc in doc_ref:
            beer_dict = doc.to_dict()
            self.beer_ts.insert(0, beer_dict['beerTemp'])
            self.amb_ts.insert(0, beer_dict['ambientTemp'])
            time_str = beer_dict['timestamp'].strftime('%H:%M:%S')
            self.timestamps.insert(0, time_str)

    def get_labels(self):
        """Return the labels for the x-axis."""
        return self.timestamps

    def get_providers(self):
        """Return names of datasets."""
        return ["Beer temp", "Ambient temp", "Alcohol amount"]

    def get_data(self):
        """Return the datasets to plot."""

        return [self.beer_ts, self.amb_ts]

class AlcoholChartView(BaseLineChartView):
    alcs = []; timestamps = []
    def __init__(self):
        self.alcs = []
        self.timestamps = []
        doc_ref = db.collection(u'device-config') \
                    .order_by(u'timestamp', direction = firestore.Query.DESCENDING) \
                    .limit(num_points) \
                    .stream()

        for doc in doc_ref:
            beer_dict = doc.to_dict()
            self.alcs.insert(0, beer_dict.get('gasPerc', 0))
            time_str = beer_dict['timestamp'].strftime('%H:%M:%S')
            self.timestamps.insert(0, time_str)

    def get_labels(self):
        return self.timestamps

    def get_providers(self):
        return ["Alcohol amount"]

    def get_data(self):
        return [self.alcs]

# Handles stuff on the index page 
def index(request):
#       BELOW IS THE EXAMPLE CODE FROM GOOGLE-CLOUD-FIRESTORE DOCUMENTATION
#       TODO: CHANGE IT TO USE INFO FROM OUR OWN FIRESTORE THINGAMAJIG

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
        doc_ref = db.collection(u'device-config').order_by(u'timestamp', direction = firestore.Query.DESCENDING).limit(1).stream()  #.stream() #(u'beerTemp')
        last_beer_temp = 0 
        last_ambient_temp = 0 
        last_timestamp = 0 

        for doc in doc_ref:
                beer_dict = doc.to_dict()
                print(beer_dict)
                last_beer_temp = beer_dict['beerTemp'] 
                last_ambient_temp = beer_dict['ambientTemp']
                last_alcs = beer_dict.get('gasPerc', 0)
                last_timestamp = beer_dict['timestamp']

        print("*****************************************")

        context = {
                "beer_temperature_string": last_beer_temp,
                "ambient_temperature_string": last_ambient_temp,
                "alcohol_amount_string": last_alcs,
                "timestamp": f"{last_timestamp}".split(".")[0],
        }

        return context

temp_chart_json = TempChartView.as_view()
alcohol_chart_json = AlcoholChartView.as_view()
