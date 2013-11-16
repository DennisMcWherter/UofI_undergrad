/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

/**
 * Game board class which is the foundation for the puzzles
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Board {
	private int Dimension;
	private HashSet<Piece> Pieces;
	
	/**
	 * Secondary Constructor
	 * 
	 * @param dim	Board dimensions (board is square; dim x dim)
	 * 
	 * NOTE: If dimension is negative, then value is default'ed to 0
	 */
	public Board(int dim) {
		if(dim < 0)
			this.Dimension = 0;
		else
			this.Dimension = dim;
		this.Pieces = new HashSet<Piece>();
	}
	
	/**
	 * Place a piece somewhere on the game board
	 * 
	 * @param x		Piece's X coordinate
	 * @param y		Piece's Y coordinate
	 * @param piece	The game piece object which should be placed
	 * @return	true = piece placed otherwise false
	 */
	public boolean placePiece(int x, int y, Piece piece) {
		if(x < 0 || y < 0 || x >= this.Dimension || y >= this.Dimension)
			return false;
		piece.setParentBoard(this);
		piece.setX(x);
		piece.setY(y);
		if(!this.Pieces.contains(piece))
			this.Pieces.add(piece);
		return true;
	}
	
	/**
	 * Test the current board configuration
	 * 
	 * @return	true = valid configuration otherwise false
	 */
	public boolean isValid() {
		Iterator<Piece> it = this.Pieces.iterator();
		
		if(this.Pieces.isEmpty())
			return false;
		
		// Check each piece individually
		while(it.hasNext()) {
			Piece current = it.next();
			Iterator<Piece> others = this.Pieces.iterator();
			
			while(others.hasNext()) {
				Piece enemy = others.next();
				if(enemy.equals(current))
					continue;
				if(current.canAttack(enemy.getX(), enemy.getY()))
					return false;
			}
		}
		
		return true;
	}
	
	/**
	 * Try to retrieve a piece at a particular position on the board
	 * 
	 * @param x		The X coordinate for the piece
	 * @param y		The Y coordinate for the piece
	 * @return	Some piece object if it exists at position (x,y) otherwise null
	 */
	public Piece getPieceAt(int x, int y) {
		if(this.Pieces.isEmpty() || x < 0 || y < 0 
			|| x >= this.Dimension || y >= this.Dimension)
			return null;
		
		Iterator<Piece> it = this.Pieces.iterator();
		while(it.hasNext()) {
			Piece gamePiece = it.next();
			if(gamePiece.getX() == x && gamePiece.getY() == y)
				return gamePiece;
		}
		
		return null;
	}
	
	/**
	 * Remove a game piece from the board
	 * 
	 * @param x		X Coordinate of game piece
	 * @param y		Y Coordinate of game piece
	 */
	public void removePieceAt(int x, int y) {
		Piece p = getPieceAt(x, y);
		
		if(p != null) {
			p.setParentBoard(null);
			this.Pieces.remove(p);
		}
	}
	
	/**
	 * Get all the pieces currently on the board
	 * 
	 * @return	The set of pieces currently on the board
	 */
	public Set<Piece> getPieces() {
		return this.Pieces;
	}
	
	/**
	 * Get the board dimension
	 * 
	 * @return	The dimension of the board
	 */
	public int getDimension() {
		return this.Dimension;
	}
	
	/**
	 * Reset the game board (clear pieces, etc.)
	 */
	public void reset() {
		this.Pieces.clear();
	}
	
	/**
	 * Print out status of game
	 */
	public void print() {
		if(this.Pieces.isEmpty())
			return;
		
		int currentPieceIdx = -1;
		
		for(int i = 0 ; i <= Dimension*2 ; ++i) {
			if(i%2 == 0) { // Print out board markings
				printBoardMarkings();
			} else { // Print out piece type
				printPiece(++currentPieceIdx);
			}
			System.out.print("\n");
		}
		System.out.print("\n");
	}
	
	/**
	 * Print out board markings
	 */
	private void printBoardMarkings() {
		System.out.print("+");
		for(int i = 0 ; i < Dimension ; ++i)
			System.out.print("---+");
	}
	
	/**
	 * Print out piece on game board
	 * 
	 * @param idx	Piece index to determine where to output
	 */
	private void printPiece(int idx) {
		Piece currentPiece = null;
		
		// Not necessarily in order - makes sure of proper piece
		Iterator<Piece> it = this.Pieces.iterator();
		while(it.hasNext()) {
			currentPiece = it.next();
			if(currentPiece.getY() == idx)
				break;
		}
		
		if(currentPiece == null || currentPiece.getY() != idx)
			return;
		
		System.out.print("|");
		
		for(int i = 0 ; i < Dimension ; ++i) {
			if(currentPiece.getX() == i) {
				System.out.print(" "+currentPiece.getSymbol()+" |");
			} else {
				System.out.print("   |");
			}
		} // End print loop
	}
}
