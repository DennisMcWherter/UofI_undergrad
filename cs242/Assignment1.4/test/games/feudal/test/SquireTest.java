package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;
import games.feudal.Squire;

import org.junit.Before;
import org.junit.Test;

/**
 * Test class for piece
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class SquireTest {
	FeudalBoard board;
	Squire piece;

	/**
	 * Setup tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new FeudalBoard(new GameManager(2));
		piece = new Squire();
		board.placePiece(0, 0, piece);
	}

	/**
	 * test canAttack method
	 */
	@Test
	public void testCanAttack() {
		assertTrue(piece.canAttack(1, 2));
		assertTrue(piece.canAttack(2, 1));
		
		assertFalse(piece.canAttack(0, 0));
		assertFalse(piece.canAttack(0, 1));
		assertFalse(piece.canAttack(1, 0));
		assertFalse(piece.canAttack(-1, 0));
		assertFalse(piece.canAttack(0, -1));
		assertFalse(piece.canAttack(0, 3));
	}
	
	/**
	 * Test can attack with landscape
	 */
	@Test
	public void testCanAttackWithLandscape() {
		board.toggleRoughTerrain(1, 2);
		board.toggleMountain(0, 1);
		
		assertTrue(piece.canAttack(1, 2));
		
		board.toggleMountain(1, 0);
		
		assertFalse(piece.canAttack(1, 2));
	}

}
