/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

/**
 * Knight gameboard piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Knight extends Piece {
	/**
	 * Determines whether a piece can attack a given location
	 * 
	 * @param x		X coordinate to attack
	 * @param y		Y coordinate to attack
	 * @return	true if cannot attack otherwise false
	 */
	public boolean canAttack(int x, int y) {
		if(!validArguments(x, y))
			return false;
		
		int diffX   = Math.abs(x - this.getX());
		int diffY   = Math.abs(y - this.getY());
		int diffSum = diffX + diffY;
		
		// Knights can attack in L-shaped fashion of 3 total squares
		if(diffSum != 3 || diffX > 2 || diffY > 2)
			return false;
		
		return true;
	}
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	public String getSymbol() {
		return "N";
	}
	
	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new Knight();
	}
}
