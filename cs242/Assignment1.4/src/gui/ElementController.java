/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import games.Piece;

import java.awt.event.MouseListener;
import java.util.List;

import javax.swing.JPanel;

/**
 * Base class for controllers of "Elements"
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract interface ElementController extends MouseListener {
	/**
	 * Generate a new instance of the controller (boards)
	 * 
	 * @param board		The board "object" which the controller works on
	 * @param pieces	The pieces passed to the controller
	 * @param model		The model passed to the controller
	 * @return	A new instance of the particular controller
	 */
	public abstract MouseListener newController(JPanel board, List<Piece> pieces, Model model);
	
	/**
	 * Generate a new instance of the controller for elements (not boards)
	 * 
	 * @param board		The board "object" which the controller works on
	 * @param piece		The piece passed to the controller
	 * @return	A new instance of the particular controller
	 */
	public abstract MouseListener newController(JPanel board, Piece piece);
}
