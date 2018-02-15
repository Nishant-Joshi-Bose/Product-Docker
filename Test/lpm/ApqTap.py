"""
Examples for various LPM tests that are initiated from APQ tap.
"""

import pexpect

class ApqTap():
	"""
	Wrapper for a connection to APQ tap.
	"""

	__tap = None

	def __init__(self, ip_addr='localhost', con='adb'):
		"""
		Create tap connection.
		"""
		if ip_addr != "adb":
			self.__tap = pexpect.spawn("adb shell")
			self.__tap.expect("#")
			self.__tap.sendline("telnet %s 17000" % ip_addr)
		else:
			self.__tap = pexpect.spawn("telnet %s 17000" % ip_addr )
    
		self.__tap.expect("->")

	def __enter__(self):
		"""
		Entry point for instantiated using 'with Package() as package_obj:' 
		"""
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		"""
		Automatically clean up any opened tap instance when this class is
		instantiated using 'with Package() as package_obj:'
		"""
		self.close()

	def close(self):
		"""
		Clean up any opened tap instance.
    	"""
		if self.__tap != None:
			self.__tap.terminate(True)
			self.__tap = None

	def send(self, command, expect = None):
		"""
		Send a command.
		:param: command The command string to send (should not include a CR/LF)
		:param: expect Optional string to expect as a response. Check will be ignored if None.
		"""
		self.__tap.sendline(command)
		if expect != None:
			self.__tap.expect(expect)

	def read(self):
		"""
		Reads from the tap process until there is no more data.
		:return: Output from last command.
		"""
		output = ""
		while True:
			try:
				chars = self.__tap.read_nonblocking(16, 1)
				output = output + chars
			except:
				break

		return output
