#! /usr/bin/env python
#original script from IgorLynx https://github.com/IgorLynx/CSV-to-JSON

import urllib2
import csv
import posixfile
import os

#Get remote file to local copy
FILE_LINK = 'https://docs.google.com/spreadsheets/d/1r_DUqKKEPSV__GKNqlBalXOkB-xk2SsuP5ID-ReCtUs/pub?gid=0&single=true&output=csv'
response = urllib2.urlopen(FILE_LINK)
local_CSV = open('./out.csv', 'w')
local_CSV.write(response.read())
local_CSV.close()
col_count = None

def reader():
	with open('./out.csv', 'rb') as csvfile:
			reader = csv.reader(csvfile, delimiter = ',', dialect=csv.excel)
			for row in reader:
					yield[row]

def write(filename, text):
	output=posixfile.open("./{}.csv".format(filename), 'a')
	output.lock('w|')
	output.write(text)
	output.lock('u')
	output.close

with open('./out.csv', 'r') as raw:
	line = raw.readline()
	col_count = len(line.split(","))
	for i in range(2, col_count):
		#remove previous files
		if os.path.exists("./{}.csv".format(i)):
			os.remove("./{}.csv".format(i))
		write(i, "{")

list = reader()

for rows in list:
	for row in rows:
		key = row[1]
		for x in range(2, len(row)):
			write(x,"\"{}\":\"{}\",\n".format(key, row[x]))

for i in range(2, col_count):
	write(i, "}")
