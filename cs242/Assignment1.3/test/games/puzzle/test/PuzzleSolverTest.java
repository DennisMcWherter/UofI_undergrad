/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzle.test;

import static org.junit.Assert.*;
import games.Board;
import games.puzzles.NQueensSolver;

import org.junit.Test;



/**
 * Test the non-abstract methods in the puzzle solver class
 * @author Dennis J. McWherter, Jr.
 *
 */
public class PuzzleSolverTest {

	/**
	 * Test whether or not the retrieved game board is correct
	 */
	@Test
	public void testGetGameBoard() {
		Board b = new Board(5);
		NQueensSolver q = new NQueensSolver(b);
		
		assertSame(b, q.getGameBoard());
	}

}
