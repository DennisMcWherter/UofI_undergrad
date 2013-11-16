/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzles;

import games.Backtracker;
import games.Board;
import games.Piece;

import java.util.List;

/**
 * Particular solver for the N Queens problem
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class NQueensSolver extends Backtracker {
	private static final Piece[] PIECE = {new Queen()};
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 */
	public NQueensSolver(Board b) {
		super(b, PIECE, false);
	}
	
	/**
	 * Solver for the N Queens problem
	 * 
	 * @param pieces	The list of pieces for the solver
	 * @return	Number of solutions. Returns -1 on error
	 */
	@Override
	public int solve(List<Piece> pieces) {
		return backtrack(pieces);
	}
}
