#!/usr/local/bin/python

import socket
import time

def print_msg(msg):
	print len(msg)
	for i in xrange(0, len(msg)):
		tmp = "   "
		for char in msg:
			tmp += "%02X " % ord(char)
	print tmp 

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('127.0.0.1', 8883))	
time.sleep(0.1)
s.send('hello')
while(1):
	chunk = s.recv(10)
	if chunk == '':
		break
	print_msg(chunk)


