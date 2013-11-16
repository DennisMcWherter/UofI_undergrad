/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;
import games.feudal.Pikeman;

import org.junit.Before;
import org.junit.Test;

/**
 * Test class for piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PikemanTest {
	FeudalBoard board;
	Pikeman piece;

	/**
	 * Setup tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new FeudalBoard(new GameManager(2));
		piece = new Pikeman();
		board.placePiece(0, 0, piece);
	}

	/**
	 * Test canAttack method
	 */
	@Test
	public void testCanAttack() {
		assertFalse(piece.canAttack(-1, 0));
		assertFalse(piece.canAttack(13, 0));
		assertFalse(piece.canAttack(2, 2));
		assertFalse(piece.canAttack(0, 0));
		assertFalse(piece.canAttack(0, 13));
		
		assertTrue(piece.canAttack(0, 12));
		assertTrue(piece.canAttack(12, 0));
		assertTrue(piece.canAttack(1, 1));
		assertTrue(piece.canAttack(11, 0));
		assertTrue(piece.canAttack(0, 11));
	}
	
	/**
	 * Test can attack with landscape
	 */
	@Test
	public void testCanAttackWithLandscape() {
		board.toggleRoughTerrain(0, 1);
		board.toggleMountain(1, 0);
		
		assertTrue(piece.canAttack(0, 2));
		assertFalse(piece.canAttack(2, 0));
		
		board.toggleRoughTerrain(0, 1);
		board.toggleMountain(0, 1);
		
		assertFalse(piece.canAttack(0, 2));
		assertTrue(piece.canAttack(1, 1));
	}

}
