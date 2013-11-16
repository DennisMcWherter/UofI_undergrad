'''
Peer.py

Implementation of a "peer" unit from an S-Chord
Implementation described here:
http://www.cs.unm.edu/~saia/papers/swarm.pdf

@author Dennis J. McWherter, Jr.
'''

from DKG import DKG, Request
import math, random

class Message:
	''' Message class '''
	ident = 0.0
	M     = None # Message content
	good  = True
	route = None # Message final route

	'''
	Constructor

	@param ident		Message identifier
	@param M 			Message content
	@param good			Whether the message is a good or corrupt message
	@param route 		Message route
	'''
	def __init__(self, ident, M, good, route):
		self.ident = ident
		self.M     = M
		self.good  = good
		self.route = route

''''''''''''''''''''''''

class Peer:
	''' Peer class '''
	ident      = 1.0 # Identifier and peer position on the unit circle
	realId     = 1.0 # True identifier of node (i.e. node id cannot be trusted - this is for simulation only, would not work in practice)
	n          = 1.0 # Number of peers in the network
	lnn        = 1.0 # ln(n)
	con        = 1.0 # (C * ln(n)) / n
	peerList   = None
	swarmList  = [] # List of peers in the swarm
	centerList = [] # Nodes in the center interval for this peer
	m          = 32.0 # Value all peers should know
	                # need to determine best way to configure this
	fSet       = [] # Set of nodes in forward interval
	reqs       = dict() # Valid requests
	parent     = None # Node which admitted this peer into the network
	executing  = False # Avoid infinite loop
	messages   = dict() # A dictionary with the messages
	good       = True # Whether peer is faulty or not
	dkg        = None # DKG

	'''
	Privately configurable options
	'''
	TAMPER_CHANCE = 0.75	# The chance a message will be tampered if it goes through a bad node

	'''
	Constructor

	@param q 			Some trusted peer q to attempt to join the network
	@param peerList 	List of all peers in the network
	@param msgs 		Dictionary containing all messages on the network
	@param good 		Whether this is a correct or byzantine peer
	'''
	def __init__(self, q, peerList, msgs, dkg, good=True):
		if q != None:
			self.parent = q 
			self.ident  = q.__join__()
		else:
			self.ident = self.__randomId__()
		self.realId   = self.ident
		self.peerList = peerList
		self.messages = msgs
		self.good     = good
		self.dkg      = dkg

	'''
	Update the number of nodes in the network

	@param n 	The actual number of nodes currently in the network
	@param lnn	The natural log of n. Provided simply to reduce re-computation
	@param con  ((C * lnn) / n) - provided to reduce re-computation
	'''
	def updateNumNodes(self, n, lnn, con):
		self.n   = n
		self.lnn = lnn
		self.con = con
		self.m   = n * 8.0 # Number of bits in the id's of the nodes?
		self.__calculateSwarm__() # Build our swarm for quick indexing
		self.__buildFLinks__() # Build our set for the __next__ method to use
		self.__buildCLinks__() # Build the center links

	'''
	Get the peer's id

	@return	The peer's id (x coordinate of position)
	'''
	def getId(self):
		return self.ident

	'''
	Get the distance between two peers

	@param p 	The peer to find this distance to
	@return	The distance between two peers on the unit circle
	'''
	def dist(self, p):
		x = self.ident
		y = p.getId()
		return self.__dist__(x, y)

	'''
	Check if a given peer p is in this swarm(this)

	@param p 	Another peer to check whether or not is in this swarm
	@return True if the peer p is in the swarm, false otherwise
	'''
	def swarm(self, p):
		if self == p:
			return True # Skip math, this is true
		return (p in self.swarmList)

	'''
	Get the set of all peers on the swarm containing this peer

	@return The set of all peers in this swarm
	'''
	def getSwarm(self):
		return self.swarmList

	'''
	Check if a given peer p is in the center interval

	@param peer 	Another peer to check whether or not is in this swarm
	@return True if the peer p is in the center interval, false otherwise
	'''
	def center(self, peer):
		off   = 2.0 * self.con
		q     = peer.getId()
		p     = self.ident
		left  = p - off
		right = p + off
		return self.__inClockwiseRange__(q, left, right)

	'''
	Check if a given peer p is in the forward interval

	@param peer 	Another peer to check whether or not is in this swarm
	@return True if the peer p is in the center interval, false otherwise
	'''
	def forward(self, peer):
		q  = peer.getId()
		p  = self.ident
		m  = float(self.m)
		# As per the paper, for all i
		# between 1 ... log(m) - 1
		for i in range(int(math.ceil(math.log(m))) - 1):
			offset = ((2.0 ** (i + 1)) / m)
			n      = ((p + offset) % 1)
			left   = (n - self.con)
			right  = (n + self.con)
			if self.__inClockwiseRange__(q, left, right):
				return True
		return False

	'''
	Check if a given peer p is in the backward interval

	@param peer	Another peer to check whether or not is in this swarm
	@return True if the peer p is in the center interval, false otherwise
	'''
	def backward(self, peer):
		q  = peer.getId()
		p  = self.ident
		m  = float(self.m)
		# As per the paper, for all i
		# between 1 ... log(m) - 1
		for i in range(int(math.ceil(math.log(m))) - 1):
			offset = ((2.0 ** (i + 1)) / m)
			n      = ((p - offset) % 1)
			left   = (n - self.con)
			right  = (n + self.con)
			if self.__inClockwiseRange__(q, left, right):
				return True
		return False

	'''
	The successor method as described in the paper

	@param k 	The key to look up
	@return The next node to visit for a given key
	'''
	def successor(self, k):
		# TODO: 1. How to send request for k to all peers in S(p)?

		p = self
		# 2. S <- set of all peers in S(p)
		S = p.swarmList
		# 3. x <- identifier of p
		x = p.getId()

		# 4. while (d(x, k) > (Clnn/n)) do:
		while p.__dist__(x, k) > p.con:
			p_ = p.__next__(k)

			# if p == p' then p is closest
			if p == p_:
				print "!! I'm closest !!"
				break

			# 5. x' <- next(x, k)
			x_ = p_.getId()

			# 6. All peers in S send the request for k to all peers in S(x')
			# 7. Set of all peers in S(x') that received the above request from a majority of peers in S
			S_ = p.__makeRequest__(S, k, p_)

			# 8. S <- S'
			S = S_

			#9. x <- x'
			x = x_

			# 10. end while

 	 	# Step 11 is to update links - that should already be done
 	 	# since this is simulated (i.e. shared array - arithmetically checked)

 	 	# This cheating a little, but it avoids sending backlinks
 	 	return self.__computeSwarm__(k)

 	'''
 	Naively insert an item into the network

 	@param k 		Key to identify the message
 	@param v 		Value of the message
 	@param good		Determine whether this message is good or not
 	@return 	True if message delivered successfully, False otherwise.
 	'''
 	def insertItem(self, k, v, good=True):
 		kv    = self.__getKey__(k)
 		route = []

 		peer = self
		M = Message(kv, v, good, route)

 		# Keep forwarding through swarms until we are at 
 		# the responsible node (i.e. the message may be tampered with as we go)
 		# Use a loop rather than recursion to avoid overflow on large networks
 		while not peer.__responsible__(kv):
 			route.append(peer)
 			# Use the "verify_request" method to send the request. This
 			# should implement the important DKG functionality.
 			(p, M) = self.dkg.verify_request(peer, Request(M, self))#peer.__next__(kv)
 			if p == False:
 				return False
 			elif p == True:
 				break
 			if M.good != False and self.good == False: # Bad nodes may try to tamper
 				M.good = False if (random.random() <= self.TAMPER_CHANCE) else True
 			if p == peer:
 				print "!AH - next == self"
 			if p in route:
 				print "No route to good node (routing to {0} again)".format(p.getId())
 			peer = p

 		# Send message
 		print "Trying to insert message with key: {0}".format(kv)
 		if kv not in self.messages:
 			self.messages[kv] = M
 		return True

 	'''
 	Naive retrieval of an item from the chord network

 	@param k 		The key used to lookup the item (NOTE: This is _NOT_ the same as the network hash key)
 	@return 	The message if it exists, None otherwise
 	'''
 	def retrieveItem(self, k):
 		kv = self.__getKey__(k)

 		# Keep forwarding the message until we are responsible:
 		# NOTE: The responsible method "cheats" since the simulator
 		#		knows the network (i.e. no backlinks)
 		if not self.__responsible__(kv):
 			p = self.__next__(kv)
 			return p.retrieveItem(k)

 		if kv in self.messages:
 			print "Served by: {0}".format(self.ident)
 			return self.messages[kv]
 		return None

 	'''
 	Control messages
 	'''
	def receiveMessage(self, p, M):
		# TODO: Finish implementing this
		if not self.backward(p):
			return False # Ignored message
		# TODO: Will we need to process these messages??
		return True

	def sendMessage(self, p, M):
		return p.receiveMessage(self, M)


	''' Private methods '''

	'''
	Get the corresponding network key for some key value

	@param k 		The key to hash
	@return 	The value of the key
	'''
	def __getKey__(self, k):
		kv = hash(k)
 		# Only positive keys
 		kv = kv * -1.0 if kv < 0 else kv
 		# Map these integers into the range (0, 1]
 		#kv = float(kv) / (10000000000.0)
 		while kv > 1.0:
 			kv = float(kv) / 10.0
 		return kv

 	'''
 	Determine if this user is responsible for this key

 	@param kv		The hashed key value in the network
 	@return 	True if responsible (i.e. closest node), false otherwise
 	'''
 	def __responsible__(self, kv):
 		dist = 0
 		closest = None
 		for peer in self.peerList:
 			d = self.__dist__(peer.getId(), kv)
 			if closest == None or d < dist:
 				dist = d
 				closest = peer
 		return (self == closest)

	'''
	Calculate arbitrary swarm

	@param k 	Key/id to compute swarm for 
	@return 	A list containing the peers in S(k)
	'''
	def __computeSwarm__(self, k):
		swarmList = []
		for peer in self.peerList:
			dist = self.__dist__(peer.getId(), k)
			if dist <= self.con:
				swarmList.append(peer)
		return swarmList

	'''
	Recalculate the peers in swarm
	'''
	def __calculateSwarm__(self):
		self.swarmList = []
		for peer in self.peerList:
			dist = self.dist(peer)
			if self != peer and dist <= self.con:
				self.swarmList.append(peer)

	'''
	Build the set {f(p, 0), f(p, 1) ... f(p, log(m) - 1)}
	'''
	def __buildFLinks__(self):
		self.fSet = []
		bound = int(math.ceil(math.log(float(self.m)))) - 1
		for i in range(bound):
			self.fSet.append(self.__f__(i))

	'''
	Build center links
	'''
	def __buildCLinks__(self):
		self.centerList = []
		for p in self.peerList:
			if self.center(p):
				self.centerList.append(p)

	'''
	Get the distance between two points

	@param x 	The x point
	@param y	The y point
	@return	The distance between two points on the unit circle
			as defined by http://www.cs.unm.edu/~saia/papers/swarm.pdf
	'''
	def __dist__(self, x, y):
		if y >= x:
			return ((y - x) % 1)
		return (((1.0 - x) + y) % 1)

	'''
	Check if a particular point is in the clockwise range [left, right]

	special case:
	 * left > right : Need to wrap around the circle

	@param q 		Point to check
	@param left 	Left range value
	@param right 	Right range value
	'''
	def __inClockwiseRange__(self, q, left, right):
		# Make sure these values are in range
		left  = left  % 1
		right = right % 1
		if left == 0.0:
			left = 0.1 # This is a hack.
		# Go around the circle if this is the case
		if left > right:
			# q \in [left, 1] or q \in (0, right]
			return (left <= q and 1.0 >= q) or (0.0 < q and right >= q)
		# q \in [left, right]
		return (left <= q and right >= q)

	'''
	f function as described in
	http://www.cs.unm.edu/~saia/papers/swarm.pdf

	@return The point p + ((2.0 ** i) / m)
	'''
	def __f__(self, i):
		'''
		point     = ((self.ident + ((2.0 ** i) / float(self.m))) % 1)
		dist      = self.__dist__(self.ident, point)
		closePeer = self
		for peer in self.peerList:
			if not self.forward(peer):
				continue # Ignore outside of forward interval?
			p = peer.getId()
			d = self.__dist__(p, point)
			if d < dist:
				dist      = d
				closePeer = peer
		return closePeer
		'''
		return ((self.ident + ((2.0 ** i) / float(self.m))) % 1)

	'''
	Return the closest peer in the set {f(p, 0), ... f(p, log(m) - 1)}

	@param k 	The point to find the closest peer to
	@return The closest peer to the point k in the f-set
	'''
	def __next__(self, k):
		closePeer = None
		point     = self.ident
		dist      = self.__dist__(self.ident, k)
		# Find the closest point in fSet to k
		for p in self.fSet:
			d = self.__dist__(p, k)
			if d < dist:
				dist  = d
				point = p
		# Now find the peer closest to that point
		for peer in self.peerList:
			p = peer.getId()
			d = self.__dist__(p, k)
			if closePeer == None or d < dist:
				dist      = d
				closePeer = peer
		return closePeer

	'''
	Make a request for k from S to a S(xp)

	@param S 	The original swarm S
	@param k 	The key k to request
	@param xp 	x' - the peer representative of swarm S'
	@return  Set of all peers in S' that received the request for k
			 by a majority of peers in S
	'''
	def __makeRequest__(self, S, k, xp):
		print "Making request: " + str(self.getId())
		Sp = xp.getSwarm() # S'
		for peer in S:
			for recp in Sp:
				peer.__simSendRequest__(recp, k)
		# Populate list of peers who received majority for this request
		F = []
		majority = math.ceil(float(len(self.swarmList)) * (2.0 / 3.0))
		for recp in Sp:
			if recp.__requestsReceived__(k) >= majority:
				F.append(recp)
		return F

	'''
	Helper method to know how many unique requests received (i.e. majority?)

	@param k 	Key k
	@return  The number of unique requests received
	'''
	def __requestsReceived__(self, k):
		val = len(self.reqs[k]) if k in self.reqs else 0
		print "Request received: {0} val: {1}".format(self.getId(), val)
		return val

	'''
	Simulate sending a request

	@param p 	Peer to send request to
	@param k 	Key of request to send
	'''
	def __simSendRequest__(self, p, k):
		print "Sending request from (" + str(k) + ") : " + str(self.ident) + " to : " + str(p.getId())
		if self.ident == p.getId():
			return # Don't do this?...
		p.__simRecvRequest__(self, k)

	'''
	Method to simulate receiving a request for a given key k

	@param p 	Peer sending the request
	@param k 	key being requested
	'''
	def __simRecvRequest__(self, p, k):
		if not self.backward(p):
			print "Invalid request from: " + str(p.getId())
			return # Not a valid request
		print "Recvd req and logged"
		if k not in self.reqs:
			self.reqs[k] = dict()
		self.reqs[k][p] = 1

	'''
	Generate a random id

	@return  A random id in range (0, 1]
	'''
	def __randomId__(self):
		offset = 0.0000000000000001 # Around the machine epsilon?
		peerId = random.random() + offset # random() is [0, 1), we want (0, 1]
		return peerId

	'''
	Join a peer into the simulated network

	@return  A random id which is not already in the peer list
	'''
	def __join__(self):
		# This is a trusted peer, so allow it to decide random number
		# Notice that since this is simulated, a lot of the algorithm
		# is omitted since links are update automatically.
		while True:
			peerId = self.__randomId__()
			good = True
			for peer in self.peerList:
				if peer.getId() == peerId:
					good = False
					break # Break out of the for loop
			if good:
				return peerId
		return None # Should never get here
