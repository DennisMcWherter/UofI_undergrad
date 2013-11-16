/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
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
	private Piece beingMoved;
	private int[] lastMove;
	
	/**
	 * Construct
	 * 
	 * @param view	The view on which this model operates on
	 */
	public FeudalModel(MainFeudalView view) {
		this.view    = view;
		this.board   = null;
		this.manager = null;
		this.beingMoved = null;
		this.lastMove = new int[2];
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
		
		setupNextMove();
		
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
		
		if(turn == -1)
			return true;
		
		this.view.initialPlace(half);
		
		return false;
	}
	
	/**
	 * Ready to start game
	 */
	public void readyToStart() {
		this.manager.resetTurns();
		this.view.enableGUI();
	}
	
	/**
	 * Complete a turn
	 */
	public void completeTurn() {
		Piece p = this.manager.completeTurn(this.getPieces());
		this.board.removePiece(p);
		this.beingMoved = null;
		this.view.drawMap();
		
		int winner = this.manager.checkGameOver();
		
		if(this.board.checkCastle(this.lastMove[0], this.lastMove[1]))
			winner = 1;			
		
		if(winner == -1)
			setupNextMove();
		else
			this.view.sendNotification("Player "+(winner+1)+" wins!");
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
	 */
	public void setupNextMove() {
		assert(this.view != null);
		
		List<Integer> indices = getTurnPieceIdxes();
		
		this.view.setupNextMove(indices);
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
		this.manager = new GameManager(armies);
		this.board   = new FeudalBoard(this.manager);
		
		assert(this.board != null && this.manager != null);
		
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
	
	// Unimplemented
	@Override
	public int handleAction(ActionTypes src) { return 0; }
}
