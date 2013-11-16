/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.AI;

import games.Piece;
import games.feudal.FeudalBoard;

/**
 * Basic AI package
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class AI {
	private FeudalBoard board;
	
	/**
	 * Constructor
	 * 
	 * @param board		The board (graph) to use
	 */
	public AI(FeudalBoard board) {
		this.board = board;
	}
	
	/**
	 * Simple algorithm to move closer to castle
	 * 
	 * @param piece		Piece to move
	 * @param castleLoc	The location of the castle
	 */
	public void moveClosest(Piece piece, int castleLoc[]) {
		if(piece == null || castleLoc == null)
			return;
		int x = piece.getX();
		int y = piece.getY();
		
		int diffX = castleLoc[0] - x;
		int diffY = castleLoc[1] - y;
		
		boolean canMoveDiagonal   = (piece.canAttack(x-1, y-1) || piece.canAttack(x+1, y+1));
		boolean canMoveVertical   = (piece.canAttack(x, y+1) || piece.canAttack(x, y-1));
		boolean canMoveHorizontal = (piece.canAttack(x+1, y) || piece.canAttack(x-1, y));
		
		boolean leftHorizontal = (x < 0);
		boolean upVertical	   = (y < 0);
		
		// TODO: Determine the shortest valid path (within valid moves)
		// which can be made to get closer to the castle
	}
}
