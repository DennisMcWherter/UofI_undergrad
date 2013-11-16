/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.test;

import static org.junit.Assert.*;

import gui.PuzzlesController;
import gui.PuzzlesModel;
import gui.PuzzleTypes;

import org.junit.Before;
import org.junit.Test;

/**
 * Test class for GUI controller
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PuzzlesControllerTest {
	PuzzlesController controller;

	/**
	 * Setup test cases
	 * 
	 * @throws Exception	JUnit exception
	 */
	@Before
	public void setUp() throws Exception {
		controller = new PuzzlesController(new PuzzlesModel(null), null);
	}

	/**
	 * Test the solution for N Queens
	 */
	@Test
	public void testNQueens() {		
		assertEquals(92, controller.solvePuzzle(PuzzleTypes.N_QUEENS, 8));
	}
	
	/**
	 * Test N Rooks problem
	 */
	@Test
	public void testNRooks() {
		assertEquals(40320, controller.solvePuzzle(PuzzleTypes.N_ROOKS, 8));
	}

	/**
	 * Test Queens and Knights Solver
	 */
	@Test
	public void testQNKSolver() {
		assertEquals(0, controller.solvePuzzle(PuzzleTypes.M_QUEENSKNIGHTS, 0));
		assertEquals(4, controller.solvePuzzle(PuzzleTypes.M_QUEENSKNIGHTS, 8));
	}
	
	/**
	 * Test N Custom piece solver
	 */
	@Test
	public void testNCustomPieceSolver() {
		assertEquals(8, controller.solvePuzzle(PuzzleTypes.M_CUSTOM, 8));
	}
	
	/**
	 * Test bad input
	 */
	@Test
	public void testBadInput() {
		assertEquals(0, controller.solvePuzzle(PuzzleTypes.N_QUEENS, -1));
		assertEquals(0, controller.solvePuzzle(PuzzleTypes.N_QUEENS, 0));
	}
}
