/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;
import org.junit.Test;

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
	
	/**
	 * Test for valid constructor
	 */
	@Test
	public void testValidConstructor1() {
		Board board = new Board();
		
		assertEquals("Result", 5, board.getDimension());
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
		assertEquals(5, board3.getDimension());
	}
	

	/**
	 * Test whether pieces appropriately set are put in the proper place
	 */
	@Test
	public void testPlacePiece() {
		Board board = new Board(10);
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		Queen q3 = new Queen();
				
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
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		Queen q3 = new Queen();
		Queen q4 = new Queen();
		
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
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		
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
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		Queen q3 = new Queen();
		Queen q4 = new Queen();
		
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
		Board b  = new Board();
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		
		b.placePiece(0, 0, q1);
		b.placePiece(0, 1, q2);
		
		assertSame(q1, b.getPieceAt(0, 0));
		assertSame(q2, b.getPieceAt(0, 1));
		
		b.removePieceAt(0, 0);
		assertNull(b.getPieceAt(0, 0));
		assertSame(q2, b.getPieceAt(0, 1));
	}
	
	/**
	 * Test removePieceAt functionality with bad input
	 */
	@Test
	public void testRemovePieceAtDirty() {
		Board b  = new Board();
		Queen q1 = new Queen();
		Queen q2 = new Queen();
		
		b.placePiece(0, 0, q1);
		b.placePiece(0, 1, q2);
		
		assertSame(q1, b.getPieceAt(0, 0));
		assertSame(q2, b.getPieceAt(0, 1));
		
		b.removePieceAt(-1, 0);
		b.removePieceAt(0, -1);
		b.removePieceAt(4, 4);
		b.removePieceAt(200, 0);
		b.removePieceAt(0, 200);
		b.removePieceAt(200, 200);
		
		assertSame(q1, b.getPieceAt(0, 0));
		assertSame(q2, b.getPieceAt(0, 1));
	}

	/**
	 * Test standard getPieceAt functionality
	 */
	@Test
	public void testGetPieceAt() {
		Board board = new Board();
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
		Board board = new Board();
		
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
		Board board = new Board();
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
		Board board = new Board();
		
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
		assertEquals(5, board3.getDimension());
	}

	/**
	 * Test print method works properly
	 */
	@Test
	public void testPrint() {
		Board board = new Board();
		board.print();
	}

}
