/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.test;

import static org.junit.Assert.*;

import java.awt.event.ActionEvent;

import gui.MainGuiController;

import org.junit.Before;
import org.junit.Test;

/**
 * The test class for the main controller
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainGuiControllerTest {
	private MainGuiController controller;
	private ActionEvent puzzleEvt;
	private ActionEvent feudalEvt;

	/**
	 * Setup for each test case
	 * 
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		controller = new MainGuiController(null);
		puzzleEvt  = new ActionEvent(this, 0, "Chess Puzzles");
		feudalEvt  = new ActionEvent(this, 0, "Feudal (Game)");
	}
	
	/**
	 * Test starting the feudal game
	 */
	@Test
	public void testInitFeudal() {
		controller.actionPerformed(feudalEvt);
		assertTrue(controller.isPlayingFeudal());
	}
	
	/**
	 * Test starting the chess solvers
	 */
	@Test
	public void testInitPuzzles() {
		controller.actionPerformed(puzzleEvt);
		assertTrue(controller.isPlayingChessPuzzles());
	}

	/**
	 * Test whether the games start properly or not (both of them)
	 */
	@Test
	public void testInitGames() {
		controller.actionPerformed(feudalEvt);
		controller.actionPerformed(puzzleEvt);
		assertTrue(controller.isPlayingFeudal());
		assertTrue(controller.isPlayingChessPuzzles());
	}

}
