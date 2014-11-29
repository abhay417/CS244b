#! /usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

f = open('stats.csv','r')
changes = []
results = []
timeseries = []
for line in f:
	tokens = line.split(',')
	if (len(tokens) == 0):
		continue
	elif (len(tokens) < 2):
		continue
	elif (len(tokens) == 2):
		ts = tokens[0]
		value = tokens[1].strip()
		if (value == 'addMember') or (value == 'dropMember'):
			changes.append(int(ts))
		else:
			timeseries.append(int(ts))
			results.append(float(value))

line = np.arange(100)
plt.plot(timeseries, results, 'b')
for change in changes:
	x = [change] * len(line)
	plt.plot(x, line, 'r-')
plt.show()
plt.savefig('cacheResults.png')
