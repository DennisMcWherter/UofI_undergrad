/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;
import games.feudal.King;

import org.junit.Before;
import org.junit.Test;

public class KingTest {
	FeudalBoard board;
	King piece;
	
	/**
	 * Setup tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new FeudalBoard(new GameManager(2));
		piece = new King();
		board.placePiece(0, 0, piece);
	}

	/**
	 * Test can attack method
	 */
	@Test
	public void testCanAttack() {
		assertFalse(piece.canAttack(0, -1));
		assertFalse(piece.canAttack(0, 0));
		assertFalse(piece.canAttack(1, 2));
		
		assertTrue(piece.canAttack(2, 2));
		assertTrue(piece.canAttack(1, 1));
		assertTrue(piece.canAttack(0, 2));
		assertTrue(piece.canAttack(2, 0));
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
