/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import java.util.ArrayList;
import java.util.List;

import puzzle.Backtracker;
import puzzle.Board;
import puzzle.NCustomPieceSolver;
import puzzle.NQueensSolver;
import puzzle.PuzzleSolver;
import puzzle.QueensKnightsSolver;
import puzzle.RooksSolver;

import puzzle.Piece;

/**
 * Model for the MVC
 * Basically wraps all of our classes and actions
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PuzzlesModel {
	private MainGuiView view;
	private ArrayList<Piece> solutions;
	private PuzzleSolver solver;
	
	/**
	 * Constructor
	 * 
	 * @param view		The view for the controller
	 */
	public PuzzlesModel(MainGuiView view) {
		this.view      = view;
		this.solutions = new ArrayList<Piece>();
		this.solver	   = null;
	}

	/**
	 * Handle events which directly relate to our model
	 * 
	 * @param src	Action code performed
	 * @return	Result of some input value (if necessary). If no input needed, ignore it.
	 * @see ActionTypes
	 */
	public int handleAction(ActionTypes src) {
		assert(this.view != null);
		switch(src){
			case SOLVE_PUZZLE:
				String result = this.view.requestInput("Board dimension?");
				if(result == null) // Hit cancel
					return -1;
				int retval = Integer.parseInt(result);
				if(retval < 0) {
					retval *= -1;
					this.view.sendNotification("You entered a negative board dimension which is impossible.\nUsing the absolute value to solve.");
				}
				return retval;
		}
		return -1;
	}
	
	/**
	 * Solve a given puzzle
	 * 
	 * @param puzzleId	Determine which puzzle to solve based on this ID
	 * @param dimension	Dimension of the board
	 * @return	Result of solver (-1 = error)
	 * @see PuzzleTypes
	 */
	public int solvePuzzle(PuzzleTypes puzzleId, int dimension) {
		assert(this.solutions != null);
		
		this.solutions.clear();
		
		switch(puzzleId) {
			case N_ROOKS:
				this.solver = new RooksSolver(new Board(dimension));
				break;
			case M_QUEENSKNIGHTS:
				this.solver = new QueensKnightsSolver(new Board(dimension));
				break;
			case M_CUSTOM:
				this.solver = new NCustomPieceSolver(new Board(dimension));
				break;
			default:
				this.solver = new NQueensSolver(new Board(dimension));
				break;
		}
		
		assert(solver != null);
		
		return solver.solve(this.solutions);
	}
	
	/**
	 * Draw the solutions to GUI
	 * 
	 * @param numSolns	The number of total solutions found
	 * @param dim		The dimension of the board
	 * 
	 * NOTE: Should be called AFTER solvePuzzle (depends on the solution list)
	 */
	public void drawSolutions(int numSolns, int dim) {
		this.view.drawChessSolutions(numSolns, dim, this.solutions);
	}
	
	/**
	 * Convert piece position to board coordinate (assuming linearly related board)
	 * 
	 * @param piece		The piece to find the proper position for
	 * @return	The index of the square on the board where the piece belongs
	 */
	public static int convertPositionToIdx(Piece piece) {
		assert(piece != null);
		Board board = piece.getParentBoard();
		
		assert(board != null);
		
		int dim = board.getDimension();
		
		return convertPositionToIdx(piece.getX(), piece.getY(), dim);
	}
	
	/**
	 * Convert a row (i.e. coord (0, y)) into an index
	 * 
	 * @param piece	The piece with the row to be converted
	 * @return	An array of integers. The first element contains the starting index
	 * 	of the row and the second contains the end of the row
	 */
	public static int[] convertRowToIdx(Piece piece) {
		assert(piece != null);
		Board board = piece.getParentBoard();
		
		assert(board != null);
		
		int x[] = new int[2];
		
		final int dim = board.getDimension();
		final int row = piece.getY();
		
		x[0] = row*dim;
		x[1] = x[0]+dim;
		
		return x;
	}
	
	/**
	 * Convert piece position to board coordinate (assuming linearly related board)
	 * (private helper)
	 * 
	 * @param x			The piece's x position on the game board
	 * @param y			The piece's y position on the game board
	 * @param dim		Board dimension (trusted as passed by this class)
	 * @return	The index of the square on the board where the piece belongs
	 */
	private static int convertPositionToIdx(int x, int y, int dim) {
		return (y*dim)+x;
	}
	
	/**
	 * Convert index to piece position on game board
	 * 
	 * @param idx		The index to convert
	 * @param dim		The dimensions of the board
	 * @return	An array of size 2 containing coordinates (x,y) on the game board
	 */
	public static int[] convertIdxToPosition(int idx, int dim) {
		int retval[] = new int[2];
		
		int x = idx % dim;
		int y = (idx - x)/dim;
		
		retval[0] = x;
		retval[1] = y;
		
		return retval;
	}
	
	/**
	 * Provide a list of square indices which should be highlighted
	 * 
	 * @param piece		Piece which to check
	 * @return	List of square indices to highlight
	 */
	public static List<Integer> findAttackable(Piece piece) {
		assert(piece != null);
		Board board = piece.getParentBoard();
		
		assert(board != null);
		int dim = board.getDimension();
		
		ArrayList<Integer> solns = new ArrayList<Integer>();
		
		for(int i = 0 ; i < dim ; ++i)
			for(int j = 0 ; j < dim ; ++j)
				if(piece.canAttack(i, j))
					solns.add(convertPositionToIdx(i, j, dim));
		
		return solns;
	}
	
	/**
	 * Find the solution list to animate the solver
	 * 
	 * @param soln		The solution to reproduce
	 * @return	A list (in order) of the pieces chosen. Null if error or no routes found
	 */
	public List<Piece> findAnimationSolution(List<Piece> soln) {
		assert(this.solver != null);
		
		Backtracker b = (Backtracker)this.solver;
		
		return b.backtrackSoln(soln);
	}
}
