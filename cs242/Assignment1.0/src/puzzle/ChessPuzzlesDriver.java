/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle;

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
		NQueensSolver solver = null;
		ArrayList<Piece> solns = new ArrayList<Piece>();
		int numSolns = 0;
		int dimension = 0;
		
		// Determine dimension
		if(args.length == 1) { // Parse command line
			dimension = Integer.parseInt(args[0]);
			System.out.println("Read input from command line\n\tGenerating solutions for "
					+dimension+" x "+dimension+" game board...\n");
		} else { // Otherwise ask for input
			Scanner scan = new Scanner(System.in);
			System.out.print("Please enter an integer n to solve for (n x n game board): ");
			dimension = scan.nextInt();
		}
		
		board  = new Board(dimension);
		solver = new NQueensSolver(board); 
		
		numSolns = solver.solve(solns);
		
		if(numSolns == 0)
			System.out.println("No solutions exist for boards of size 2 x 2 or 3 x 3");
		
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
