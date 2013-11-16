/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package allTests;

import junit.framework.TestSuite;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import games.feudal.test.FeudalTests;
import games.puzzle.test.PuzzleTests;
import games.test.GamesTests;
import gui.feudal.test.FeudalGuiTests;
import gui.puzzles.test.GuiPuzzlesTest;
import gui.test.GuiTests;

/**
 * Runs all test cases for the entire software package
 * @author Dennis J. McWherter, Jr.
 *
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({GamesTests.class, PuzzleTests.class, GuiTests.class,
	GuiPuzzlesTest.class, FeudalTests.class, FeudalGuiTests.class})
public class RunAllTests extends TestSuite {
}
