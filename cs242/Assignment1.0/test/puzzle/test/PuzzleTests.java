/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 * 
 */
package puzzle.test;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

/**
 * Runs all test cases for the puzzle package
 * @author Dennis J. McWherter, Jr.
 * 
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({BoardTest.class, NQueensSolverTest.class,
	PieceTest.class, QueenTest.class})
public class PuzzleTests {
}
