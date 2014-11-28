#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
#import numpy as np
#import matplotlib.pyplot as plt
import time
import threading
import os
import sys
import signal

GetCurrentTime = lambda: int(round(time.time()))

PORT_NUMBER = 4006
timeWindowSeconds = 1
startTimeSeconds = GetCurrentTime()
aggregatedStats = []
timeSeries = []
bucket = []
changes = []
active = True
f = open('stats.csv','w')

def AggregateBucket():
	global bucket
	oldBucket = bucket[:]
	bucket = []
	hitCount = 0
	missCount = 0
	for stat in oldBucket:
		if stat.statType == "cacheHit":
			hitCount += 1
		if stat.statType == "cacheMiss":
			missCount += 1
	if hitCount + missCount == 0:
		return 0
	else:
		return (float(hitCount)/float(hitCount+missCount)) * 100

def PlotResults():
	line = np.arange(100)
	plt.plot(timeSeries, aggregatedStats, 'b')
	for change in changes:
		x = [change] * len(line)
		plt.plot(x, line, 'r-')
	plt.show()
	plt.savefig('cacheResults.png')

def AggregationLoop():
	global bucket
	global aggregateStats
	global timeSeries
	while(active):
		time.sleep(timeWindowSeconds)
		aggregatedStats.append(AggregateBucket())
		timeSeries.append(GetCurrentTime() - startTimeSeconds)
		print aggregatedStats[-1]
		f.write(str(GetCurrentTime() - startTimeSeconds) + "," + str(aggregatedStats[-1]) + "\n")
	print "Shutting down Aggregation Loop"
	f.close()
	sys.exit()
		

#Parts of this borrowed from online example

class StatsObject():
	def __init__(self, statType, parameters):
		self.statType = statType
		self.parameters = parameters

	def __str__(self):
		str = self.statType
		str = str + '  {'
		for key,value in self.parameters.iteritems():
			str = str + key + ':' + value + ','
		if len(self.parameters) > 0:
			str = str[:-1]
		return str + '}'

class StatsHandler(BaseHTTPRequestHandler):
	
	#Handler for the GET requests
	def do_GET(self):
		global bucket
		global changes
		if len(self.path.split('?q=')) == 1:
			self.wfile.write("Malformed query string")
			return
		queryStringTokens = self.path.split('?')[1].split('&')
		statType = queryStringTokens[0].split('q=')[1]
		statParametersTokens = queryStringTokens[1:]
		statParameters = {}
		for token in statParametersTokens:
			statParameters[token.split('=')[0]] = token.split('=')[1]
		stat = StatsObject(statType, statParameters)
		if stat.statType == 'addMember' or stat.statType == 'dropMember':
			f.write(str(GetCurrentTime() - startTimeSeconds) + "," + stat.statType + "\n")
			changes.append(GetCurrentTime() - startTimeSeconds)
		self.send_response(200)
		self.send_header('Content-type','text/html')
		self.end_headers()
		bucket.append(stat)
		# Send the html message
		# print stat
		self.wfile.write(str(stat))
		return

aggregationThread = threading.Thread(target=AggregationLoop)
aggregationThread.start()

try:
	#Create a web server and define the handler to manage the
	#incoming request
	server = HTTPServer(('', PORT_NUMBER), StatsHandler)
	print 'Started Stats Server on port ' , PORT_NUMBER
	
	#Wait forever for incoming htto requests
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the stats server'
	active = False
	server.socket.close()
        #PlotResults()
