/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 * 
 */
package games.puzzle.test;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;

/**
 * Runs all test cases for the puzzle package
 * @author Dennis J. McWherter, Jr.
 * 
 */
@RunWith(Suite.class)
@Suite.SuiteClasses({NQueensSolverTest.class, QueenTest.class, RookTest.class,
	KnightTest.class, RooksSolverTest.class, QueensKnightsSolverTest.class,
	CustomPieceTest.class, NCustomPieceSolverTest.class})
public class PuzzleTests {
}
