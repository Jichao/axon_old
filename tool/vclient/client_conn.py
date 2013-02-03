#!/usr/bin/python

import socket
import time
import select
import struct
import time
import sys
import errno

def get_hex_string(msg):
	shex = ""
	i = 0
	for c in msg:
		shex += "%02X " % ord(c)
		i += 1
		if i > 20: shex += '\n'
	return shex

#state define
FDS_NULL = 0
FDS_CONNECTING = 1
FDS_ACTIVE = 2	
ERRNO_AGAIN = (errno.EAGAIN, errno.EINPROGRESS, errno.EALREADY, errno.EWOULDBLOCK)


#nonblocking connect
class CConn(object):

	def __init__(self):
		self.sockfd = 0
		self.step = 0  #state transfer
		self.fd_state = 0
		self.tm_last_send = 0
		self.max_rtt = 0
		self.avg_rtt = 0
		self.rtt_cnt = 0

		self.wbuf = ''
		self.rbuf = ''

	def process(self):
		if self.fd_state == FDS_NULL: return
		elif self.fd_state == FDS_CONNECTING:
			self.__try_connect()
		elif self.fd_state == FDS_ACTIVE:
			self.__try_recv()
			self.__try_send()
	
	def connect(self, ip, port):
		self.sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sockfd.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
		self.sockfd.setblocking(0)
		ret = self.sockfd.connect_ex((ip, port))
		
		if ret == 0: 
			self.fd_state = FDS_ACTIVE
		else: 
			self.fd_state = FDS_CONNECTING

		self.wbuf = ''
		self.rbuf = ''
	
	def __try_connect(self):
		if self.fd_state == FDS_ACTIVE: return 1
		if self.fd_state != FDS_CONNECTING: return -1
		try:
			self.sockfd.recv(0)
		except socket.error,(code, strerr):
			print strerr
			if code in ERRNO_AGAIN: return 0
			if code in (errno.EISCONN, 10057, 10053): return 0
			sys.stderr.write('[CONNECT] connect fail')			
			return -1
	
		#connect OK
		self.rbuf = ''
		self.fd_state = FDS_ACTIVE
		print 'connected'
		return 1

	def __try_recv(self):
		if self.fd_state != FDS_ACTIVE: return -1
		rdata = ''
		while 1:
			text = ''
			try:
				text = self.sockfd.recv(1024)
				if not text:
					self.sockfd.close()
					self.fd_state = FDS_NULL
					return -1
			except socket.error,(code, strerror):
				if code not in ERRNO_AGAIN:
					self.sockfd.close()
					self.fd_state = FDS_NULL	
					print strerror
					return -1
			
			if text == '': break
			rdata = rdata + text

		self.rbuf = self.rbuf + rdata
		return len(rdata)

	def __try_send(self):
		wsize = 0
		print self.wbuf
		if self.fd_state != FDS_ACTIVE: return -1
		if len(self.wbuf) == 0: return 0
		try:
			wsize = self.sockfd.send(self.wbuf)
		except socket.error,(code, strerror):
			if code in ERRNO_AGAIN: return 0
			else:
				print '[WRITE]',strerr
				self.close()
				self.fd_state = FDS_NULL
				return -1

		self.wbuf = self.wbuf[wsize:]
		return wsize
	
	
	def close(self):
		self.fd_state = FDS_NULL
		if not self.sockfd: return 0
		try:
			self.sockfd.close()
		except:
			pass
		self.sockfd = 0
		return 0

	def send(self, data):
		self.wbuf = self.wbuf + data
		self.process()
	
	def recv(self, sz):
		self.process()
		data = ''
		if sz >= len(self.rbuf): 
			data = self.rbuf
			self.rbuf = ''
			return data
		
		data = self.rbuf[0:size]
		self.rbuf = self.rbuf[size:]
		return data
		

if __name__=='__main__':
	c = CConn()
	c.connect('127.0.0.1', 8883)
		
	while c.fd_state != FDS_NULL:
		c.send('hello')
		dat = c.recv(1000)
		print 'recv:',dat
		time.sleep(0.1)

	print 'exit'
