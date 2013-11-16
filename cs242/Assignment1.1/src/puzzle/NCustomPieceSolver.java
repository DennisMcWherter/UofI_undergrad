/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * N Custom Piece problem solver
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class NCustomPieceSolver extends Backtracker {
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 */
	public NCustomPieceSolver(Board b) {
		super(b, new CustomPiece(), true);
	}
	
	/**
	 * Solver for the N Custom pieces problem
	 * 
	 * @param pieces	The list of pieces for the solver
	 * @return	M, the highest number of M pieces to put on an N x N board
	 * 			Returns -1 on error
	 */
	@Override
	public int solve(List<Piece> pieces) {
		return backtrack(pieces);
	}
}
