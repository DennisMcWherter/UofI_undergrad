/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import games.Piece;

import java.awt.Color;
import java.awt.Component;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;

/**
 * Base class for most GUI views
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class View {
	
	/**
	 * Add a particular piece to the board
	 * 
	 * @param board		 The object which is the board
	 * @param controller The controller object to recreate
	 * @param model		 The model object to use
	 * @param where		 Which square to place on
	 * @param piece		 The piece to place on the board
	 */
	public static void addSinglePiece(JPanel board, ElementController controller, Model model, int where, Piece piece) {
		assert(piece != null && board != null);
		
		BufferedImage image = model.getPieceImg(piece);
		
		assert(image != null);
		
		JPanel square = (JPanel)board.getComponent(where);
		
		square.add(new JLabel(new ImageIcon(image)));
		
		// Paranoia
		for(MouseListener x : square.getMouseListeners())
			square.removeMouseListener(x);
		
		if(controller != null)
			square.addMouseListener(controller.newController(board, piece));
		square.updateUI();
	}
	
	/**
	 * Add a set of pieces to a board
	 * 
	 * @param model		  The model which this board uses 
	 * @param bController The controller for the board
	 * @param controller  The controller which the board uses for elements
	 * @param board		  The board to add pieces
	 * @param pieces	  The piece set
	 * @param dim		  Dimensions
	 */
	public static void addPieces(Model model, ElementController bController, ElementController controller, JPanel board, List<Piece> pieces, int dim) {
		assert(model != null && board != null && pieces != null);

		// Make sure we have a listener on this board to animate it
		for(MouseListener x : board.getMouseListeners())
			board.removeMouseListener(x);
						
		if(bController != null) {
			board.addMouseListener(bController.newController(board, pieces, model));
			board.updateUI();
		}
		
		for(int i = 0 ; i < pieces.size() ; ++i) {
			Piece piece = pieces.get(i);
			int square = Model.convertPositionToIdx(piece);
			View.addSinglePiece(board, controller, model, square, piece);
		}
	}
	
	/**
	 * Choose a color for the board square
	 * 
	 * @param x		Linear number (position on board)
	 * @return	The appropriate color for the square
	 */
	public static Color chooseColor(int x) {
		if(x % 2 == 0)
			return new Color(205, 205, 193);
		return new Color(0, 100, 0);
	}
	
	/**
	 * Clear a board of all pieces
	 * 
	 * @param board		The board object to clear items from
	 */
	public static void clearBoard(JPanel board) {
		assert(board != null);
		
		for(Component c : board.getComponents()) {
			JPanel square = (JPanel)c;
			square.removeAll();
			square.updateUI();
		}
	}
	
	/**
	 * Clear a square of all components
	 * 
	 * @param board		The board the square is on
	 * @param idx		The square index on the board
	 */
	public static void clearSquare(JPanel board, int idx) {
		assert(board != null);
		JPanel c = (JPanel)board.getComponent(idx);
		c.removeAll();
		
		for(MouseListener x : c.getMouseListeners())
			c.removeMouseListener(x);
		
		c.updateUI();
	}
		
	/**
	 * Change a square color
	 * 
	 * @param board		The board which the square is on
	 * @param idx		The index of the square on the board
	 * @param coords	An array of (x,y) coordinates where the square is located
	 */
	public static void defaultSquareColor(JPanel board, int idx, int[] coords) {
		assert(board != null && coords != null);
		JPanel square = (JPanel)board.getComponent(idx);
		square.setBackground(View.chooseColor(coords[0]+coords[1]));
	}
	
	/**
	 * Highlight a particular square
	 * 
	 * @param board		The board the square is on
	 * @param idx		The index of the square on the board
	 */
	public static void highlightSquare(JPanel board, int idx) {
		assert(board != null);
		JPanel square = (JPanel)board.getComponent(idx);
		square.setBackground(new Color(255,255,0));
	}
}
