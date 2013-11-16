/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 * 
 */
package games.test;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

/**
 * Runs all the generic games test methods which can be shared
 * across puzzles, feudal, and whatever else may be shared
 * @author Dennis J. McWherter, Jr.
 * 
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({BoardTest.class, BacktrackerTest.class, PieceTest.class})
public class GamesTests {
}
