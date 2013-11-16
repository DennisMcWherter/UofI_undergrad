/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

/**
 * Rook gameboard piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Rook extends Piece {
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
		
		// Rooks can attack vertically or horizontally _only_
		if(x == this.getX() || y == this.getY())
			return true;
		
		return false;
	}
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	public String getSymbol() {
		return "R";
	}
	
	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new Rook();
	}
}
