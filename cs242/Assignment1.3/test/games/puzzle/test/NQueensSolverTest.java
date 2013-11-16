/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzle.test;

import static org.junit.Assert.*;
import org.junit.Test;


import games.Board;
import games.Piece;
import games.puzzles.NQueensSolver;

import java.util.ArrayList;
import java.util.List;

/**
 * Tests the NQueensSolver class
 * @author Dennis J. McWherter, Jr.
 * 
 */
public class NQueensSolverTest {

	/**
	 * Test constructor
	 */
	@Test
	public void testConstructor1() {
		NQueensSolver solver = new NQueensSolver(new Board(5));
		assertNotNull(solver);
	}
	
	/**
	 * Test method for {@link games.puzzles.NQueensSolver#solve(java.util.List)}.
	 */
	@Test
	public void testSolve() {
		Board board = new Board(8);
		NQueensSolver solver = new NQueensSolver(board);
		
		List<Piece> solns = new ArrayList<Piece>();
		
		assertEquals(92, solver.solve(solns));
		assertEquals(92*8, solns.size());
		
		solns.clear();
		
		solver = new NQueensSolver(new Board(10));
		assertEquals(724, solver.solve(solns));
		assertEquals(7240, solns.size());
		
		solns.clear();
		
		solver = new NQueensSolver(new Board(5));
		assertEquals(10, solver.solve(solns));
		assertEquals(50, solns.size());
		
		solns.clear();
		
		solver = new NQueensSolver(new Board(6));
		assertEquals(4, solver.solve(solns));
		assertEquals(24, solns.size());
	}
	
	/**
	 * Test special case 1
	 */
	 @Test
	 public void testSolveDim1() {
		 Board b = new Board(1);
		 NQueensSolver solver = new NQueensSolver(b);
		 
		 ArrayList<Piece> solns = new ArrayList<Piece>();
		 
		 assertEquals(1, solver.solve(solns));
	 }
	 
	 /**
	  * Test special cases 2 and 3
	  */
	 @Test
	 public void testSolveDim23() {
		 Board b = new Board(2);
		 NQueensSolver solver = new NQueensSolver(b);
		 
		 ArrayList<Piece> solns = new ArrayList<Piece>();
		 
		 assertEquals(0, solver.solve(solns));
		 
		 b = new Board(3);
		 solver = new NQueensSolver(b);
		 
		 assertEquals(0, solver.solve(solns));
	 }

}
