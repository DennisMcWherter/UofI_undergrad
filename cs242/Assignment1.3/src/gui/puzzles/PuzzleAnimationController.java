/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import games.Piece;
import gui.ElementController;
import gui.View;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.List;
import java.util.Stack;

import javax.swing.JPanel;


/**
 * Puzzle animation controller class
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PuzzleAnimationController implements ActionListener {
	private Stack<Piece> soln;
	private JPanel board;
	private ChessBoardController controller;
	private PuzzlesModel model;
	
	private static final ElementController SQUARE_CONTROLLER = new ChessSquareController(null, null); 
	
	PuzzleAnimationController(JPanel board, List<Piece> pieces, PuzzlesModel model, ChessBoardController controller) {
		assert(pieces != null);
		this.controller = controller;
		this.model		= model;
		this.board		= board;
		this.soln		= new Stack<Piece>();
		
		for(int i = pieces.size()-1 ; i >= 0 ; i--)
			this.soln.push(pieces.get(i));
	}

	/**
	 * Method which calls the appropriate method
	 * to update the UI
	 * 
	 * @param arg0	Required by extension (unused)
	 */
	@Override
	public void actionPerformed(ActionEvent arg0) {
		assert(this.soln != null && this.board != null && this.controller != null);
		Piece piece = this.soln.pop();
		
		int where    = PuzzlesModel.convertPositionToIdx(piece);
		int bounds[] = PuzzlesModel.convertRowToIdx(piece);
			
		// Clear the row
		for(int i = bounds[0] ; i < bounds[1]; ++i) {
			View.clearSquare(this.board, i);
		}

		View.addSinglePiece(this.board,
				SQUARE_CONTROLLER,
				this.model,
				where,
				piece);
		
		if(this.soln.isEmpty())
			this.controller.stopTimer();
	}

}
