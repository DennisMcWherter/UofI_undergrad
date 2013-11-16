/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * Particular solver for the N Queens problem
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class NQueensSolver extends Backtracker {
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 */
	public NQueensSolver(Board b) {
		super(b, new Queen(), false);
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
