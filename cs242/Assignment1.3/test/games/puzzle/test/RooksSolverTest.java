/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzle.test;

import static org.junit.Assert.*;

import games.Board;
import games.Piece;
import games.puzzles.RooksSolver;

import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;



/**
 * Class to test the Rooks solver
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class RooksSolverTest {
	RooksSolver solver;
	Board board;

	/**
	 * Initialize tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board  = new Board(8);
		solver = new RooksSolver(board);
	}

	/**
	 * Test for valid solutions
	 */
	@Test
	public void testSolve() {
		List<Piece> solns = new ArrayList<Piece>();
		assertEquals(40320, solver.solve(solns));
		assertEquals(40320*8, solns.size());
	}

}
