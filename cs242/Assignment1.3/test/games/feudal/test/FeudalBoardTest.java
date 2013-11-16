/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.test;

import static org.junit.Assert.*;
import games.feudal.FeudalBoard;
import games.feudal.GameManager;

import org.junit.Before;
import org.junit.Test;

/**
 * Feudal board test
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalBoardTest {
	FeudalBoard board;
	
	/**
	 * Setup
	 */
	@Before
	public void setupTests() {
		board = new FeudalBoard(new GameManager(2));
	}

	/**
	 * Check the board dimension
	 */
	@Test
	public void testBoardDimension() {
		assertEquals(24, board.getDimension());
	}
	
	/**
	 * Make sure it starts empty
	 */
	@Test
	public void testEmptyBoard() {
		assertEquals(0, board.getPieces().size());
	}
	
	/**
	 * Test set terrain
	 */
	@Test
	public void testToggleTerrain() {
		assertFalse(board.checkRough(0, 0));
		
		board.toggleRoughTerrain(0, 0);
		assertTrue(board.checkRough(0, 0));
		
		board.toggleRoughTerrain(0, 0);
		assertFalse(board.checkRough(0, 0));
	}
	
	/**
	 * Test set mountains
	 */
	@Test
	public void testToggleMountain() {
		assertFalse(board.checkMountain(0, 0));
		
		board.toggleMountain(0, 0);
		assertTrue(board.checkMountain(0, 0));
		
		board.toggleMountain(0, 0);
		assertFalse(board.checkMountain(0, 0));
	}
	
	/**
	 * Check set castles
	 */
	@Test
	public void testSetCastle() {
		assertFalse(board.checkCastle(0, 0));
		
		assertFalse(board.setCastle(0, 0, 0, 1));
		assertFalse(board.checkCastle(0, 0));
		
		assertTrue(board.setCastle(0, 0, 0, 0));
		assertTrue(board.checkCastle(0, 0));
	}
}
