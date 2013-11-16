/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 * 
 */
package games.feudal.test;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

/**
 * Runs all test cases for the feudal package
 * @author Dennis J. McWherter, Jr.
 * 
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({ArcherTest.class, DukeTest.class, KingTest.class,
	KnightTest.class, PikemanTest.class, PrinceTest.class,
	SergeantTest.class, SquireTest.class, FeudalBoardTest.class,
	GameManagerTest.class})
public class FeudalTests {
}
