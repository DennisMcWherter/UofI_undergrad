package games.feudal.test;

import static org.junit.Assert.*;

import java.util.ArrayList;

import games.Piece;
import games.feudal.Archer;
import games.feudal.Duke;
import games.feudal.GameManager;
import games.feudal.King;
import games.feudal.Knight;
import games.feudal.Pikeman;
import games.feudal.Prince;
import games.feudal.Sergeant;
import games.feudal.Squire;

import org.junit.Before;
import org.junit.Test;

public class GameManagerTest {
	GameManager manager;

	@Before
	public void setUp() throws Exception {
		manager = new GameManager(2);
	}

	/**
	 * Test getTurn method
	 */
	@Test
	public void testGetTurn() {
		assertEquals(0, manager.getTurn());
		for(int i = 0 ; i < 13; ++i) manager.getNextPiece();
		assertEquals(1, manager.getTurn());
	}
	
	/**
	 * Test resetTurns method
	 */
	@Test
	public void testResetTurns() {
		assertEquals(0, manager.getTurn());
		for(int i = 0 ; i < 13; ++i) manager.getNextPiece();
		assertEquals(1, manager.getTurn());
		manager.resetTurns();
		assertEquals(0, manager.getTurn());
	}
	
	/**
	 * Test complete turn method
	 */
	@Test
	public void testCompleteTurn() {
		assertEquals(0, manager.getTurn());
		manager.completeTurn(new ArrayList<Piece>());
		assertEquals(1, manager.getTurn());
	}

	/**
	 * Test getNextPiece method
	 */
	@Test
	public void testGetNextPiece() {
		assertTrue(manager.getNextPiece() instanceof King);
		assertTrue(manager.getNextPiece() instanceof Prince);
		assertTrue(manager.getNextPiece() instanceof Duke);
		assertTrue(manager.getNextPiece() instanceof Knight);
		assertTrue(manager.getNextPiece() instanceof Knight);
		assertTrue(manager.getNextPiece() instanceof Sergeant);
		assertTrue(manager.getNextPiece() instanceof Sergeant);
		assertTrue(manager.getNextPiece() instanceof Squire);
		assertTrue(manager.getNextPiece() instanceof Archer);
		assertTrue(manager.getNextPiece() instanceof Pikeman);
		assertTrue(manager.getNextPiece() instanceof Pikeman);
		assertTrue(manager.getNextPiece() instanceof Pikeman);
		assertTrue(manager.getNextPiece() instanceof Pikeman);
	}
}
