/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.List;

import javax.swing.JPanel;

import puzzle.Piece;

/**
 * The chess square controller class which processes
 * mouse input for chess squares
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class ChessSquareController implements MouseListener {
	private Piece piece;
	private JPanel board;
	
	/**
	 * Constructor
	 * 
	 * @param board		The JPanel board we are on
	 * @param p			The piece on which the controller operators
	 */
	ChessSquareController(JPanel board, Piece p) {
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
			MainGuiView.highlightSquare(this.board, x);
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
			MainGuiView.defaultSquareColor(this.board, x, coords);
		}
	}
	
	/**
	 * Function to send event to parent for processing
	 */
	@Override
	public void mouseClicked(MouseEvent arg0) {
		this.board.dispatchEvent(arg0);
	}

	// Methods we are required to implement, but are currently
	// unimportant
	@Override
	public void mousePressed(MouseEvent e) {}
	@Override
	public void mouseReleased(MouseEvent e) {}
}
