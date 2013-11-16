'''
Chord.py

Implementation of chord unit from a S-Chord paper
This is the basis for the simulation
Implementation described here:
http://www.cs.unm.edu/~saia/papers/swarm.pdf

@author Dennis J. McWherter, Jr.
'''

from Peer import Peer, Message
from DKG import DKG
import math, random

class Chord:
	''' Chord class (implementation of S-Chord) '''
	C      = 2.0 # Some constant
	n      = 0   # Number of peers in the network
	lnn    = 0   # Natural log of the number of peers
	peers  = []  # List containing all peers in the network
	msgs   = dict() # dictionary containing all the messages in the network (to maintain one list rather than propegate information)
	cPeers = []  # List containing the correct peers
	dkg    = None # DKG

	'''
	Constructor

	@param C 	Network constant
	'''
	def __init__(self, C=2.0):
		self.C   = C
		self.n   = 0
		self.lnn = 0
		self.dkg = DKG(self.peers)

	'''
	Join a certain number of peers to the network
	This is a convenience method which sets up the
	network properly after adding so many good nodes

	@param num 			Number of nodes to add
	@param percGood		(Relative) Percentage of nodes which are non-faulty (in interval (0, 1])
	'''
	def joinPeers(self, num, percGood=1.0):
		if percGood > 1.0 or percGood <= 0.0: # If out of bounds, no faulty nodes
			percGood = 1.0
		for x in range(num):
			if x == 0: # Need at least one good peer
				self.peerJoin()
			else:
				good = random.random() < percGood	# Try to get a random assortment of faulty nodes
				self.peerJoin(good)
		self.updateNodes()

	'''
	Join a peer to the network

	NOTE: After calling this method, you must call the
		  updateNodes() method or else problems WILL occur

	@param good		Whether the peer is a good (correct) node or not
	@return The peer id of the new peer
	'''
	def peerJoin(self, good=True):
		# TODO: Don't assume all are trusted
		trusted = None
		if len(self.peers) > 0:
			trusted = random.sample(self.cPeers, 1)
			trusted = trusted[0] # No need for the array

		# Add this peer to our network through the trusted node
		peer = Peer(trusted, self.peers, self.msgs, self.dkg, good)
		self.peers.append(peer)
		if good == True:
			self.cPeers.append(peer)

		# Recalculate n and ln(n) so our swarms are correct
		self.n   = len(self.peers)
		self.lnn = math.log(self.n)

		return peer.getId()

	'''
	Get a random peer

	@return 	A random peer from the network (may or may not be a faulty peer)
	'''
	def randomPeer(self):
		peers = random.sample(self.peers, 1)
		return peers[0]

	'''
	This updates the information that the nodes have
	'''
	def updateNodes(self):
		# A little wasteful, but the simulator (like the paper)
		# assumes each peer knows a certain amount of information
		# about the network
		self.__updatePeers__()

	'''
	Find a peer

	@param ident 	Identity of peer to find
	@return The peer object with the id. None if no peer exists with 
			the specified id
	'''
	def findPeer(self, ident):
		for peer in self.peers:
			if peer.getId() == ident:
				return peer
		return None

	'''
	Get the swarm of a particular peer id

	@param x 	The peer id
	@return The swarm S(p(x))
	'''
	def swarm(self, x):
		peer  = self.findPeer(x)
		if peer == None:
			return None
		return peer.getSwarm()

	'''
	Return the swarm of a given peer

	@param p 	The peer
	@return The swarm S(p)
	'''
	def swarm(self, p):
		return p.getSwarm()

	'''
	Get the list of peers

	@return	 A the list of peers
	'''
	def getPeers(self):
		return self.peers

	'''
	Return the number of peers in the network

	@return  The number of total peers in the network
	'''
	def peerCount(self):
		return len(self.peers)

	'''
	Return the number of correct peers in the network

	@return 	Number of correct peers in the network
	'''
	def correctCount(self):
		count = 0
		for p in self.peers:
			if p.good == True:
				count = count + 1
		return count

	'''
	Get faulty node count

	@return 	Number of faulty peers in the network
	'''
	def faultyCount(self):
		return (self.peerCount() - self.correctCount())

	'''
	Debug methods
	'''

	'''
	Print the forward interval
	'''
	def printForward(self):
		for peer in self.peers:
			print "id: " + str(peer.getId()) + "\nList:"
			for f in peer.fSet:
				print f.getId()
				print "In set: {0}".format(f.backward(peer))
			print "\n\n"

	'''
	Print the backward interval
	'''
	def printBackward(self):
		for peer in self.peers:
			print "id: {0}\nList:".format(peer.getId())
			for c in self.peers:
				if peer.backward(c):
					print c.getId()
			print "\n\n"

	'''
	Print the center interval
	'''
	def printCenter(self):
		for peer in self.peers:
			print "id: {0}\nList:".format(peer.getId())
			for c in self.peers:
				if peer.center(c):
					print c.getId()
			print "\n\n"

	'''
	Check that the center interval is correctly computed.
	That is, if p \in Center(q) then q \in Center(p)

	@return True if the interval is correct for all peers, false otherwise
	'''
	def checkCenter(self):
		correct = True
		for p in self.peers:
			for q in self.peers:
				if p.center(q):
					correct = correct & q.center(p)
		return correct

	'''
	Check the the forward and backward intervals
	That is, for any peer in some forward interval, that 
	same peer has the corresponding peer in its backward interval

	@return True if this interval is correct for all peers, false otherwise
	'''
	def checkBandF(self):
		correct = True
		for p in self.peers:
			for q in self.peers:
				if p.forward(q):
					t = q.backward(p)
					if not t:
						print "Node: {0} vs {1}".format(p.getId(), q.getId())
					correct = correct & t
		return correct

	''' Private methods '''

	'''
	This method updates all information which each peer
	should have in the event of a network change (i.e. join,
	leave, etc.)
	'''
	def __updatePeers__(self):
		# Be sure our constants are up to date
		con = float(((float(self.C) * float(self.lnn)) / float(self.n)))
		for peer in self.peers:
			peer.updateNumNodes(self.n, self.lnn, con)


