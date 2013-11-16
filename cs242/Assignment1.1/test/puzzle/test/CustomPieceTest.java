/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import puzzle.Board;
import puzzle.CustomPiece;
import puzzle.Piece;

/**
 * Custom piece test class
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class CustomPieceTest {
	Board board;
	CustomPiece piece;
	
	/**
	 * Setup test methods
	 */
	@Before
	public void setupTest() {
		board = new Board(8);
		piece = new CustomPiece();
		board.placePiece(0, 3, piece);
	}

	/**
	 * Test the canAttack method
	 */
	@Test
	public void testCanAttack() {
		assertTrue(piece.canAttack(2, 0));
		assertTrue(piece.canAttack(2, 6));
		
		board.placePiece(3, 3, piece);
		
		assertTrue(piece.canAttack(1, 0));
		assertTrue(piece.canAttack(1, 6));
		assertTrue(piece.canAttack(3, 4));
		assertTrue(piece.canAttack(7, 3));
		assertFalse(piece.canAttack(7, 2));
		assertFalse(piece.canAttack(3, 3));
	}
	
	/**
	 * Test the canAttack method with bad inputs
	 */
	@Test
	public void testCanAttackDirty() {
		assertFalse(piece.canAttack(-1, -1));
		assertFalse(piece.canAttack(-1, 0));
		assertFalse(piece.canAttack(0, -1));
		assertFalse(piece.canAttack(8, 8));
		assertFalse(piece.canAttack(0, 8));
		assertFalse(piece.canAttack(8, 0));
	}

	/**
	 * Test the getSymbol method
	 */
	@Test
	public void testGetSymbol() {
		String symbol = piece.getSymbol();
		assertNotNull(symbol);
		assertFalse(symbol.isEmpty());
	}

	/**
	 * Test the getNewPiece method
	 */
	@Test
	public void testGetNewPiece() {
		Piece p = piece.getNewPiece();
		assertNotNull(p);
		assertTrue(p instanceof CustomPiece);
	}

}
