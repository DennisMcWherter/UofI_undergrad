/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games;

import games.puzzles.PuzzleSolver;

import java.util.ArrayList;
import java.util.List;


/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public abstract class Backtracker extends PuzzleSolver {
	private Piece[] gamePieces;
	private int numSolutions; // Num of total solutions
	private int[] numPieces; // Num pieces counted currently
	private int[] m; // Maximum m
	private boolean findM; // Whether to find maximum M
	
	/**
	 * Constructor
	 * 
	 * @param b		Game board to solve on
	 * @param p		Game Piece types to use
	 * @param m		Determine whether we should look for m or not
	 */
	public Backtracker(Board b, Piece[] p, boolean m) {
		super(b);
		this.gamePieces   = p;
		this.numSolutions = 0;
		this.numPieces	  = new int[p.length];
		this.m			  = new int[p.length];
		this.findM 		  = m;
	}
	
	/**
	 * Backtrack method to solve puzzles
	 * 
	 * @param pieces	The list of valid solutions
	 * @return	Highest M or number of total solutions found depending
	 * 	on the value of this.findM set in the constructor
	 */
	protected int backtrack(List<Piece> pieces) {
		Board board = this.getGameBoard();
		
		if(board == null || pieces == null)
			return -1;
		
		// Special case: dim = 1
		if(board.getDimension() == 1) {
			if(this.findM && this.gamePieces.length > 1)
				return 0;
			board.placePiece(0, 0, this.gamePieces[0].getNewPiece());
			saveConfiguration(pieces, board);
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
			int currIdx = 0;
			for(Piece p : this.gamePieces) {
				board.placePiece(i, 0, p.getNewPiece());
				incrementPieces(currIdx);
				
				generateValidSolns(board, pieces, 1, currIdx+1);
				
				decrementPieces(currIdx);
				board.reset();
				
				currIdx++;
			}
		}
		
		return (this.findM) ? bestM() : this.numSolutions;
	}
	
	/**
	 * Returns the lowest M in the array of max M's. This number is the
	 * minimum possible M for which M of each piece can fit on an N x N board
	 * 
	 * @return The lowest M in the array
	 */
	private int bestM() {
		int retval = -1;
		for(int x : this.m)
			retval = (retval < 0) ? x : (x < retval) ? x : retval;
		return retval;
	}
	
	/**
	 * Increment the number of pieces on the board
	 */
	private void incrementPieces(int idx) {
		this.m[idx] = ((++this.numPieces[idx]) > this.m[idx]) ? this.numPieces[idx] : this.m[idx];
	}
	
	/**
	 * Decrement the number of pieces on the board
	 */
	private void decrementPieces(int idx) {
		this.numPieces[idx]--;
	}
	
	/**
	 * Count pieces of all types of this solution set for this instance
	 * 
	 * @param solns		The solution set to count piece types of
	 * @return	An array of integers (indices corresponding to this.gamePieces)
	 * 		which gives the number of piece types in this solution set
	 */
	private int[] countPieceTypes(List<Piece> solns) {
		int pieceCount[] = new int[this.gamePieces.length];
		
		for(Piece p : solns) {
			int t = 0;
			for(Piece c : this.gamePieces) {
				if(c.getClass() == p.getClass()) {
					pieceCount[t]++;
					break;
				}
				t++;
			}
		}
		
		return pieceCount;
	}
	
	/**
	 * Determine whether or not the number of pieces match as expected
	 * for this type of solution (i.e. M of each piece)
	 * 
	 * @param solns		The solution list to be checked
	 * @param dim		The dimension of the board
	 * @return	Returns true if this is a valid solution set, otherwise false.
	 */
	private boolean isValidMSoln(List<Piece> solns, int dim) {
		if(solns.size() % 2 != 0)
			return false;
		
		if(dim % 2 != 0) // Last piece does not matter
			solns.remove(solns.get(solns.size()-1));
		
		int pieceCount[] = countPieceTypes(solns);
		int rootVal = pieceCount[0];
		
		for(int i = 1 ; i < pieceCount.length ; ++i) {
			if(rootVal != pieceCount[i])
				return false;
		}
		
		return true;
	}
	
	/**
	 * Saves the current configuration of the board by adding the pieces
	 * to the current solutions list
	 * 
	 * @param solns		Valid solutions list
	 * @param board		The valid board to take pieces from
	 */
	private void saveConfiguration(List<Piece> solns, Board board) {
		if(board == null || solns == null)
			return;
		
		ArrayList<Piece> testList = new ArrayList<Piece>();
		
		for(Piece p : board.getPieces())
			testList.add(p);
		
		if(this.findM) {
			if(solns.size() % 2 != 0)
				return;
			if(!isValidMSoln(testList, board.getDimension()))
				return;
		}
		
		this.numSolutions++;
		
		solns.addAll(testList);
	}

	/**
	 * Checks next row for possible valid solutions
	 * 
	 * @param board		The game board which is used for testing
	 * @param pieces	The list of pieces for valid solutions
	 * @param currRow	The current row to be scanned
	 * @param idxOffset	The offset for the next piece index
	 */
	private void generateValidSolns(Board board, List<Piece> pieces, int currRow, int idxOffset) {
		if(board == null)
			return;
		
		final int boardDim = board.getDimension();
		final int pieceIdx = idxOffset % this.gamePieces.length;
		final Piece currentPiece = this.gamePieces[pieceIdx];
		
		// Check every spot in a row
		for(int i = 0 ; i < boardDim ; ++i) {
			board.placePiece(i, currRow, currentPiece.getNewPiece());
			if(board.isValid()) {
				incrementPieces(pieceIdx);
				if(currRow == boardDim-1) // Final row
					saveConfiguration(pieces, board);
				else
					generateValidSolns(board, pieces, currRow+1, idxOffset+1);
				decrementPieces(pieceIdx);
			}
			board.removePieceAt(i, currRow);
		} // End backtracking method to find solutions
	}
	
	/**
	 * Backtrack all moves made to find a particular solution path
	 * 
	 * @param soln	The solution to recreate
	 * @return	List of all attempted pieces (in order) to backtrack the
	 * 	particular solution. Return null if error or no paths found
	 */
	public List<Piece> backtrackSoln(List<Piece> soln) {
		Piece p = null;
		if(soln.size() < 0 || (p = soln.get(0)) == null
				|| p.getParentBoard() == null)
			return null;
		
		Board board = new Board(p.getParentBoard().getDimension());
		ArrayList<Piece> thePath = new ArrayList<Piece>();
		Piece start = null;
		
		for(Piece tmp : soln) {
			if(tmp.getY() == 0) {
				start = tmp;
				break;
			}
		}
		
		if(start == null)
			return null;
		
		board.placePiece(start.getX(), start.getY(), start);
		thePath.add(start);
		
		if(!traceRoutes(board, soln, thePath, 1, 1))
			return null;

		return thePath;
	}
	
	private boolean traceRoutes(Board board, List<Piece> soln, List<Piece> path, int currRow, int idxOffset) {
		if(board == null)
			return false;
		
		final int boardDim = board.getDimension();
		final int pieceIdx = idxOffset % this.gamePieces.length;
		final Piece currentPiece = this.gamePieces[pieceIdx];
		
		// Check every spot in a row
		for(int i = 0 ; i < boardDim ; ++i) {
			Piece p = currentPiece.getNewPiece();
			board.placePiece(i, currRow, p);
			path.add(p);
			if(board.isValid()) {
				if(currRow == boardDim-1) // Final row
					return isSameSolution(soln, board);
				else
					if(traceRoutes(board, soln, path, currRow+1, idxOffset+1))
						return true;
			}
			board.removePieceAt(i, currRow);
		} // End backtracking method to find solutions
		
		return false;
	}
	
	/**
	 * Check whether or not our solution list matches the current board configuration
	 * 
	 * @param soln		The solution list to compare configuration to
	 * @param board		The board holding the current solution
	 * @return	Returns true if the configurations are the same, false otherwise
	 */
	public boolean isSameSolution(final List<Piece> soln, Board board) {
		for(Piece x : board.getPieces()) {
			boolean foundPiece = false;
			for(Piece p : soln) {
				if(p.getX() == x.getX() && p.getY() == x.getY()) {
					if(p.getClass() != x.getClass())
						return false;
					foundPiece = true;
					break;
				}
			}
			if(!foundPiece)
				return false;
		}
		
		return true;
	}
}
