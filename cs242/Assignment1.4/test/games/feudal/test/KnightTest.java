package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;
import games.feudal.Knight;

import org.junit.Before;
import org.junit.Test;

public class KnightTest {
	FeudalBoard board;
	Knight piece;

	/**
	 * Setup tests
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		board = new FeudalBoard(new GameManager(2));
		piece = new Knight();
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
		
		assertTrue(piece.canAttack(1, 1));
		assertTrue(piece.canAttack(0, 8));
		assertTrue(piece.canAttack(8, 0));
	}
	
	/**
	 * Test canAttack with terrain/mountain information
	 */
	@Test
	public void testCanAttackWithLandscape() {
		board.toggleMountain(0, 1);
		
		assertFalse(piece.canAttack(0, 2));
		assertTrue(piece.canAttack(10, 0));
		
		board.toggleMountain(1, 0);
		
		assertTrue(piece.canAttack(1, 1));
		
		board.toggleRoughTerrain(1, 1);
		
		assertFalse(piece.canAttack(1, 1));
	}

}
