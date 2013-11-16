/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * Base class for all puzzle solvers
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class PuzzleSolver {
	private Board gameBoard;
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve
	 */
	public PuzzleSolver(Board b) {
		this.gameBoard = b;
	}
	
	/**
	 * Puzzle solving method (abstract)
	 * 
	 * @param pieces	List of solution pieces
	 * @return	Number of solutions
	 */
	public abstract int solve(List<Piece> pieces);
	
	/**
	 * Get current gameboard
	 * 
	 * @return	Current gameboard
	 */
	public Board getGameBoard() {
		return this.gameBoard;
	}
}
