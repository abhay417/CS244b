#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer

PORT_NUMBER = 4006

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
		self.send_response(200)
		self.send_header('Content-type','text/html')
		self.end_headers()
		# Send the html message
		# print stat
		self.wfile.write(str(stat))
		return

try:
	#Create a web server and define the handler to manage the
	#incoming request
	server = HTTPServer(('', PORT_NUMBER), StatsHandler)
	print 'Started Stats Server on port ' , PORT_NUMBER
	
	#Wait forever for incoming htto requests
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the stats server'
	server.socket.close()
