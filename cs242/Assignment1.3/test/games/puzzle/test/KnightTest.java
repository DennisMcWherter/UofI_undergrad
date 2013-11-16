/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzle.test;

import static org.junit.Assert.*;
import games.Board;
import games.Piece;
import games.puzzles.Knight;

import org.junit.Before;
import org.junit.Test;



/**
 * Test class for knight piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class KnightTest {
	Knight knight;
	Board board;

	/**
	 * Initialize our tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		knight = new Knight();
		board  = new Board(8);
	}

	/**
	 * Test canAttack method
	 */
	@Test
	public void testCanAttack() {
		board.placePiece(0, 0, knight);
		
		assertTrue(knight.canAttack(1, 2));
		assertTrue(knight.canAttack(2, 1));
		
		assertFalse(knight.canAttack(0, 0));
		assertFalse(knight.canAttack(0, 1));
		assertFalse(knight.canAttack(1, 0));
		assertFalse(knight.canAttack(-1, 0));
		assertFalse(knight.canAttack(0, -1));
		assertFalse(knight.canAttack(0, 3));
		
		board.placePiece(1, 2, knight);
		
		assertTrue(knight.canAttack(0, 0));
	}
	
	/**
	 * Test get symbol
	 */
	@Test
	public void testGetSymbol() {
		assertNotNull(knight.getSymbol());
		assertFalse(knight.getSymbol().isEmpty());
	}

	/**
	 * Test getNewPiece
	 */
	@Test
	public void testgetNewPiece() {
		Piece obj = knight.getNewPiece();
		
		assertNotNull(obj);
		assertTrue(obj instanceof Knight);
	}
	
}
