import client_conn
import threading

#stategy simulation
from strategy import *

CONNECT_CONN = 100
IP_TARGET = '127.0.0.1'
PORT_TARGET = 12345

def log(msg):
	pass


class Strategy(object):
	def __init__(self):
		self.state = 0
		self.req_msg = ''
		self.resp_msg = ''

	#state machine
	def on_timer(self):
		pass

	def on_msg_arrive(self, proto, msg):
		pass


#simulate lots of connection
def simulate_pressure():
	for i in xrange(0, CONNECT_CONN) :
		pass


def simulate_one():
	c = conn()
	ret = c.connect(IP_TARGET, PORT_TARGET)
	while 1:
		msg = c.recv()
