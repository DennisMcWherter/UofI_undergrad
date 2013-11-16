package games;
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
	private Board gameBoard;
	private int xPos;
	private int yPos;
	private boolean moved;
	
	/**
	 * Determines whether a piece can attack a given location
	 * 
	 * @param x		X coordinate to attack
	 * @param y		Y coordinate to attack
	 * @return	true if cannot attack otherwise false
	 */
	public abstract boolean canAttack(int x, int y);
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	public abstract String getSymbol();
	
	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	public abstract Piece getNewPiece();
	
	/**
	 * Constructor
	 */
	public Piece() {
		this.gameBoard = null;
		this.xPos = -1;
		this.yPos = -1;
		this.moved = false;
	}
	
	/**
	 * Checks to make sure that board is not null, coordinates are valid, etc.
	 * 
	 * @return	True if everything is well (i.e. conditions are met and valid 
	 * 			parameters), otherwise false
	 */
	protected boolean validArguments(int x, int y) {
		return !(getParentBoard() == null || x < 0 || y < 0
				|| x >= getParentBoard().getDimension()
				|| y >= getParentBoard().getDimension()
				|| (x == getX() && y == getY()));
	}
	
	/**
	 * Gets the current X position of the game piece
	 * 
	 * @return	X position on the game board
	 */
	public int getX() {
		return this.xPos;
	}
	
	/**
	 * Gets the current Y position of the game piece
	 * 
	 * @return	Y position on the game board
	 */
	public int getY() {
		return this.yPos;
	}
	
	/**
	 * Set the game piece to a new X position
	 * 
	 * @param x		The X coordinate to set the game piece to
	 * @return	The current X position of the game piece after the change
	 */
	public int setX(int x) {
		if(this.gameBoard == null)
			return -1;
		if(x < 0)
			x = 0;
		else if(x >= this.gameBoard.getDimension())
			x = this.gameBoard.getDimension() - 1;
		return this.xPos = x;
	}
	
	/**
	 * Set the game piece to a new Y position
	 * 
	 * @param y		The Y coordinate to set the game piece to
	 * @return	The current Y position of the game piece after the change
	 */
	public int setY(int y) {
		if(this.gameBoard == null)
			return -1;
		if(y < 0)
			y = 0;
		else if(y >= this.gameBoard.getDimension())
			y = this.gameBoard.getDimension() - 1;
		return this.yPos = y;
	}
	
	/**
	 * Set moved
	 * 
	 * @param value		Set whether moved or not
	 */
	public void setMoved(boolean value) {
		this.moved = value;
	}
	
	/**
	 * Get whether or not a piece has been set to moved
	 * 
	 * @return	True if moved, false otherwise
	 */
	public boolean isMoved() {
		return this.moved;
	}
	
	/**
	 * Get the board which this game piece is on
	 * 
	 * @return	The board of the game piece
	 */
	public Board getParentBoard() {
		return this.gameBoard;
	}
	
	/**
	 * Set the new board on which the game piece is set
	 * 
	 * @param board		The new game board on which to set the piece
	 */
	public void setParentBoard(Board board) {
		if(board == null || board == this.gameBoard)
			return;
		this.gameBoard = board;
	}

}
