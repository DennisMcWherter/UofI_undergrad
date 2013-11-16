/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.AI;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;

import games.Piece;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;

/**
 * Basic AI package
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class AI {
	private FeudalBoard board;
	private GameManager manager;
	
	/**
	 * Constructor
	 * 
	 * @param board		The board (graph) to use
	 */
	public AI(FeudalBoard board, GameManager manager) {
		this.board   = board;
		this.manager = manager;
	}
	
	/**
	 * The magic function to tell pieces where to go
	 */
	public void makeMove() {
		assert(this.board != null && this.manager != null);
		
		List<Piece> players = this.manager.getPieceList();
		List<int[]> attackList = generateAttackList();
		
		for(Piece p : players) {
			Collections.shuffle(attackList);
			int coords[] = attackList.get(0);
			moveClosest(p, coords);
		}
	}
	
	/**
	 * Make the list of attackable coordinates
	 * 
	 * @return	A list of integer arrays or null if error. The
	 * 			form of the array is [0] = x coordinate [1] = y coordinate
	 */
	public List<int[]> generateAttackList() {
		Set<Piece> pieces = this.board.getPieces();
		List<Piece> players = this.manager.getPieceList();
		List<int[]> attackList = new ArrayList<int[]>();
		
		// Opponent castle
		//int opponentId = (this.manager.getTurn() + 1) % this.manager.getArmyCount();
		//this.board.getCastle(opponentId);
		
		for(Piece p : pieces) {
			if(!players.contains(p)) {
				int coords[] = {p.getX(), p.getY()};
				attackList.add(coords);
			}
		}
		
		return attackList;
	}
	
	/**
	 * Simple algorithm to move closer to castle (it moves the piece)
	 * 
	 * @param piece		Piece to move
	 * @param dest  	The destination to try to attack
	 */
	private void moveClosest(Piece piece, int dest[]) {
		assert(this.board != null);
		if(piece == null || dest == null)
			return;
		
		int x = piece.getX();
		int y = piece.getY();
		int bestX = x;
		int bestY = y;
		
		final int sqDestX = dest[0]*dest[0];
		final int sqDestY = dest[1]*dest[1];
		
		double bestDist = getDistance(sqDestX, sqDestY, x, y);
		
		// Max movement for a piece is twelve places
		for(int i = 0 ; i < 12 ; ++i)
			for(int j = 0 ; j < 12 ; ++j)
				if(piece.canAttack(x+i, y+j)) {
					int thisDist = getDistance(sqDestX, sqDestY, x+i, y+i);
					if(thisDist < bestDist) {
						bestDist = thisDist;
						bestX = x+i;
						bestY = y+j;
					}
				} else if(piece.canAttack(x-i, y-j)) {
					int thisDist = getDistance(sqDestX, sqDestY, x-i, y-i);
					if(thisDist < bestDist) {
						bestDist = thisDist;
						bestX = x-i;
						bestY = y-j;
					}
				} else if(piece.canAttack(x+i, y-i)) {
					int thisDist = getDistance(sqDestX, sqDestY, x+i, y-i);
					if(thisDist < bestDist) {
						bestDist = thisDist;
						bestX = x+i;
						bestY = y-j;
					}
				} else if(piece.canAttack(x-i, y+i)) {
					int thisDist = getDistance(sqDestX, sqDestY, x-i, y+i);
					if(thisDist < bestDist) {
						bestDist = thisDist;
						bestX = x-i;
						bestY = y+j;
					}
				}
		
		this.board.placePiece(bestX, bestY, piece);
	}
	
	/**
	 * Get distance between a possible position
	 * 
	 * @param sqX		The square X coordinate of destination
	 * @param sqY		The square Y coordinate of destination
	 * @param newX		The new X value of the piece
	 * @param newY		The new Y value of the piece
	 * @return	The squared distance between the destination and source
	 */
	private static int getDistance(int sqX, int sqY, int newX, int newY) {
		int x = sqX + (newX*newX);
		int y = sqY + (newY*newY);
		return x+y;
	}
}
