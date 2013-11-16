/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

import java.util.List;

/**
 * Solver for Queens and Knights problem
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class QueensKnightsSolver extends PuzzleSolver {
	private int NumQueens;
	private int NumKnights;
	private int BestQueens;
	private int BestKnights;
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 */
	public QueensKnightsSolver(Board b) {
		super(b);
		NumQueens = 0;
		NumKnights = 0;
		BestQueens = 0;
		BestKnights = 0;
	}
	
	/**
	 * Solver for the N Queens and knights problem
	 * 
	 * @param pieces	The list of pieces for the solver (UNUSED)
	 * @return	Returns the largest number M for the problem. Returns -1
	 * 			on error
	 */
	@Override
	public int solve(List<Piece> pieces) {
		Board board = this.getGameBoard();
		
		if(board == null)
			return -1;
		
		findResult(board);
		
		return bestM();
	}
	
	/**
	 * Chooses the best M based on NumQueens and NumKnights
	 * (i.e. lowest value of the two if not equal)
	 * 
	 * @return	Lowest value between NumQueens and NumKnights
	 */
	private int bestM() {
		return (BestKnights < BestQueens) ? BestKnights : BestQueens;
	}
	
	/**
	 * Finds the best possible M to place M queens and knights on an N x N board
	 * 
	 * @param board	The game board to work with
	 */
	private void findResult(Board board) {
		if(board == null)
			return;
		
		// Need to manually manage which pieces are played
		for(int i = 0 ; i < board.getDimension() ; ++i) {
			board.placePiece(i, 0, new Queen());
			generateValidSolns(board, 1, true);
			board.reset();
					
			board.placePiece(i, 0, new Knight());
			generateValidSolns(board, 1, false);
			board.reset();
		}
	}
	
	/**
	 * Increments the count of a particular piece
	 * 
	 * @param playKnight	Choose whether to increment knight or queen
	 */
	private void incrementPieceCount(boolean playKnight) {
		if(playKnight) {
			NumKnights++;
			BestKnights = (NumKnights > BestKnights) ? NumKnights : BestKnights;
		} else {
			NumQueens++;
			BestQueens = (NumQueens > BestQueens) ? NumQueens : BestQueens;
		}
	}
	
	/**
	 * Decrements the count of a particular piece
	 * 
	 * @param playKnight	Choose whether to decrement knight or queen
	 */
	private void decrementPieceCount(boolean playKnight) {
		if(playKnight)
			NumKnights--;
		else
			NumQueens--;
	}
	
	/**
	 * Checks next row for possible valid solutions
	 * 
	 * @param board		The game board which is used for testing
	 * @param currRow	The current row to be scanned
	 * @param pieceType	Play a knight. If false, play a queen
	 * @return	Largest number m knights and queens which can be placed on the board.
	 * 			Returns -1 on error
	 */
	private void generateValidSolns(Board board, int currRow, boolean playKnight) {
		if(board == null)
			return;
		
		final int boardDim = board.getDimension();
		
		// Check every spot in a row
		for(int i = 0 ; i < boardDim ; ++i) {
			board.placePiece(i, currRow, (playKnight) ? new Knight() : new Queen());
			if(board.isValid()) {
				incrementPieceCount(playKnight);
				if(currRow == boardDim-1) { // Final row, so this is a valid config
					decrementPieceCount(playKnight);
					return;
				} else { // Not the final row, so there may exist a solution
					generateValidSolns(board, currRow+1, !playKnight);
				}
				decrementPieceCount(playKnight);
			}
			board.removePieceAt(i, currRow);
		} // End backtracking method to find solutions
	}
}
