/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class Backtracker extends PuzzleSolver {
	private Piece GamePiece;
	private int NumSolutions;
	private int NumPieces;
	private int M;
	private boolean FindM;
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 * @param p		Game Piece type to use
	 */
	public Backtracker(Board b, Piece p, boolean m) {
		super(b);
		GamePiece = p;
		NumSolutions = 0;
		M = 0;
		FindM = m;
	}
	
	protected int backtrack(List<Piece> pieces) {
		Board board = this.getGameBoard();
		
		if(board == null)
			return -1;
		
		// Special case: dim = 1
		if(board.getDimension() == 1) {
			board.placePiece(0, 0, GamePiece.getNewPiece());
			pieces.add((Piece)board.getPieces().toArray()[0]);
			return 1;
		}
		
		/**
		 * Procedure:
		 * Try each position (0 to n-1) in row 1
		 * Then for each position, try to find a proper place
		 * 	for next piece to be placed
		 * [x] If no valid place exists, invalid config for 
		 * 		at least previous level
		 * [x] Otherwise, continue
		 * Backtracking information found at:
		 * 	http://en.wikipedia.org/wiki/Backtracking
		 */
		for(int i = 0 ; i < board.getDimension() ; ++i) {
			// Check all columns in row 0 for possible solutions
			board.placePiece(i, 0, GamePiece.getNewPiece());
			generateValidSolns(board, pieces, 1);
			board.reset();
		}
		
		return (FindM) ? M : NumSolutions;
	}
	
	/**
	 * Increment the number of pieces on the board
	 */
	private void incrementPieces() {
		M = (++NumPieces > M) ? NumPieces : M;
	}
	
	/**
	 * Decrement the number of pieces on the board
	 */
	private void decrementPieces() {
		NumPieces--;
	}
	
	/**
	 * Checks next row for possible valid solutions
	 * 
	 * @param board		The game board which is used for testing
	 * @param pieces	The list of pieces for valid solutions
	 * @param currRow	The current row to be scanned
	 */
	private void generateValidSolns(Board board, List<Piece> pieces, int currRow) {
		if(board == null)
			return;
		
		final int boardDim = board.getDimension();
		
		// Check every spot in a row
		for(int i = 0 ; i < boardDim ; ++i) {
			board.placePiece(i, currRow, GamePiece.getNewPiece());
			if(board.isValid()) {
				incrementPieces();
				if(currRow == boardDim-1) { // Final row, so this is a valid config
					NumSolutions++;
					// Save current configuration
					for(int j = 0 ; j < boardDim ; ++j)
						pieces.add((Piece)board.getPieces().toArray()[j]);
				} else { // Not the final row, so there may exist a solution
					generateValidSolns(board, pieces, currRow+1);
				}
				decrementPieces();
			}
			board.removePieceAt(i, currRow);
		} // End backtracking method to find solutions
	}
}
