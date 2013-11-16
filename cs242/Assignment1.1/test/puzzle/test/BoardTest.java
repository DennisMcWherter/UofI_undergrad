/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;
import org.junit.Test;
import org.junit.Before;

import puzzle.Board;
import puzzle.Piece;
import puzzle.Queen;

import java.util.Set;

/**
 * Tests the Board class
 * @author Dennis J. McWherter, Jr.
 * 
 */
public class BoardTest {
	Board board;
	Queen q1;
	Queen q2;
	Queen q3;
	Queen q4;
	
	/**
	 * Setup
	 */
	@Before
	public void setupTests() {
		board = new Board(5);
		q1 = new Queen();
		q2 = new Queen();
		q3 = new Queen();
		q4 = new Queen();
	}
	
	/**
	 * Test for second valid constructor
	 */
	@Test
	public void testValidConstructor2() {
		Board board  = new Board(10);
		Board board2 = new Board(2012);
		Board board3 = new Board(-3);
		
		assertEquals(10, board.getDimension());
		assertEquals(2012, board2.getDimension());
		assertEquals(0, board3.getDimension());
	}
	

	/**
	 * Test whether pieces appropriately set are put in the proper place
	 */
	@Test
	public void testPlacePiece() {
		Board board = new Board(10);
				
		assertTrue(board.placePiece(0, 0, q1));
		assertTrue(board.placePiece(5, 0, q2));
		assertTrue(board.placePiece(9, 5, q3));
		
		assertSame("Result", q1, board.getPieceAt(0, 0));
		assertSame("Result", q2, board.getPieceAt(5, 0));
		assertSame("Result", q3, board.getPieceAt(9, 5));
	}
	
	/**
	 * Test invalid piece placement
	 */
	@Test
	public void testPlacePieceInvalid() {
		Board board = new Board(10);
		
		// Clean
		assertTrue(board.placePiece(0, 0, q1));
		assertTrue(board.placePiece(5, 0, q2));
		assertTrue(board.placePiece(9, 5, q3));
		
		// Dirty
		assertFalse(board.placePiece(10, 5, q4));
		assertFalse(board.placePiece(-1, -1, q4));
		assertFalse(board.placePiece(11, 11, q4));
		
		assertSame("Result", q1, board.getPieceAt(0, 0));
		assertSame("Result", q2, board.getPieceAt(5, 0));
		assertSame("Result", q3, board.getPieceAt(9, 5));
		assertNull("Result", board.getPieceAt(11, 11));
		assertNull("Result", board.getPieceAt(-1, -1));
	}

	/**
	 * Test the general isValid method functionality
	 */
	@Test
	public void testIsValidBadConfig() {
		Board board = new Board(4);
		
		assertFalse(board.isValid());
		
		board.placePiece(0, 0, q1);
		board.placePiece(0, 1, q2);
		
		assertFalse(board.isValid());
	}
	
	/**
	 * Test a valid configuration with isValid
	 */
	@Test
	public void testIsValidGoodConfig() {
		Board board = new Board(4);
		
		// Test a working configuration
		board.placePiece(0, 2, q1);
		board.placePiece(1, 0, q2);
		board.placePiece(2, 3, q3);
		board.placePiece(3, 1, q4);
		
		assertTrue(board.isValid());
	}
	
	/**
	 * Test standard removePieceAt functionality
	 */
	@Test
	public void testRemovePieceAt() {
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		
		board.placePiece(0, 0, q1);
		board.placePiece(0, 1, q2);
		
		assertSame(q1, board.getPieceAt(0, 0));
		assertSame(q2, board.getPieceAt(0, 1));
		
		board.removePieceAt(0, 0);
		assertNull(board.getPieceAt(0, 0));
		assertSame(q2, board.getPieceAt(0, 1));
	}
	
	/**
	 * Test removePieceAt functionality with bad input
	 */
	@Test
	public void testRemovePieceAtDirty() {
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		
		board.placePiece(0, 0, q1);
		board.placePiece(0, 1, q2);
		
		assertSame(q1, board.getPieceAt(0, 0));
		assertSame(q2, board.getPieceAt(0, 1));
		
		board.removePieceAt(-1, 0);
		board.removePieceAt(0, -1);
		board.removePieceAt(4, 4);
		board.removePieceAt(200, 0);
		board.removePieceAt(0, 200);
		board.removePieceAt(200, 200);
		
		assertSame(q1, board.getPieceAt(0, 0));
		assertSame(q2, board.getPieceAt(0, 1));
	}

	/**
	 * Test standard getPieceAt functionality
	 */
	@Test
	public void testGetPieceAt() {	
		Queen queen = new Queen();
		
		board.placePiece(3, 3, queen);
		
		// Not where the queen exists
		assertNull("Result", board.getPieceAt(3, 2));
		
		// Check queen exists
		assertSame("Result", queen, board.getPieceAt(3, 3));
	}
	
	/**
	 * Test getPieceAt with bad inputs
	 */
	@Test
	public void testOutOfBoundsGetPieceAt() {
		// Non-existent piece
		assertNull("Result", board.getPieceAt(0, 0));
		
		// Non-existent dimension
		assertNull("Result", board.getPieceAt(-1, -1));
	}

	/**
	 * Test standard getPieces
	 */
	@Test
	public void testGetPieces() {
		
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		Queen q3 = new Queen();
		
		board.placePiece(0, 0, q1);
		board.placePiece(1, 0, q2);
		board.placePiece(1, 1, q3);
		
		Set<Piece> pieceSet = board.getPieces();
		
		assertEquals("Result", 3, pieceSet.size());
	}

	/**
	 * Test standard board reset
	 */
	@Test
	public void testReset() {
		// Fill the board
		for(int i = 0 ; i < 5 ; ++i)
			for(int j = 0 ; j < 5 ; ++j)
				board.placePiece(i, j, new Queen());
		
		assertEquals("Result", 25, board.getPieces().size());
		
		board.reset();
		
		assertEquals("Result", 0, board.getPieces().size());
	}
	
	/**
	 * Test getDimension method
	 */
	@Test
	public void testGetDimension() {
		Board board  = new Board(10);
		Board board2 = new Board(2012);
		Board board3 = new Board(-3);
		
		assertEquals(10, board.getDimension());
		assertEquals(2012, board2.getDimension());
		assertEquals(0, board3.getDimension());
	}

	/**
	 * Test print method works properly
	 */
	@Test
	public void testPrint() {
		board.print();
	}

}
