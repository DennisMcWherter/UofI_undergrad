/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import games.Piece;

/**
 * The squire piece for the Feudal game
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Squire extends Piece {

	/**
	 * Determines whether a piece can attack a given location
	 * 
	 * @param x		X coordinate to attack
	 * @param y		Y coordinate to attack
	 * @return	true if cannot attack otherwise false
	 */
	@Override
	public boolean canAttack(int x, int y) {
		if(!validArguments(x, y))
			return false;
		
		FeudalBoard board = (FeudalBoard)this.getParentBoard();
		int diffX   = Math.abs(x - this.getX());
		int diffY   = Math.abs(y - this.getY());
		int diffSum = diffX + diffY;
		
		// Just need to determine a single valid path around the mountain
		if(diffSum == 3 && diffX < 3 && diffY < 3) {
			for(int i = this.getX() ; i <= x ; ++i) {
				for(int j = this.getY() ; j <= y ; ++j) {
					if(board.checkMountain(i, j))
						break;
					else if(i == x && j == y)
						return true;
				}
			}
		}
		
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
		return new Squire();
	}

}
