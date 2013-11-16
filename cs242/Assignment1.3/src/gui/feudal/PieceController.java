/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal;

import games.feudal.FeudalBoard;
import gui.Model;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JPanel;

/**
 * Controller to move pieces
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class PieceController implements MouseListener {
	private FeudalModel model;
	private boolean moveable;
	
	public PieceController(FeudalModel model, boolean moveable) {
		this.model    = model;
		this.moveable = moveable;
	}

	@Override
	public void mouseClicked(MouseEvent arg0) {
		JPanel square = (JPanel)arg0.getSource();
		JPanel parent = (JPanel)square.getParent();
		
		int idx = 0;
		
		for(idx = 0 ; idx < parent.getComponentCount() ; ++idx)
			if(parent.getComponent(idx) == square)
				break;
		
		int[] coords = Model.convertIdxToPosition(idx, FeudalBoard.BOARD_SIZE);
		
		if(this.moveable) {
			this.model.movePiece(this.model.getPieceAt(coords[0], coords[1]));
		} else {
			this.model.placePiece(coords[0], coords[1]);
		}
	}

	// Not implemented
	@Override
	public void mouseEntered(MouseEvent arg0) {}
	@Override
	public void mouseExited(MouseEvent arg0) {}
	@Override
	public void mousePressed(MouseEvent arg0) {}
	@Override
	public void mouseReleased(MouseEvent arg0) {}

}
