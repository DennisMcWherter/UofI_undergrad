/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import games.Backtracker;
import games.Board;
import games.Piece;
import games.puzzles.NCustomPieceSolver;
import games.puzzles.NQueensSolver;
import games.puzzles.PuzzleSolver;
import games.puzzles.QueensKnightsSolver;
import games.puzzles.RooksSolver;
import gui.ActionTypes;
import gui.Model;

import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;




/**
 * Model for the MVC
 * Basically wraps all of our classes and actions
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PuzzlesModel extends Model {
	private MainPuzzlesView view;
	private ArrayList<Piece> solutions;
	private PuzzleSolver solver;
	
	/**
	 * Constructor
	 * 
	 * @param view		The view for the controller
	 */
	public PuzzlesModel(MainPuzzlesView view) {
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
	@Override
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
					solns.add(Model.convertPositionToIdx(i, j, dim));
		
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
	
	/**
	 * Determine piece image
	 * 
	 * @param piece		The piece to find image for
	 * @return	A buffered image which represents the piece
	 */
	public BufferedImage getPieceImg(Piece piece) {
		String symbol = piece.getSymbol();
		
		if("Q".equalsIgnoreCase(symbol))
			return MainPuzzlesView.queenImg;
		else if("R".equalsIgnoreCase(symbol))
			return MainPuzzlesView.rookImg;
		else if("N".equalsIgnoreCase(symbol))
			return MainPuzzlesView.knightImg;
		else if("C".equalsIgnoreCase(symbol))
			return MainPuzzlesView.customImg;
		
		return null;
	}
}
