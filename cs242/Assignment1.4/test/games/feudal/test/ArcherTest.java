/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.Archer;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;

import org.junit.Before;
import org.junit.Test;

/**
 * Test class for piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class ArcherTest {
	FeudalBoard board;
	Archer piece;

	/**
	 * Setup tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new FeudalBoard(new GameManager(2));
		piece = new Archer();
		board.placePiece(0, 0, piece);
	}

	/**
	 * Test the can attack method
	 */
	@Test
	public void testCanAttack() {
		assertFalse(piece.canAttack(-1, 0));
		assertFalse(piece.canAttack(0, 0));
		assertFalse(piece.canAttack(4, 0));
		assertFalse(piece.canAttack(0, 4));
		assertFalse(piece.canAttack(7, 7));
		
		assertTrue(piece.canAttack(3, 0));
		assertTrue(piece.canAttack(0, 3));
		assertTrue(piece.canAttack(2, 2));
		assertTrue(piece.canAttack(0, 1));
		assertTrue(piece.canAttack(0, 2));
		assertTrue(piece.canAttack(1, 0));
		assertTrue(piece.canAttack(1, 1));
	}

}
