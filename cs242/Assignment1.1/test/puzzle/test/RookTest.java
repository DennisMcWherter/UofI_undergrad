/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import puzzle.Board;
import puzzle.Piece;
import puzzle.Rook;

/**
 * Test class for Rook piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class RookTest {
	private Rook rook;
	private Board board;

	/**
	 * Basic initialization
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		rook   = new Rook();
		board  = new Board(8);
	}

	/**
	 * test canAttack with valid inputs
	 */
	@Test
	public void testCanAttack() {
		board.placePiece(0, 0, rook);
		
		assertTrue(rook.canAttack(0, 1));
		assertTrue(rook.canAttack(1, 0));
		assertTrue(rook.canAttack(0, 7));
		assertTrue(rook.canAttack(7, 0));
		
		assertFalse(rook.canAttack(0, 0));
		assertFalse(rook.canAttack(-1, 0));
		assertFalse(rook.canAttack(0, -1));
		assertFalse(rook.canAttack(8, 0));
		assertFalse(rook.canAttack(0, 8));
	}
	
	/**
	 * Make sure we have the correct symbol
	 */
	@Test
	public void testGetSymbol() {
		assertNotNull(rook.getSymbol());
		assertFalse(rook.getSymbol().isEmpty());
	}
	
	/**
	 * Test getNewPiece
	 */
	@Test
	public void testgetNewPiece() {
		Piece obj = rook.getNewPiece();
		
		assertNotNull(obj);
		assertTrue(obj instanceof Rook);
	}

}
