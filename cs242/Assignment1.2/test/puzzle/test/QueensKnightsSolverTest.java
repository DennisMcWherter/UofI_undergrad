/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;

import java.util.ArrayList;

import org.junit.Before;
import org.junit.Test;

import puzzle.Board;
import puzzle.Piece;
import puzzle.QueensKnightsSolver;

/**
 * Test class for QueensKnightsSolver
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class QueensKnightsSolverTest {
	QueensKnightsSolver solver;
	Board board;

	/**
	 * Initialize tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new Board(0);
		solver = new QueensKnightsSolver(board);
	}

	/**
	 * Test QueensKnightsSolver
	 */
	@Test
	public void testSolve() {
		assertEquals(-1, solver.solve(null));
		
		solver = new QueensKnightsSolver(new Board(8));
		assertEquals(4, solver.solve(new ArrayList<Piece>()));
		
		solver = new QueensKnightsSolver(new Board(0));
		assertEquals(0, solver.solve(new ArrayList<Piece>()));
	}

}
