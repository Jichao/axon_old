import socket
import time

def get_hex_string(msg):
	shex = ""
	i = 0
	for c in msg:
		shex += "%02X " % ord(c)
		i += 1
		if i > 20: shex += '\n'
	return shex

#synchronize socket
class conn(object):
	def __init__(self):
		self.sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.state = 0
		self.tm_last_send = 0
		self.max_rtt = 0
		self.avg_rtt = 0
		self.rtt_cnt = 0

	def connect(self, ip, port):
		try:
			ret = self.sockfd.connect_ex((ip, port))
		except:
			return
		if ret == 0: self.state = 1
	
	def send(self, msg):
		if self.state == 0: return
		self.sockfd.send(msg)
		self.tm_last_send = time.time()

	def recv(self):
		if self.state == 0: return
		try:
			ret = self.sockfd.recv(1024)
		except:
			return

		rtt = time.time() - self.tm_last_send

