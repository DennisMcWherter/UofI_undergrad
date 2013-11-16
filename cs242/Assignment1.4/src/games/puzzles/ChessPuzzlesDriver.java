/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.puzzles;

import games.Board;
import games.Piece;

import java.util.ArrayList;
import java.util.Scanner;

/**
 * Program entry point
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class ChessPuzzlesDriver {
	/**
	 * Program entry point
	 * 
	 * @param args	Program input arguments
	 */
	public static void main(String[] args) {
		Board board = null;
		PuzzleSolver solver = null;
		ArrayList<Piece> solns = new ArrayList<Piece>();
		int numSolns = 0;
		int dimension = 0;
		String gameType = null;
		
		// Determine dimension
		Scanner scan = new Scanner(System.in);
		System.out.print("Please enter which puzzle you wish to solve (q = N queens, r = 8 rooks, k = queens and knights, c = custom): ");
		gameType = scan.next();
		
		// Determine which solver to use
		if(gameType.equals("r")) {
			dimension = 8;
			board  = new Board(dimension);
			solver = new RooksSolver(board);
		} else if(gameType.equals("k") || gameType.equals("c")) {
			System.out.print("Please enter an integer n to solve for (n x n game board): ");
			dimension = scan.nextInt();
			board = new Board(dimension);
			
			if(gameType.equals("k"))
				solver = new QueensKnightsSolver(board);
			else
				solver = new NCustomPieceSolver(board);
			
			int m = solver.solve(solns);
			
			System.out.println("The highest value m for an "+dimension+"x"+dimension
					+" board is: "+m);
			
			if(gameType.equals("k"))
				return;
		} else {
			System.out.print("Please enter an integer n to solve for (n x n game board): ");
			dimension = scan.nextInt();
			board = new Board(dimension);
			solver = new NQueensSolver(board);
		}
		
		if(!gameType.equals("c"))
			numSolns = solver.solve(solns);
		else
			numSolns = solns.size()/dimension;
		
		if(numSolns == 0)
			System.out.println("No solutions found (or only blank board)\n");
		
		for(int i = 0 ; i < numSolns ; i++) {
			System.out.println("\nSolution "+(i+1)+"\n");
			for(int j = dimension*i ; j < dimension+(dimension*i) ; ++j) {
				Piece p = solns.get(j);
				board.placePiece(p.getX(), p.getY(), p);
			}
			board.print();
			board.reset();
		}
	}
}
