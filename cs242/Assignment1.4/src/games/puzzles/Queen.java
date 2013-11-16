/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzles;

import games.Piece;

/**
 * A game piece with properties of a chess queen
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Queen extends Piece {
		
	/**
	 * Check if queen can attack at a position (x,y)
	 * 
	 * @param x		X coordinate to check
	 * @param y		Y coordinate to check
	 * @return	true if piece can attack coordinate (x,y) otherwise false
	 */
	@Override
	public boolean canAttack(int x, int y) {
		if(!validArguments(x, y))
			return false;
		
		// Check row/cols (should always be positive integer subtraction)
		int rowDiff = Math.abs(Math.abs(this.getX()) - Math.abs(x));
		int colDiff = Math.abs(Math.abs(this.getY()) - Math.abs(y));
		
		/**
		 * Basic conditions for attack:
		 * 	[x] Same row
		 * 	[x] Same column
		 * 	[x] Same diagonal
		 */
		if(x == this.getX() || y == this.getY()
				|| rowDiff == colDiff)
			return true;
		
		return false;
	}
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	@Override
	public String getSymbol() {
		return "Q";
	}
	
	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new Queen();
	}
}
