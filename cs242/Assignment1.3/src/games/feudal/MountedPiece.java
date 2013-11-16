/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import games.Piece;

/**
 * Base class for all mounted pieces
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class MountedPiece extends Piece {
	
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
		
		// If valid move, make sure no rough patches
		// or mountains are in the way
		if(dx == dy) {
			for(int i = this.getX() ; i <= x ; ++i)
				if(board.checkMountain(i, i) || board.checkRough(i, i))
					return false;
			return true;
		} else if(x == this.getX()) {
			for(int i = this.getY() ; i <= y ; ++i)
				if(board.checkMountain(x, i) || board.checkRough(x, i))
					return false;
			return true;
		} else if(y == this.getY()) {
			for(int i = this.getX() ; i <= x ; ++i)
				if(board.checkMountain(i, y) || board.checkRough(i, y))
					return false;
			return true;
		}
		
		return false;
	}
}
