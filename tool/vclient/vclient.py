from client_conn import *
import socket
import sys
import time

#virtual client used for protocol test or benchmark
class VClient(object):
	def __init__(self):
		self.conn = CConn()  #CConn
		self.send_cnt = 0
		self.recv_cnt = 0
		self.send_time = 0
		
		self.step = 0
		self.last_time = time.time()
		self.timeout = 0

	def fd_state(self):
		return self.conn.fd_state

	def connect(self, ip, port):
		if self.conn.fd_state == FDS_NULL:
			self.conn.connect(ip, port)		
	
	#process VClient
	def process(self):
		self.conn.process()
		if self.step == 0:
			self.timeout = 100
		self.on_tick()
		msg = self.conn.recv(1024)
		if len(msg) > 0:
			self.on_msg_recv(msg)

	#param: tm -- timeout time( unit: ms)
	def set_timeout(self, tm):
		self.timeout = time.time() + tm / 1000.0

	#override by sub class
	def on_msg_recv(self, msg):
		proto_id = 0
		#all state common response
		print 'recv: %s' % msg
		if self.step == 1:
			self.conn.send('step 2')
			self.step = 2		
		elif self.step == 2:
			self.conn.send('step 1')
			self.step = 1

	#override by sub class
	def on_tick(self):
		tnow = time.time()
		if tnow < self.timeout : return
		self.timeout = 0
		if self.step == 0:
			self.step = 1
			self.conn.send('start')


if __name__=='__main__':
	c = VClient()
	c.connect('127.0.0.1',8883)
	while FDS_NULL != c.fd_state():
		c.process()	
