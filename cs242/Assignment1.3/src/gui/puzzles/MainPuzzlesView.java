/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.puzzles;

import games.Piece;
import gui.MenuItems;
import gui.View;
import gui.puzzles.ChessBoardController;
import gui.puzzles.PuzzlesController;
import gui.puzzles.PuzzlesModel;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JTextPane;
import javax.swing.UIManager;


/**
 * Class for the main puzzles view
 * 
 * Modification of code from:
 * https://wiki.engr.illinois.edu/display/cs242sp12/Assignment+1.2
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainPuzzlesView {
	private PuzzlesModel model;
	private PuzzlesController controller;
	private JTextPane leftContent;
	private JPanel rightContent;
	public static BufferedImage queenImg;
	public static BufferedImage rookImg;
	public static BufferedImage knightImg;
	public static BufferedImage customImg;

	public static final int MAX_SOLNS_DISPLAY = 250;
	
	/**
	 * Constructor
	 * 
	 * @param x		X Dimension for view
	 * @param y		Y Dimension for view
	 */
	public MainPuzzlesView(int x, int y) {
		this.model = new PuzzlesModel(this);
		this.controller = new PuzzlesController(this.model, this);
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch(Exception e) {
			// Don't worry about it
		}
		
		JFrame window = new JFrame("Chess Puzzle Solver");
		window.setSize(x, y);
		window.setLayout(new BorderLayout());
		
		JSplitPane panel  = initMainPane(150, x, y);
		
		setupMenu(window);
		
		window.setContentPane(panel);
		window.setVisible(true);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// Buffer images
		try {
			MainPuzzlesView.queenImg  = ImageIO.read(new File("images/queen.gif"));
			MainPuzzlesView.rookImg   = ImageIO.read(new File("images/rook.gif"));
			MainPuzzlesView.knightImg = ImageIO.read(new File("images/knight.gif"));
			MainPuzzlesView.customImg = ImageIO.read(new File("images/king.gif"));
		} catch(IOException e) {
			// Silently ignore
		}
	}
	
	/**
	 * Create the main pane
	 * 
	 * @param div	The position of the divider
	 * @param dimx	The x dimension of the window
	 * @param dimy	The y dimension of the window
	 * @return	The pane which was initialized
	 */
	private JSplitPane initMainPane(int div, int dimx, int dimy) {
		leftContent  = new JTextPane();
		rightContent = new JPanel();
		JScrollPane leftPane  = new JScrollPane(leftContent);
		JScrollPane rightPane = new JScrollPane(rightContent);
		Dimension leftDim     = new Dimension(div, dimy);
		Dimension rightDim    = new Dimension(dimx-div, dimy);
		JSplitPane pane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
				leftPane,
				rightPane);
		
		leftPane.setMinimumSize(leftDim);
		leftPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		leftPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		
		leftContent.setEditable(false);
		leftContent.setText("Please use the \"Solver\" menu at the top to begin!");

		leftContent.setPreferredSize(leftDim);
		
		rightPane.setMinimumSize(rightDim);
		rightPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		rightPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		
		
		pane.setOneTouchExpandable(true);
		
		return pane;
	}
	
	/**
	 * Add menu to a given window
	 * 
	 * @param window	Window to add menu to
	 */
	private void setupMenu(JFrame window) {
		JMenuBar menuBar = new JMenuBar();
		
		// Menus
		JMenu file  = new JMenu("File");
		JMenu solve = new JMenu("Solvers");
		JMenu help  = new JMenu("Help");
		
		// Menu items
		JMenuItem exit = new JMenuItem(MenuItems.FILE_EXIT);
		
		JMenuItem nqueens  = new JMenuItem(MenuItems.SOLVERS_NQUEENS);
		JMenuItem nrooks   = new JMenuItem(MenuItems.SOLVERS_NROOKS);
		JMenuItem qksolver = new JMenuItem(MenuItems.SOLVERS_MQKNIGHTS);
		JMenuItem customp  = new JMenuItem(MenuItems.SOLVERS_MCUSTOM);
		
		JMenuItem helpit = new JMenuItem(MenuItems.HELP_HELP);
		JMenuItem about  = new JMenuItem(MenuItems.HELP_ABOUT);
		
		// Add menu items to menus
		exit.addActionListener(controller);
		nqueens.addActionListener(controller);
		nrooks.addActionListener(controller);
		qksolver.addActionListener(controller);
		customp.addActionListener(controller);
		helpit.addActionListener(controller);
		about.addActionListener(controller);
		
		file.add(exit);
		
		solve.add(nqueens);
		solve.add(nrooks);
		solve.add(qksolver);
		solve.add(customp);
		
		help.add(helpit);
		help.add(about);
		
		// Add menus to menu bar
		menuBar.add(file);
		menuBar.add(solve);
		menuBar.add(help);
		
		window.setJMenuBar(menuBar);
	}
	
	/**
	 * Draw chess board solutions - will only display up to MAX_SOLNS_DISPLAY
	 * solutions (for reasonable load times, etc.)
	 * 
	 * @param numSolns	The number of solutions to draw
	 * @param dim		The dimension of the boards
	 * @param solns		The solution list
	 */
	public void drawChessSolutions(int numSolns, int dim, ArrayList<Piece> solns) {
		assert(rightContent != null);
		
		rightContent.removeAll();
		rightContent.setLayout(new BoxLayout(rightContent, BoxLayout.Y_AXIS));
		
		leftContent.setText("Found "+numSolns+" solutions. " +
		((numSolns > MAX_SOLNS_DISPLAY) ? "Only displaying "+MAX_SOLNS_DISPLAY+" results..."
				: "Displaying all results..."));
		
		if(numSolns > MAX_SOLNS_DISPLAY)
			numSolns = MAX_SOLNS_DISPLAY;
		
		for(int i = 0 ; i < numSolns ; ++i) {
			JPanel board = new JPanel();
			JSeparator separator = new JSeparator(JSeparator.VERTICAL);
			
			int start = i*dim;
			int end   = dim+(i*dim);
			
			List<Piece> pieces = solns.subList(start, end);
			
			rightContent.add(board);
			rightContent.add(separator);		
			separator.setSize(new Dimension(5,5));
			
			drawChessBoard(board, dim);

			View.addPieces(this.model,
					new ChessBoardController(board, pieces, this.model),
					new ChessSquareController(board, null),
					board,
					pieces,
					dim);
		}
	}
	
	/**
	 * Draw the chess board
	 * 
	 * @param board	The pane to create the board in
	 * @param dim	The n x n dimension of the chessboard
	 */
	private void drawChessBoard(JPanel board, int dim) {
		if(board == null)
			return;
		
		board.setLayout(new GridLayout(dim, dim));
		
		for(int i = 0 ; i < dim ; ++i) {
			for(int j = 0 ; j < dim ; ++j) {
				JPanel square = new JPanel();
				// Magic numbers = min. square dimensions to make board look nice
				square.setPreferredSize(new Dimension(65, 65));
				square.setBackground(View.chooseColor(i+j));
				board.add(square);
			}
		}
		board.updateUI(); // Flush changes out
	}
	
	/**
	 * Get user input
	 * 
	 * @param msg	Input message
	 */
	public String requestInput(String msg) {
		return JOptionPane.showInputDialog(msg, "");
	}
	
	/**
	 * Send notification pop-up
	 * 
	 * @param msg	Message to show user
	 */
	public void sendNotification(String msg) {
		JOptionPane.showMessageDialog(null, msg);
	}
	
	/**
	 * Update status text
	 * 
	 * @param msg	New status text
	 */
	public void updateStatus(String msg) {
		leftContent.setText(msg);
	}
}
