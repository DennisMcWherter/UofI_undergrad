/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import games.Piece;

/**
 * The archer piece for the Feudal game
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Archer extends Piece {

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
		int dx = Math.abs(x - this.getX());
		int dy = Math.abs(y - this.getY());
		
		if((dx == 0) && dy < 4) {
			for(int i = this.getY() ; i <= y ; ++i)
				if(board.checkMountain(x, i))
					return false;
			return true;
		} else if((dy == 0) && dx < 4) {
			for(int i = this.getX() ; i <= x ; ++i)
				if(board.checkMountain(i, y))
					return false;
			return true;
		} else if((dx == dy) && dx < 4) {
			for(int i = this.getX() ; i <= x ; ++i)
				if(board.checkMountain(i, i))
					return false;
			return true;
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
		return "A";
	}

	/**
	 * Returns a new object instance of the current piece
	 * 
	 * @return	New object of current piece
	 */
	@Override
	public Piece getNewPiece() {
		return new Archer();
	}

}
