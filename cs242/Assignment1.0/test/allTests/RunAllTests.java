/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package allTests;

import junit.framework.TestSuite;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;

import puzzle.test.PuzzleTests;

/**
 * Runs all test cases for the entire software package
 * @author Dennis J. McWherter, Jr.
 *
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({PuzzleTests.class})
public class RunAllTests extends TestSuite {
}
