/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * Solver for Queens and Knights problem
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class QueensKnightsSolver extends Backtracker {
	private static final Piece[] PIECE = {new Queen(), new Knight()};
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 */
	public QueensKnightsSolver(Board b) {
		super(b, PIECE, true);
	}
	
	/**
	 * Solver for the M Queens and knights problem
	 * 
	 * @param pieces	The list of pieces for the solver
	 * @return	Returns the largest number M for the problem. Returns -1
	 * 			on error
	 */
	@Override
	public int solve(List<Piece> pieces) {
		return backtrack(pieces);
	}
}
