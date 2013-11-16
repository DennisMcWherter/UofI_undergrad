package puzzle;
/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */

/**
 * Base class for all pieces in the puzzles
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class Piece {
	private Board GameBoard;
	private int XPos;
	private int YPos;
	
	/**
	 * Determines whether a piece can attack a given location
	 * 
	 * @param x		X coordinate to attack
	 * @param y		Y coordinate to attack
	 * @return	true if cannot attack otherwise false
	 */
	public abstract boolean canAttack(int x, int y);
	
	/**
	 * Constructor
	 */
	public Piece() {
		this.GameBoard = null;
		this.XPos = -1;
		this.YPos = -1;
	}
	
	/**
	 * Gets the current X position of the game piece
	 * 
	 * @return	X position on the game board
	 */
	public int getX() {
		return this.XPos;
	}
	
	/**
	 * Gets the current Y position of the game piece
	 * 
	 * @return	Y position on the game board
	 */
	public int getY() {
		return this.YPos;
	}
	
	/**
	 * Set the game piece to a new X position
	 * 
	 * @param x		The X coordinate to set the game piece to
	 * @return	The current X position of the game piece after the change
	 */
	public int setX(int x) {
		if(this.GameBoard == null)
			return -1;
		if(x < 0)
			x = 0;
		else if(x >= this.GameBoard.getDimension())
			x = this.GameBoard.getDimension() - 1;
		return this.XPos = x;
	}
	
	/**
	 * Set the game piece to a new Y position
	 * 
	 * @param y		The Y coordinate to set the game piece to
	 * @return	The current Y position of the game piece after the change
	 */
	public int setY(int y) {
		if(this.GameBoard == null)
			return -1;
		if(y < 0)
			y = 0;
		else if(y >= this.GameBoard.getDimension())
			y = this.GameBoard.getDimension() - 1;
		return this.YPos = y;
	}
	
	/**
	 * Get the board which this game piece is on
	 * 
	 * @return	The board of the game piece
	 */
	public Board getParentBoard() {
		return this.GameBoard;
	}
	
	/**
	 * Set the new board on which the game piece is set
	 * 
	 * @param board		The new game board on which to set the piece
	 */
	public void setParentBoard(Board board) {
		if(board == null || board == this.GameBoard)
			return;
		this.GameBoard = board;
	}

}
