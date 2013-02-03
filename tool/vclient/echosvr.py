#!/usr/bin/python
import sys
import socket
import time
import errno

class nethost(object):
	def __init__(self, port):
		self.clients = []
		self.sock = 0
		self.port = port	
		self.stat = 0
		self.errd = (errno.EAGAIN, errno.EINPROGRESS, errno.EALREADY, errno.EWOULDBLOCK)

	def start(self):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		try:
			self.sock.bind(('127.0.0.1', self.port))
		except socket.error, (code, strerr):
			print code, strerr	
			self.sock.close()
			self.sock = 0
			return -1

		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.sock.listen(1000)
		self.sock.setblocking(0)
		self.stat = 1
		return 0

	def shutdown(self):
		if self.sock:
			try: self.sock.close()
			except: pass
		self.sock = 0
		self.stat = 0
		for c in self.clients:
			if not c: continue
			try: c.close()
			except: pass

		self.clients = []

	def on_msg_arrive(self, pos, msg):
		c = self.clients[pos]
		if not c: return
		print msg
		try:
			c.send(msg)
		except:
			pass

	def process(self):
		sock = None
		try:
			sock, remote = self.sock.accept()	
			sock.setblocking(0)
		except:
			pass
		if sock:
			pos = -1
			for i in xrange(len(self.clients)):
				pos = i
				break
			if pos < 0:
				pos = len(self.clients)
				self.clients.append(None)
			self.clients[pos] = sock
			print 'accept: %d' % pos
		
		#check each clients
		for pos in xrange(len(self.clients)):
			c = self.clients[pos]
			if not c: continue	
			msg = ''
			try:
				msg = c.recv(1024)
				print msg
				if not msg:
					c.close()
					self.clients[pos] = None
					continue	
			except socket.error, (code, strerr):
				if code in self.errd:
					continue
				print strerr
				c.close()
				self.clients[pos] = None
				continue
			if msg == '': continue
			print 'recv:',msg
			self.on_msg_arrive(pos, msg)

	def loop(self):
		while (1):
			self.process()
			time.sleep(0.1)


if __name__ == '__main__':
	port = 8883
	host = nethost(port)
	ret = host.start()
	if ret < 0: 
		print 'cannot listen'
		exit(-1)
	host.loop()
	

