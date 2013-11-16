/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzle.test;

import static org.junit.Assert.*;

import games.Board;
import games.Piece;
import games.puzzles.NCustomPieceSolver;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;



/**
 * Test class for NCustomPieceSolver
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class NCustomPieceSolverTest {

	/**
	 * Test the solver method
	 */
	@Test
	public void testSolve() {
		Board board = new Board(5);
		NCustomPieceSolver solver = new NCustomPieceSolver(board);
		
		List<Piece> solns = new ArrayList<Piece>();
		
		assertEquals(5, solver.solve(solns));
		solns.clear();
		
		board  = new Board(8);
		solver = new NCustomPieceSolver(board);
		assertEquals(8, solver.solve(solns));
		assertTrue(solns.size() > 7);
		
		solver = new NCustomPieceSolver(new Board(1));
		assertEquals(1, solver.solve(new ArrayList<Piece>()));
	}

}
