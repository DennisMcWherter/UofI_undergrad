/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;

import puzzle.Backtracker;
import puzzle.Board;
import puzzle.NQueensSolver;
import puzzle.Piece;
import puzzle.Queen;

/**
 * Backtracker class test cases
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class BacktrackerTest {
	private Backtracker bt;
	private Board board;
	private Board board2;
	private List<Piece> pieces;
	private List<Piece> pieces2;

	/**
	 * Setup test cases
	 * 
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board  = new Board(4);
		board2 = new Board(4);
		bt    = new NQueensSolver(board);
		
		// Setup the board
		board.placePiece(1, 0, new Queen());
		board.placePiece(3, 1, new Queen());
		board.placePiece(0, 2, new Queen());
		board.placePiece(2, 3, new Queen());
		
		// A second board
		board2.placePiece(2, 0, new Queen());
		board2.placePiece(0, 1, new Queen());
		board2.placePiece(3, 2, new Queen());
		board2.placePiece(1, 3, new Queen());
		
		// Add pieces to a solution list
		pieces = new ArrayList<Piece>();
		for(Piece p : board.getPieces())
			pieces.add(p);
				
		pieces2 = new ArrayList<Piece>();
		for(Piece p : board2.getPieces())
			pieces2.add(p);
	}

	/**
	 * Test the backtrack to solution method
	 */
	@Test
	public void testBacktrackSoln() {
		List<Piece> solns = bt.backtrackSoln(pieces);
		assertNotNull(solns);
		assertEquals(9, solns.size());
		
		bt = new NQueensSolver(board2);
		pieces.clear();
		
		for(Piece p : board2.getPieces())
			pieces.add(p);
		
		solns = bt.backtrackSoln(pieces);
		assertNotNull(solns);
		assertEquals(8, solns.size());
	}

	/**
	 * Test the isSameSolution method
	 */
	@Test
	public void testIsSameSolution() {
		assertTrue(bt.isSameSolution(pieces, board));
		assertTrue(bt.isSameSolution(pieces2, board2));
	}

}
