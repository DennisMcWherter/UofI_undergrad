/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzles;

import games.Piece;

/**
 * Custom chess piece class
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class CustomPiece extends Piece {

	/**
	 * Check if piece can attack at a position (x,y)
	 * 
	 * @param x		X coordinate to check
	 * @param y		Y coordinate to check
	 * @return	true if piece can attack coordinate (x,y) otherwise false
	 */
	@Override
	public boolean canAttack(int x, int y) {
		if(!validArguments(x, y))
			return false;
		
		// Piece can only move exactly 2 spaces in the X direction
		// and 3 in the Y direction
		// Or anywhere in same row/same column
		int deltaX = Math.abs(x - this.getX());
		int deltaY = Math.abs(y - this.getY());
		
		if((deltaX != 2 || deltaY != 3)
				&& !(deltaY == 0 && deltaX != 0)
				&& !(deltaX == 0 && deltaY != 0))
			return false;
		
		return true;
	}
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	@Override
	public String getSymbol() {
		return "C";
	}
	
	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new CustomPiece();
	}
}
