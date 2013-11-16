/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import games.Piece;
import gui.ElementController;
import gui.Model;
import gui.View;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.List;

import javax.swing.JPanel;


/**
 * The chess square controller class which processes
 * mouse input for chess squares
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class ChessSquareController implements ElementController {
	private Piece piece;
	private JPanel board;
	
	/**
	 * Constructor
	 * 
	 * @param board		The JPanel board we are on
	 * @param p			The piece on which the controller operators
	 */
	public ChessSquareController(JPanel board, Piece p) {
		this.board	   = board;
		this.piece	   = p;
	}
	
	/**
	 * Method which should highlight appropriate squares
	 * 
	 * @param arg0	Part of required signature. Unusued.
	 */
	@Override
	public void mouseEntered(MouseEvent arg0) {
		assert(this.piece != null && this.board != null);
		List<Integer> solns = PuzzlesModel.findAttackable(this.piece);
		for(Integer x : solns) {
			View.highlightSquare(this.board, x);
		}
	}

	/**
	 * Method to unhighlight all squares
	 * 
	 * @param arg0	Part of required signature. Unusued.
	 */
	@Override
	public void mouseExited(MouseEvent arg0) {
		assert(this.piece != null);
		assert(this.piece.getParentBoard() != null);
		
		int dim = this.piece.getParentBoard().getDimension();
		
		List<Integer> solns = PuzzlesModel.findAttackable(this.piece);
		for(Integer x : solns) {
			int coords[] = PuzzlesModel.convertIdxToPosition(x, dim);
			View.defaultSquareColor(this.board, x, coords);
		}
	}
	
	/**
	 * Function to send event to parent for processing
	 */
	@Override
	public void mouseClicked(MouseEvent arg0) {
		this.board.dispatchEvent(arg0);
	}
	
	/**
	 * Generate a new instance of the controller for elements (not boards)
	 * 
	 * @param board		The board "object" which the controller works on
	 * @param piece		The piece passed to the controller
	 * @return	A new instance of the particular controller
	 */
	@Override
	public MouseListener newController(JPanel board, Piece piece) {
		return new ChessSquareController(board, piece);
	}

	// Methods we are required to implement, but are currently
	// unimportant
	@Override
	public void mousePressed(MouseEvent e) {}
	@Override
	public void mouseReleased(MouseEvent e) {}
	@Override
	public MouseListener newController(JPanel board, List<Piece> p, Model m) {
		return null;
	}
}
