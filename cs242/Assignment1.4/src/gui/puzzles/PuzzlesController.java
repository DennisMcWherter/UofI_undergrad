/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import gui.ActionTypes;
import gui.MenuItems;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * Class for Main GUI controller
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PuzzlesController implements ActionListener {
	private PuzzlesModel model;
	private MainPuzzlesView view;
	
	/**
	 * Constructor
	 * 
	 * @param model		Driver model to use for GUI
	 * @param view		Main GUI view reference
	 */
	public PuzzlesController(PuzzlesModel model, MainPuzzlesView view) {
		this.model = model;
		this.view  = view;
	}
	
	/**
	 * Action handler
	 * 
	 * @param ae	Action to be handled
	 */
	@Override
	public void actionPerformed(ActionEvent ae) {
		assert(this.model != null);
		String what = ae.getActionCommand();

		for(PuzzleTypes type : PuzzleTypes.values()) {
			if(type.isEqual(what)) {
				int dimension = this.model.handleAction(ActionTypes.SOLVE_PUZZLE);
				int result = solvePuzzle(type, dimension);
				this.model.drawSolutions(result, dimension);
				return;
			}
		}
		
		if(what.equalsIgnoreCase(MenuItems.FILE_EXIT))
			System.exit(0);
		else if(what.equalsIgnoreCase(MenuItems.HELP_HELP))
			displayHelp();
		else if(what.equalsIgnoreCase(MenuItems.HELP_ABOUT))
			displayAbout();
	}
	
	/**
	 * Determine which puzzle to solve, then solve it
	 * 
	 * @param puzzle	Determine which puzzle to solve
	 * @param dimension	Dimension of the board
	 * @param pieces	Solution set (will be modified by solver)
	 * @return	Result of solver (-1 = error)
	 */
	public int solvePuzzle(PuzzleTypes puzzle, int dimension) {
		assert(this.model != null);
		return this.model.solvePuzzle(puzzle, dimension);
	}
	
	/**
	 * Display help
	 */
	private void displayHelp() {
		assert(this.view != null);
		this.view.sendNotification("To use this application select a puzzle you wish to solve from\n"
				+"the \"Solvers\" menu. Then input the dimension of the board at the prompt.\nNOTE: "
				+"If dimension is too large program will take a long time to compute.\n\nWhen solutions "
				+"are displayed, you can hover a piece to view its attack pattern or click on\nthe board "
				+"to watch the solver solve that particular board.\n\nFeature List:\n"
				+"Solve several puzzles from the \"Solvers\" menu\n"
				+"Hover a piece to view attack patterns\n"
				+"Animation to watch the solver solve a board");
	}
	
	/**
	 * Display about
	 */
	private void displayAbout() {
		assert(this.view != null);
		this.view.sendNotification("This application was created by Dennis J. McWherter, Jr. for\n"
				+"a CS242 assignment at the University of Illinois at Urbana-Champaign");
	}
}
