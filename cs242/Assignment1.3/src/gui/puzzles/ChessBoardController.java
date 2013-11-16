/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import games.Board;
import games.Piece;
import gui.ElementController;
import gui.Model;
import gui.View;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.List;

import javax.swing.JPanel;
import javax.swing.Timer;


/**
 * Chess board controller to process mouse events
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class ChessBoardController implements ElementController {
	private JPanel board;
	private List<Piece> pieces;
	private List<Piece> soln;
	private PuzzlesModel model;
	private Timer timer;

	/**
	 * Constructor
	 * 
	 * @param board		The board which this controller operates on
	 * @param pieces	The list of pieces in the final solution
	 * @param model		The used to find valid reversed solutions 
	 */
	public ChessBoardController(JPanel board, List<Piece> pieces, Model model) {
		assert(model instanceof PuzzlesModel);
		this.board  = board;
		this.pieces = pieces;
		this.soln   = null;
		this.model  = (PuzzlesModel)model;
		
		assert(pieces != null);
		
		Board b = new Board(pieces.get(0).getParentBoard().getDimension());
		
		// To maintain data integrity, let's make sure our board isn't overwritten
		// i.e. copy to new/unique board
		for(Piece p : pieces)
			b.placePiece(p.getX(), p.getY(), p);
	}
	
	/**
	 * Method to animate the board if clicked
	 * 
	 * @param arg0	Default stubbed (unused)
	 */
	@Override
	public void mouseClicked(MouseEvent arg0) {
		assert(this.board != null && this.model != null && this.pieces != null);
		
		if(this.timer != null) // Currently solving
			return;
		
		if(this.soln == null)
			this.soln = this.model.findAnimationSolution(this.pieces);
		
		if(this.soln == null)
			return;
		
		View.clearBoard(this.board);
		
		// Make animation happen
		this.timer = new Timer(100, /* 100ms delay */
				new PuzzleAnimationController(this.board, this.soln, this.model, this));
		this.timer.setRepeats(true);
		this.timer.start();
	}
	
	/**
	 * Stop the animation timer
	 */
	public void stopTimer() {
		this.timer.stop();
		this.timer = null;
	}
	
	/**
	 * Generate a new instance of the controller
	 * 
	 * @param board		The board "object" which the controller works on
	 * @param pieces	The pieces passed to the controller
	 * @param model		The model passed to the controller
	 * @return	A new instance of the particular controller
	 */
	@Override
	public MouseListener newController(JPanel board, List<Piece> pieces, Model model) {
		return new ChessBoardController(board, pieces, model);
	}

	// Unimplemented methods since we don't need them
	@Override
	public void mouseEntered(MouseEvent arg0) {}
	@Override
	public void mouseExited(MouseEvent arg0) {}
	@Override
	public void mousePressed(MouseEvent arg0) {}
	@Override
	public void mouseReleased(MouseEvent arg0) {}
	@Override
	public MouseListener newController(JPanel j, Piece p){ return null; }
	
}
