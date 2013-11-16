/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import games.Piece;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;
import games.feudal.AI.AI;
import games.feudal.networking.Client;
import games.feudal.networking.GameActions;
import games.feudal.networking.Server;
import gui.ActionTypes;
import gui.Model;

/**
 * Basic feudal model
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalModel extends Model {
	private MainFeudalView view;
	private FeudalBoard board;
	private GameManager manager;
	private AI aiManager;
	private Piece beingMoved;
	private int[] lastMove;
	private Client client;
	private int player;
	private boolean setup;
	private int[] castlePos;
	
	/**
	 * Construct
	 * 
	 * @param view	The view on which this model operates on
	 */
	public FeudalModel(MainFeudalView view) {
		this.view       = view;
		this.board      = null;
		this.manager    = null;
		this.aiManager  = null;
		this.beingMoved = null;
		this.lastMove   = new int[2];
		this.client     = null;
		this.player     = -1;
		this.setup      = false;
		
		this.lastMove[0] = this.lastMove[1] = -1;
	}

	/**
	 * Return the image of the piece if it knows how to handle the piece
	 * 
	 * @param piece		The piece to find the image for
	 * @return	The buffered image of the piece if one exists. Otherwise null
	 */
	@Override
	public BufferedImage getPieceImg(Piece piece) {
		if(piece == null)
			return null;
		String symbol = piece.getSymbol();
		
		if("Q".equalsIgnoreCase(symbol))
			return MainFeudalView.SQUIRE_IMG;
		else if("S".equalsIgnoreCase(symbol))
			return MainFeudalView.SERG_IMG;
		else if("P".equalsIgnoreCase(symbol))
			return MainFeudalView.PRINCE_IMG;
		else if("I".equalsIgnoreCase(symbol))
			return MainFeudalView.PIKE_IMG;
		else if("N".equalsIgnoreCase(symbol))
			return MainFeudalView.KNIGHT_IMG;
		else if("K".equalsIgnoreCase(symbol))
			return MainFeudalView.KING_IMG;
		else if("D".equalsIgnoreCase(symbol))
			return MainFeudalView.DUKE_IMG;
		else if("A".equalsIgnoreCase(symbol))
			return MainFeudalView.ARCHER_IMG;
		
		return null;
	}
	
	/**
	 * Place piece
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 * @return	True if piece placed, else false
	 */
	public boolean placePiece(int x, int y) {
		assert(this.board != null);
		
		if(this.beingMoved == null)
			return false;
		
		if(!this.beingMoved.canAttack(x, y) || this.beingMoved.isMoved() ||
				!canMoveTo(x, y))
			return false;
		
		boolean retval = this.board.placePiece(x, y, this.beingMoved);
		this.beingMoved.setMoved(true);
		
		this.beingMoved = null;
		
		this.view.drawMap();
		
		this.lastMove[0] = x;
		this.lastMove[1] = y;
		
		setupNextMove((this.client == null));
		
		return retval;
	}
	
	/**
	 * Let the world know we're moving a piece
	 * 
	 * @param p		The piece we are moving
	 */
	public void movePiece(Piece p) {
		this.beingMoved = p;
	}
	
	/**
	 * Get a piece at coordinates
	 * 
	 * @param x		X Position to find piece
	 * @param y		Y position to find piece
	 * @return	The piece at the position or null otherwise
	 */
	public Piece getPieceAt(int x, int y) {
		assert(this.board != null);
		return this.board.getPieceAt(x, y);
	}
	
	/**
	 * Check if current position is occupied by friendly piece
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 * @return	True if the spot is empty, false otherwise
	 */
	private boolean canMoveTo(int x, int y) {
		List<Integer> indices = getTurnPieceIdxes();
		int currIdx = Model.convertPositionToIdx(x, y, FeudalBoard.BOARD_SIZE);
		return !indices.contains(currIdx);
	}
	
	/**
	 * Place castles. This function will select the orientation
	 * 
	 * @param army	Which player to set a castle for
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 */
	public void setCastle(int army, int x, int y) {
		if(x < 0)
			x = 0;
		else if(x >= (FeudalBoard.BOARD_SIZE-2))
			x = FeudalBoard.BOARD_SIZE-3;
		if(y < 0 && army == 0)
			y = 0;
		else if(y >= FeudalBoard.BOARD_SIZE/2)
			y = (FeudalBoard.BOARD_SIZE/2)-1;
		if(y < FeudalBoard.BOARD_SIZE/2 && army != 0)
			y = FeudalBoard.BOARD_SIZE/2;
		else if(y > FeudalBoard.BOARD_SIZE && army != 0)
			y = FeudalBoard.BOARD_SIZE-1;
		this.board.setCastle(x, y, army, army);
	}
	
	/**
	 * Get castle array in index form
	 */
	public int[][] getCastles() {
		int[][] orig = this.board.getCastles();
		int[][] castles = new int[orig.length][3];
		
		for(int i = 0 ; i < castles.length ; ++i) {
			castles[i] = new int[3];
			castles[i][0] = Model.convertPositionToIdx(orig[i][0], orig[i][1],
					FeudalBoard.BOARD_SIZE);
			castles[i][1] = Model.convertPositionToIdx(orig[i][2], orig[i][1],
					FeudalBoard.BOARD_SIZE);
			castles[i][2] = Model.convertPositionToIdx(orig[i][3], orig[i][1],
					FeudalBoard.BOARD_SIZE);
		}
		
		return castles;
	}
	
	/**
	 * Place a piece appropriately
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 * @return	True if done, false otherwise
	 */
	public boolean initialPlace(int x, int y) {
		if(this.board.checkMountain(x, y) || this.board.checkRough(x, y))
			return false;
		
		if(!canMoveTo(x, y))
			return false;
		
		Piece piece = this.manager.getNextPiece();
		
		if(piece != null)
			this.board.placePiece(x, y, piece);
		
		this.view.drawMap();
		
		// NOTE: Currently only works for 2 armies
		int turn = this.manager.getTurn();
		int half = (turn == 0) ? 1 : 2;
		
		if(turn == -1 ||
			(this.client != null && this.manager.getTurn() != this.player))
			return true;
		
		this.view.initialPlace(half);
		
		return false;
	}
	
	/**
	 * Ready to start game
	 */
	public void readyToStart() {
		if(this.client == null) {
			this.manager.resetTurns();
			this.view.enableGUI();
		} else {
			completeTurn();
		}
	}
	
	/**
	 * Complete a turn
	 */
	public void completeTurn() {
		List<Piece> pieces = this.manager.completeTurn(this.getPieces());
		
		for(Piece p : pieces)
			this.board.removePiece(p);
				
		this.beingMoved = null;
		this.view.drawMap();
		
		int winner = -1;
		if(this.client == null || this.setup)
			winner = this.manager.checkGameOver();
		
		if(this.board.checkCastle(this.lastMove[0], this.lastMove[1]))
			winner = this.manager.getTurn();			
		
		if(winner == -1 && this.client == null) {
			setupNextMove(false);
		} else if(this.client == null) {
			this.view.sendNotification("Player "+(winner+1)+" wins!");
		} else {
			this.view.removeListeners();
			if(winner != -1) {
				this.sendCompletionMsg(GameActions.GAMEOVER, winner);
				return;
			} else if(!this.setup) {
				this.sendCompletionMsg(GameActions.BOARDSETUP, 0);
				this.setup = true;
				return;
			}
			this.sendCompletionMsg(GameActions.NEXTTURN, 0);
		}
	}
	
	/**
	 * Get the piece indices of whoever player's turn it is
	 * 
	 * @return	A list of indices of current player pieces
	 */
	private List<Integer> getTurnPieceIdxes() {
		List<Piece> currPieces = this.manager.getPieceList();
		List<Piece> allPieces  = this.getPieces();
		List<Integer> indices  = new ArrayList<Integer>();
		
		for(Piece p : allPieces) {
			for(Piece x : currPieces) {
				if(x == p) {
					indices.add(Model.convertPositionToIdx(x));
					break;
				}
			}
		}
		
		return indices;
	}
	
	/**
	 * Setup the next move
	 * 
	 * @param networked		Online or local game
	 */
	public void setupNextMove(boolean networked) {
		assert(this.view != null);
		
		if(networked)
			this.manager.setTurn(this.player);
		
		List<Integer> indices = getTurnPieceIdxes();
		
		if(networked)
			this.manager.setTurn(this.player);
		
		this.view.setupNextMove(indices, networked);
	}
	
	/**
	 * Make an AI move
	 */
	public void makeAIMove() {
		if(this.client != null) // Unavailable in network mode
			return;
		this.aiManager.makeMove();
		completeTurn();
	}
	
	/**
	 * Get the terrain of the map
	 * 
	 * @return	An int array where the 0th element holds all rough
	 * 			terrain indices and 1st element contains mountainous
	 * 			terrain indices 
	 */
	public List<Integer>[] getTerrain() {
		@SuppressWarnings("unchecked")
		List<Integer>[] retval = new ArrayList[2];
		retval[0] = new ArrayList<Integer>();
		retval[1] = new ArrayList<Integer>();

		for(int i = 0 ; i < FeudalBoard.BOARD_SIZE ; ++i) {
			for(int j = 0 ; j < FeudalBoard.BOARD_SIZE ; ++j) {
				int idx = Model.convertPositionToIdx(i, j, FeudalBoard.BOARD_SIZE);
				if(this.board.checkMountain(i, j))
					retval[1].add(idx);
				else if(this.board.checkRough(i, j))
					retval[0].add(idx);
			}
		}
		
		return retval;
	}
	
	/**
	 * Get a list of pieces on the board
	 * 
	 * @return	A list of current pieces on board
	 */
	public List<Piece> getPieces() {
		assert(this.board != null);
		List<Piece> pieces = new ArrayList<Piece>();
		
		for(Piece p : this.board.getPieces())
			pieces.add(p);
				
		return pieces;
	}
	
	/**
	 * Read and parse a file
	 * 
	 * @param file		The file to be read/parsed
	 * @param armies	Number of armies to play on the map
	 * @return	Returns true if file read/parsed, false otherwise 
	 */
	public boolean parseFile(File file, int armies) {
		this.manager   = new GameManager(armies);
		this.board     = new FeudalBoard(this.manager);
		this.aiManager = new AI(this.board, this.manager);
		
		assert(this.board != null && this.manager != null &&
				this.aiManager != null);
		
		try {
			DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder = factory.newDocumentBuilder();
			Document doc = docBuilder.parse(file);
			
			doc.getDocumentElement().normalize();
			
			NodeList rough    = doc.getElementsByTagName("rough");
			NodeList mountain = doc.getElementsByTagName("mountain");
			
			parseCoordinates(rough, false);
			parseCoordinates(mountain, true);
			
		} catch (ParserConfigurationException e) {
			System.out.println("Parser error encountered");
			return false;
		} catch (SAXException e) {
			System.out.println("SAX error encountered");
			return false;
		} catch (IOException e) {
			System.out.println("IO error encountered");
			return false;
		}
		
		if(this.view != null)
			this.view.drawMap();
		
		return true;
	}
	
	/**
	 * Finds the coordinates for either mountains or rough terrain
	 * 
	 * @param list		The list of parameters passed in
	 * @param mountain	If true, then looking at mountains, otherwise rough terrain
	 */
	private void parseCoordinates(NodeList list, boolean mountain) {
		if(list == null)
			return;
		
		for(int i = 0 ; i < list.getLength() ; ++i) {
			Node rootNode = list.item(i);
			int x = -1;
			int y = -1;
			
			if(rootNode.getNodeType() == Node.ELEMENT_NODE) {
				Element elemLst = (Element)rootNode;
				
				// Get x
				NodeList xCoordList = elemLst.getElementsByTagName("x");
				Element xElem = (Element)xCoordList.item(0);
				NodeList xCoord = xElem.getChildNodes();
				x = Integer.parseInt(((Node)xCoord.item(0)).getNodeValue());
				
				// Get y
				NodeList yCoordList = elemLst.getElementsByTagName("y");
				Element yElem = (Element)yCoordList.item(0);
				NodeList yCoord = yElem.getChildNodes();
				y = Integer.parseInt(((Node)yCoord.item(0)).getNodeValue());
				
				if(mountain) {
					this.board.toggleMountain(x, y);
				} else {
					this.board.toggleRoughTerrain(x, y);
				}
			}
		}
	}
	
	/**** NETWORKING ****/
	
	/**
	 * Start a server to listen for clients
	 * 
	 * @param protocol	The protocol file to use
	 * @param port		The port to listen on
	 * @param castle	The castle coords
	 */
	public void startServer(String protocol, int port, int[] castle) {
		Server server = null;
		Socket sock   = null;
		try {
			server = new Server(port);
			
			this.view.sendNotification("When you close this box, the server will be waiting"+
					" for clients\n(the application may become unresponsive until one connects)");
			
			sock = server.waitForClient();
			
			this.client = new Client(sock, protocol);
		} catch (IOException e) {
			this.view.sendNotification("Could not initialize server: \n"+e.toString());
			return;
		}
		this.player = 0;
		
		setCastle(this.player, castle[0], castle[1]);
		
		this.view.initialPlace(0);
	}
	
	/**
	 * Connect to a server
	 * 
	 * @param host		Server host
	 * @param port		Server port
	 * @param protocol	Protocol file
	 * @param castle	Castle coordinates
	 */
	public void connectToServer(String host, int port, String protocol, int[] castle) {
		try {
			this.client = new Client(host, port, protocol);
		} catch (UnknownHostException e) {
			this.view.sendNotification("Unknown host");
			return;
		} catch (IOException e) {
			this.view.sendNotification("Could not connect to host:\n"+e.toString());
			return;
		}
		this.player = 1;
		this.castlePos = castle;
		waitTurn();
	}
	
	/**
	 * Wait for next turn
	 */
	public void waitTurn() {
		assert(this.client != null);
				
		GameActions r = this.client.getResponse();
		
		switch(r) {
			case BOARDSETUP:
				setupNetworkedBoard();
				break;
			case NEXTTURN:
				nextTurn();
				break;
			case GAMEOVER:
				getWinner();
				break;
		}
	}
	
	/**
	 * Send a completion message to other client
	 * 
	 * @param action	The action to send
	 * @param winner	The game winner (only needed for GAMEOVER)
	 */
	private void sendCompletionMsg(GameActions action, int winner) {
		assert(this.client != null);
		this.client.sendMessage(action);
		
		// Just in case anything else is to be done
		switch(action) {
			case BOARDSETUP:
				sendBoardConfig();
				break;
			case NEXTTURN:
				sendCurrentPieceSetup(false);
				break;
			case GAMEOVER:
				String txt = (winner == this.player) ? "win" : "lose";
				this.view.sendNotification("You "+txt+"!");
				this.client.sendMessage(Integer.toString(winner));
				return;
		}
		waitTurn();
	}
	
	/**
	 * Get the winner on Game over
	 */
	private void getWinner() {
		String win = this.client.getMessage();
		int winner = Integer.parseInt(win.trim());
		String txt = (winner == this.player) ? "win" : "lose";
		this.view.sendNotification("You "+txt+"!");
	}
	
	/**
	 * Setup the next turn
	 */
	private void nextTurn() {
		getPieceSetup(false);
		this.view.drawMap();
		setupNextMove(true);
	}
	
	/**
	 * Setup the board
	 */
	private void setupNetworkedBoard() {
		assert(this.client != null);
		
		if(this.player == 0) {
			finishSetupNetBoard();
			return;
		}
		
		this.manager = new GameManager(2);
		this.board   = new FeudalBoard(this.manager);
		
		assert(this.manager != null && this.board != null);
		
		// Scheme:
		// 0 = Grass
		// 1 = Rough
		// 2 = Mountain
		for(int i = 0 ; i < FeudalBoard.BOARD_SIZE ; ++i) {
			for(int j = 0 ; j < FeudalBoard.BOARD_SIZE ; ++j) {
				String msg = this.client.getMessage();
				int v = Integer.parseInt(msg.trim());
				
				if(v == 1)
					this.board.toggleRoughTerrain(i, j);
				else if(v == 2)
					this.board.toggleMountain(i, j);
			}
		}
		
		// Get castle
		String c1 = this.client.getMessage();
		String c2 = this.client.getMessage();
		
		setCastle(0, Integer.parseInt(c1.trim()), Integer.parseInt(c2.trim()));
		setCastle(1, this.castlePos[0], this.castlePos[1]);
		
		this.manager.setTurn(0);
		getPieceSetup(true);
		
		this.view.drawMap();
		this.view.initialPlace(2);
	}
	
	/**
	 * Finish original setup
	 */
	private void finishSetupNetBoard() {
		// Castle information to be recv'd
		String c1 = this.client.getMessage();
		String c2 = this.client.getMessage();
		
		int x = Integer.parseInt(c1);
		int y = Integer.parseInt(c2);
		
		setCastle(1, x, y);
		
		this.manager.setTurn(1);
		getPieceSetup(true);
		
		this.view.drawMap();
		setupNextMove(true);
	}
	
	/**
	 * Send the initial board configuration
	 */
	private void sendBoardConfig() {
		assert(this.board != null && this.client != null);
		
		if(this.player == 1) {
			sendRestBoard();
			return;
		}
		
		// Terrain
		for(int i = 0 ; i < FeudalBoard.BOARD_SIZE ; ++i) {
			for(int j = 0 ; j < FeudalBoard.BOARD_SIZE ; ++j) {
				if(this.board.checkMountain(i, j))
					this.client.sendMessage("2");
				else if(this.board.checkRough(i, j))
					this.client.sendMessage("1");
				else
					this.client.sendMessage("0");
			}
		}
		
		// Castle
		int[] castle = this.board.getCastle(this.player);
		this.client.sendMessage(Integer.toString(castle[0]));
		this.client.sendMessage(Integer.toString(castle[1]));
		
		// Pieces are next
		this.manager.setTurn(0);
		sendCurrentPieceSetup(true);
	}
	
	/**
	 * Send the remaining part of the board (i.e. board placement
	 * 	from player 2) for the initial setup
	 */
	private void sendRestBoard() {
		// Castle
		int[] castle = this.board.getCastle(this.player);
		this.client.sendMessage(Integer.toString(castle[0]));
		this.client.sendMessage(Integer.toString(castle[1]));
		
		// Pieces
		this.manager.setTurn(1);
		sendCurrentPieceSetup(true);
	}
	
	/**
	 * Get the initial position of pieces
	 * 
	 * @param init	Determine whether to get all pieces or half
	 */
	private void getPieceSetup(boolean init) {
		assert(this.board != null && this.client != null);
		
		if(!init)
			this.manager.resetTurns();
		
		// MAGIC NUMBERS : Number of pieces to get
		// only works for two players 
		for(int i = 0 ; i < (init ? 13 : 26) ; ++i) {
			String xS = this.client.getMessage();
			String yS = this.client.getMessage();
			int x = Integer.parseInt(xS.trim());
			int y = Integer.parseInt(yS.trim());
			Piece p = this.manager.getNextPiece();
			//System.out.println(xS + " " + yS);
			if(x == -1 && y == -1) {
				p.setX(-1);
				p.setY(-1);
				this.board.removePiece(p);
			} else {
				this.board.placePiece(x, y, p);
			}
		}
	}
	
	/**
	 * Send the current piece configuration
	 * 
	 * @param init	If true = initial setup (i.e. only send half)
	 */
	private void sendCurrentPieceSetup(boolean init) {
		if(!init)
			this.manager.resetTurns();
		
		for(int i = 0 ; i < (init ? 13 : 26) ; ++i) {
			Piece p = this.manager.getNextPiece();
			String x = Integer.toString(p.getX());
			String y = Integer.toString(p.getY());
			this.client.sendMessage(x);
			this.client.sendMessage(y);
		}
	}
	
	// Unimplemented
	@Override
	public int handleAction(ActionTypes src) { return 0; }
}
