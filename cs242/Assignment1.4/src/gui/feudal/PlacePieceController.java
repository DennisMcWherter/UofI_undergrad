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
public class PlacePieceController implements MouseListener {
	private FeudalModel model;
	
	public PlacePieceController(FeudalModel model) {
		this.model = model;
	}

	/**
	 * Handle mouse clicks when placing pieces
	 * 
	 * @param arg0	The argument passed in - used for getting index
	 */
	@Override
	public void mouseClicked(MouseEvent arg0) {
		JPanel square = (JPanel)arg0.getSource();
		JPanel parent = (JPanel)square.getParent();
		
		int idx = 0;
		
		for(idx = 0 ; idx < parent.getComponentCount() ; ++idx)
			if(parent.getComponent(idx) == square)
				break;
		
		int[] coords = Model.convertIdxToPosition(idx, FeudalBoard.BOARD_SIZE);
		
		if(this.model.initialPlace(coords[0], coords[1]))
			this.model.readyToStart();
	}

	// Unimplemented
	@Override
	public void mouseEntered(MouseEvent arg0) {}
	@Override
	public void mouseExited(MouseEvent arg0) {}
	@Override
	public void mousePressed(MouseEvent arg0) {}
	@Override
	public void mouseReleased(MouseEvent arg0) {}

}
