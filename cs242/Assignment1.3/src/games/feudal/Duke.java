/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import games.Piece;

/**
 * The duke piece for the Feudal game
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Duke extends MountedPiece {
	
	/**
	 * Returns a string containing the piece's symbol
	 * 
	 * @return	Symbol of a piece
	 */
	@Override
	public String getSymbol() {
		return "D";
	}

	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new Duke();
	}

}
