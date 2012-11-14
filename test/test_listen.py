#!/usr/local/bin/python

import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('127.0.0.1', 8883))
time.sleep(0.1)
s.listen(5)

while(1):
	conn, addr = s.accept()
	print 'accepted'
	chunk = conn.recv(1024)
	if chunk != '':
		print chunk
		conn.send('Im here')
		conn.send('xxxxx')


