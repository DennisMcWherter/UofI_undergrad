'''
visualize.py

Visualize a chord network

@author Dennis J. McWherter, Jr.
'''

from pyglet import *
from pyglet.gl import *
import math

class Visualize(window.Window):
	''' Class to visualize the network '''
	DEG2RAD = math.pi/180.0
	width   = 500
	height  = 500

	'''
	Constructor

	@param peers	List of peers in the network
	'''
	def __init__(self, peers):
		# Add some extra buffering so anti-aliasing works
		cfg = pyglet.gl.Config(double_buffer=True, sample_buffers=True, samples=4)

		super(Visualize, self).__init__(config=cfg, width=self.width, height=self.height)

		screen = self.get_size()
		width  = screen[0]
		height = screen[1]
		rVal   = float(min(width, height))
		rad    = (rVal / 2.0) - (rVal * 0.05) # Leave a little space to be visually nice
		self.rad = rad

		# Make sure the window is in proper perspective
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluOrtho2D(0, width, 0, height)
		glMatrixMode(GL_MODELVIEW)

		# Clear the view
		glClearColor(1, 1, 1, 1)
		self.clear()

		pyglet.clock.schedule(self.update)

		# Draw chord unit circle
		self.drawCircle(rad, (width / 2, height / 2),
			(0.0, 0.0, 0.0, 0.75), False)

		self.peers = peers
		self.drawPeers()

	'''
	Draw the peers on the network
	'''
	def drawPeers(self):
		for peer in self.peers:
			# TODO: Should I think about realId or will that go unused?
			self.drawNode(peer.getId(), peer.good)

	'''
	Method to start the viewer
	'''
	def run(self):
		app.run()

	'''
	Handle key events (callback)

	@param symbol		Key pressed
	@param modifiers	Modifying keys
	'''
	def on_key_press(self, symbol, modifiers):
		if symbol == window.key.S:
			filename = raw_input("Save image as (*.png): ")
			self.save_image(filename)
		elif symbol == window.key.ESCAPE:
			app.exit()

	'''
	Draw a node with a given id on the circle

	@param ident 		Id of the node
	@param good 		Whether the node is good or bad
	'''
	def drawNode(self, ident, good):
		color = (0, 25, 50, 50) if good else (250, 0, 0, 50)
		n = float(len(self.peers))
		# Decrease node size at 1 unit per 100 nodes
		nodeSize = 7.0 - (n / 50.0)
		nodeSize = max(2.0, nodeSize) # Size is no less than 2.0
		# Simple to calculate since L = r*theta
		# Thus, since L = ident, 
		# theta = L / r : use this to calculate center
		# Put ident on the circle and translate it by pi/2
		theta = (math.pi / 2) - (2.0 * math.pi * ident)
		pos = ((self.width / 2) + (math.cos(theta) * self.rad),
			  (self.height / 2) + (math.sin(theta) * self.rad))
		self.drawCircle(nodeSize, pos, color)

	'''
	Circle drawing primitive method

	@param radius		Radius of the circle
	@param center		Position of the center
	@param color 		Color of the circle
	@param filled 		Whether or not the circle is filled
	'''
	def drawCircle(self, radius, center, color=(0, 50, 50, 50), filled=True):
		(x, y) = center
   		glPushMatrix()
   		glColor4f(0,0,0,1)
   		glEnable(GL_LINE_SMOOTH)
   		glEnable(GL_BLEND)
   		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
   		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)

   		# Determine what kind of circle to draw
   		if filled:
   			# Draw two circles so we have a border
   			glBegin(GL_TRIANGLE_FAN)
   			self.__drawCircle__(x, y, radius)
   			glEnd()
   			rad = radius - (.12 * radius)
   			glColor4f(*color)
   			glBegin(GL_TRIANGLE_FAN)
   			self.__drawCircle__(x, y, rad)
   			glEnd()
   		else: # Otherwise, only one open circle
   			glColor4f(*color)
   			glLineWidth(0.5)
   			glBegin(GL_LINE_LOOP)
   			self.__drawCircle__(x, y, radius)
   			glEnd()
   		glDisable(GL_BLEND)
   		glDisable(GL_LINE_SMOOTH)
   		glPopMatrix()

   	'''
   	Update method
   	'''
   	def update(self, dt):
   		pass

   	'''
   	Save the image as a .png

   	@param img 		The filename to save the image as
   	'''
	def save_image(self, img):
		image.get_buffer_manager().get_color_buffer().save(img)

	''' Private '''
	'''
	Loop to actually draw the circle

	@param x 		x coordinate of center
	@param y 		y coordinate of center
	@param radius 	Radius of the circle
	'''
	def __drawCircle__(self, x, y, radius):
		for deg in range(0, 360):
   			theta = deg * self.DEG2RAD
   			xc = x + (math.cos(theta)*radius)
   			yc = y + (math.sin(theta)*radius)
   			glVertex2f(xc, yc)
