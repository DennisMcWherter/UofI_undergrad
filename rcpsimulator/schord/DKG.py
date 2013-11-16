'''
DKG.py

Distributed key generation as described here:
http://www.cypherpunks.ca/~iang/pubs/DKG.pdf

@author Dennis J. McWherter, Jr.
'''

import random

class Request:
	''' A standard request '''
	M = None # Message
	peerList = [] # List of peers this message touched

	def __init__(self, M, peer):
		self.M = M
		self.peerList.append(peer) # Originator

class DKG:
	''' Implementation stub '''
	nodes = []
	primeList = []
	t = 2 # TODO: How to set this? 3t + 2f + 1 threshold.
	i = 1 # DEBUG

	'''
	Constructor
	'''
	def __init__(self, nodes):
		self.nodes = nodes
		self.bootstrap_primes(10000) # first 10,000 primes - we have small simulations
									 # NOTE: This shouldn't be a fixed list in practice

	'''
	Verify the request

	@param req 		Request class to verify
	@oaram peer 	Peer to verify
	@return True if message received, False if bad, or the next peer to send
		to if verified and need to forward and the new message to send
		to this client if necessary
	'''
	def verify_request(self, peer, req):
		# This is a stupid verification right now.
		if not req.M.good:
			return (False, req.M)
		if not peer.backward(req.peerList[-1]): # If we were not referred by someone in the last quorum, drop this
			return (False, req.M)
		req.peerList.append(peer)
		if peer.__responsible__(req.M.ident):
			print "Is this good?"
			return (True, req.M)
		return (peer.__next__(req.M.ident), req.M)

	'''
	Prime number generator
	Implementation from:
	http://code.activestate.com/recipes/366178-a-fast-prime-number-list-generator/
	'''
	def bootstrap_primes(self, n): 
		if n == 2:
			return [2]
		elif n < 2:
			return []
		s = range(3,n+1,2)
		mroot = n ** 0.5
		half = (n + 1) / 2 - 1
		i = 0
		m = 3
		while m <= mroot:
			if s[i]:
				j = (m * m - 3) / 2
				s[j] = 0
				while j < half:
					s[j] = 0
					j += m
			i = i + 1
			m = 2 * i + 3
		self.primeList = ([2]+[x for x in s if x])

	'''
	Receive the share message
	'''
	def recv_share(self, Pd, t, s):
		if Pd != self.i:
			return # This message is not meant for me.
		# Random symmetric bivariate polynomial
		# f(x,y) = sum_{j,l=0}^{t} f_{jl}x^jy^l \in Z_p[x,y]
		# s.t. f_{jl} = f_{lj} and f_{00} = s for j,l \in [0, t]
		f = [s] # f00 = secret

		for j in range(t):
			if j == 0:
				continue
			f.append(random.choice(self.primeList))
			# C <- C_{jl} where C_{jl} = g^{f_{jl}} for j,l \in [0, t]
		return f # Debug
