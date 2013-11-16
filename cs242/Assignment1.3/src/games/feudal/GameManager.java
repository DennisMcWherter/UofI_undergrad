/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import java.util.ArrayList;
import java.util.List;

import games.Piece;

/**
 * Game manager holds/manages state information about the game
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class GameManager {
	public final static int MAX_ARMIES = 6;
	
	private List<Piece>[] pieces; // NOTE: Castles not included
	private int armies;
	private int turn;
	private int nextPiece;
	
	/**
	 * Constructor
	 * 
	 * @param armies	Number of armies in the game
	 * NOTE: Number of armies MUST be [2, 6]. If any value outside of the range
	 * 		is given, it will be normalized (i.e. anything >6 becomes 6 anything
	 * 		< 2 becomes 2)
	 */
	@SuppressWarnings("unchecked")
	public GameManager(int armies) {
		this.armies = (armies > MAX_ARMIES) ? MAX_ARMIES : (armies < 2) ? 2 : armies;
		this.turn   = 0;
		this.pieces = new ArrayList[armies];
		this.nextPiece = 0;
		
		for(int i = 0 ; i < armies ; ++i)
			this.pieces[i] = new ArrayList<Piece>();
		
		initPieceSet();
	}
	
	/**
	 * Get current piece list (based on turn)
	 * 
	 * @return	The piece list of whoever's turn it is
	 */
	public List<Piece> getPieceList() {
		return this.pieces[this.turn];
	}
	
	/**
	 * Get the current number of armies in this game
	 * 
	 * @return	Return the number of armies in this game
	 */
	public int getArmyCount() {
		return this.armies;
	}
	
	/**
	 * Whose turn is it?
	 * 
	 * @return	The id number (0 - 5) of the army which has the turn
	 * 			If returns -1, the game has not yet started or done with
	 * 			placing pieces
	 */
	public int getTurn() {
		return this.turn;
	}
	
	/**
	 * Reset turns
	 */
	public void resetTurns() {
		this.turn = 0;
	}
	
	/**
	 * Check for game over
	 * 
	 * @return	The player number of the winner. Otherwise -1
	 */
	public int checkGameOver() {
		int winner = -1;
		for(int i = 0 ; i < this.armies ; ++i) {
			boolean hasKing   = false;
			boolean hasDuke   = false;
			boolean hasPrince = false;
			for(Piece p : this.pieces[i]) {
				if(p instanceof Prince)
					hasPrince = true;
				else if(p instanceof Duke)
					hasDuke = true;
				else if(p instanceof King)
					hasKing = true;
			}
			if(!hasKing && !hasDuke && !hasPrince) {
				winner = (i+1) % this.armies;
				break;
			}
		}
		return winner;
	}
	
	/**
	 * Get the next piece (on the initial piece setting)
	 * 
	 * @return	The next piece or null if error
	 */
	public Piece getNextPiece() {
		Piece piece = null;
		
		int curr = this.nextPiece;
		int turn = this.turn;
		
		this.nextPiece++;
		
		if(this.nextPiece > (FeudalBoard.BOARD_SIZE/2)) {
			this.nextPiece = 0;
			this.turn++;
			if(this.turn >= this.armies)
				this.turn = -1;
		}
		
		piece = this.pieces[turn].get(curr);
		
		return piece;
	}
	
	/**
	 * Finish turn - this method actually updates the game
	 * if necessary
	 * 
	 * @param thePieces		The current board configuration
	 * @return	If a piece was captured, return it to remove it from board
	 * 			otherwise return null
	 */
	public Piece completeTurn(List<Piece> thePieces) {
		if(thePieces == null)
			return null;
		
		Piece removed = null;
		
		// Find overlapped pieces
		for(Piece i : thePieces) {
			i.setMoved(false);
			for(Piece j : thePieces) {
				if(i != j && i.getX() == j.getX() && i.getY() == j.getY()) {
					removed = removePiece(i, j);
					break;
				}
			}
		}
		
		this.turn = (this.turn + 1) % this.armies;
		
		return removed;
	}
	
	/**
	 * Choose a piece to be removed based on whose turn it is
	 * 
	 * @param i		One of the pieces
	 * @param j		The other overlapping piece
	 * @return	The piece to be removed, else null if error
	 */
	private Piece removePiece(Piece i, Piece j) {
		if(i == null || j == null)
			return null;
		for(int t = 0 ; t < this.armies ; ++t) {
			for(Piece p : this.pieces[t]) {
				if(p == i) {
					if(this.turn == t) {
						this.pieces[t].remove(j);
						return j;
					}
					this.pieces[t].remove(i);
					return i;
				}
			}
		}
		return null;
	}
	
	/**
	 * Initialize the piece set with the appropriate characters
	 */
	private void initPieceSet() {
		for(int i = 0 ; i < this.armies ; ++i) {
			this.pieces[i].add(new King());
			this.pieces[i].add(new Prince());
			this.pieces[i].add(new Duke());
			this.pieces[i].add(new Knight());
			this.pieces[i].add(new Knight());
			this.pieces[i].add(new Sergeant());
			this.pieces[i].add(new Sergeant());
			this.pieces[i].add(new Squire());
			this.pieces[i].add(new Archer());
			this.pieces[i].add(new Pikeman());
			this.pieces[i].add(new Pikeman());
			this.pieces[i].add(new Pikeman());
			this.pieces[i].add(new Pikeman());
		}
	}
}
