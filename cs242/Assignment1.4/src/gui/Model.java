/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import java.awt.image.BufferedImage;

import games.Board;
import games.Piece;

/**
 * Abstract Base class for models
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class Model {
	
	/**
	 * Handle events which directly relate to our model
	 * 
	 * @param src	Action code performed
	 * @return	Result of some input value (if necessary). If no input needed, ignore it.
	 * @see ActionTypes
	 */
	public abstract int handleAction(ActionTypes src);
	
	/**
	 * Determine piece image
	 * 
	 * @param piece		The piece to find image for
	 * @return	A buffered image which represents the piece
	 */
	public abstract BufferedImage getPieceImg(Piece piece);
	
	/**
	 * Convert piece position to board coordinate (assuming linearly related board)
	 * 
	 * @param piece		The piece to find the proper position for
	 * @return	The index of the square on the board where the piece belongs
	 */
	public static int convertPositionToIdx(Piece piece) {
		assert(piece != null);
		Board board = piece.getParentBoard();
		
		assert(board != null);
		
		int dim = board.getDimension();
		
		return convertPositionToIdx(piece.getX(), piece.getY(), dim);
	}
	
	/**
	 * Convert a row (i.e. coord (0, y)) into an index
	 * 
	 * @param piece	The piece with the row to be converted
	 * @return	An array of integers. The first element contains the starting index
	 * 	of the row and the second contains the end of the row
	 */
	public static int[] convertRowToIdx(Piece piece) {
		assert(piece != null);
		Board board = piece.getParentBoard();
		
		assert(board != null);
		
		int x[] = new int[2];
		
		final int dim = board.getDimension();
		final int row = piece.getY();
		
		x[0] = row*dim;
		x[1] = x[0]+dim;
		
		return x;
	}
	
	/**
	 * Convert index to piece position on game board
	 * 
	 * @param idx		The index to convert
	 * @param dim		The dimensions of the board
	 * @return	An array of size 2 containing coordinates (x,y) on the game board
	 */
	public static int[] convertIdxToPosition(int idx, int dim) {
		int retval[] = new int[2];
		
		int x = idx % dim;
		int y = (idx - x)/dim;
		
		retval[0] = x;
		retval[1] = y;
		
		return retval;
	}
	
	/**
	 * Convert piece position to board coordinate (assuming linearly related board)
	 * (private helper)
	 * 
	 * @param x			The piece's x position on the game board
	 * @param y			The piece's y position on the game board
	 * @param dim		Board dimension (trusted as passed by this class)
	 * @return	The index of the square on the board where the piece belongs
	 */
	protected static int convertPositionToIdx(int x, int y, int dim) {
		return (y*dim)+x;
	}
}
