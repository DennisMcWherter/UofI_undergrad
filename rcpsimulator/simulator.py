'''
simulator.py

Main simulator entry point for security project

@author Dennis J. McWherter, Jr.
'''

from schord.Chord import Chord
#from visualize import Visualize
import random

NUM_PEERS = 100		# Number of peers in the network
NUM_MSGS  = 10		# Number of messages to send in the simulation
PERC_GOOD = 0.50	# Relative percentage of correct nodes in the network

def main():
	net  = Chord()
	keys = [] # List of messages sent

	'''
	Simple simulator code
	'''
	# Start with 10 peers
	net.joinPeers(NUM_PEERS, PERC_GOOD)

	print "--- SChord Simluation Results ---"
	print "Total Peers: {0}\nCorrect Peers: {1}\nFaulty Peers: {2}\n\n".format(net.peerCount(), net.correctCount(), net.faultyCount())

	# Insert a few messages
	for x in range(NUM_MSGS):
		key  = random.random()
		val  = random.random()
		peer = net.randomPeer()

		print "Sending message ({0}): {1} from peer {2}".format(key, val, peer.getId())
		if peer.insertItem(key, val):
			print "Message Sent Successfully"
		else:
			print "Message tampering caught!"

		keys.append(key)

	print "\n\n"
	
	# Get the messages back from the network
	# and see if they've been tampered with
	for k in keys:
		peer = net.randomPeer()

		m = peer.retrieveItem(k)

		print "Retrieved message ({0}): {1} (integrity is good: {2})".format(k, m.M, m.good)

	# Visualize the network
	# NOTE: This may not be comprehensible for extremely large networks
	#viz = Visualize(net.getPeers())
	#viz.run()

	'''
	Debugging code
	'''
	'''
	print "Correct: {0}".format(net.correctCount())
	m = 0
	peers = net.getPeers()
	for p in peers:
		print "For x = " + str(p.getId())
		s = net.swarm(p)
		print "--- Swarm ---"
		if s != None and len(s) != 0:
			for p in s:
				print p.getId()
		else:
			print "None"
		#m = "someMessage{0}".format(random.random())
		#p.insertItem(m, "Hello value")
		print "\n\n"
#	for p in peers:
#		print p.retrieveItem(m).M

	#print "Forward"
	#net.printForward()
	#print "Backward"
	#net.printBackward()

	#print "For x = " + str(peers[0].getId())
	print "Successor (.01): "
	for peer in peers[0].successor(.01):
		print peer.getId()
	#print peers[0].successor(.01).getId()
	#print "For x = " + str(peers[1].getId())
#	print "Successor (.01): "
#	for peer in peers[1]:
#		print peer.getId()
	#print peers[1].successor(.01).getId()
	print "\n\n"
	'''

	print "Center: {0}\nB/F: {1}\n".format(net.checkCenter(), net.checkBandF())
	#net.printForward()

if __name__ == "__main__":
	main()
