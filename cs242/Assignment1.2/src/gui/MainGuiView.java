/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
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

import puzzle.Piece;

/**
 * Class for the main application view
 * 
 * Modification of code from:
 * https://wiki.engr.illinois.edu/display/cs242sp12/Assignment+1.2
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainGuiView {
	private PuzzlesModel model;
	private PuzzlesController controller;
	private JTextPane leftContent;
	private JPanel rightContent;
	private static BufferedImage queenImg;
	private static BufferedImage rookImg;
	private static BufferedImage knightImg;
	private static BufferedImage customImg;
	
	public static final int MAX_SOLNS_DISPLAY = 250;
	
	/**
	 * Constructor
	 * 
	 * @param x		X Dimension for view
	 * @param y		Y Dimension for view
	 */
	public MainGuiView(int x, int y) {
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
			MainGuiView.queenImg  = ImageIO.read(new File("images/queen.gif"));
			MainGuiView.rookImg   = ImageIO.read(new File("images/rook.gif"));
			MainGuiView.knightImg = ImageIO.read(new File("images/knight.gif"));
			MainGuiView.customImg = ImageIO.read(new File("images/king.gif"));
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
		JMenuItem exit = new JMenuItem("Exit");
		
		JMenuItem nqueens  = new JMenuItem("N Queens");
		JMenuItem nrooks   = new JMenuItem("N Rooks");
		JMenuItem qksolver = new JMenuItem("M Queens and Knights");
		JMenuItem customp  = new JMenuItem("M Custom Pieces");
		
		JMenuItem helpit = new JMenuItem("Help");
		JMenuItem about  = new JMenuItem("About");
		
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

			addPieces(board, pieces, dim);
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
				square.setBackground(chooseColor(i+j));
				board.add(square);
			}
		}
		board.updateUI(); // Flush changes out
	}
	
	/**
	 * Choose a color for the board square
	 * 
	 * @param x		Linear number (position on board)
	 * @return	The appropriate color for the square
	 */
	private static Color chooseColor(int x) {
		if(x % 2 == 0)
			return new Color(205, 205, 193);
		return new Color(0, 100, 0);
	}
	
	/**
	 * Add list of pieces to chessboard
	 * 
	 * @param board		The board to add pieces to
	 * @param pieces	The pieces to add
	 * @param dim		The board dimension
	 */
	private void addPieces(JPanel board, List<Piece> pieces, int dim) {
		assert(this.model != null && board != null && pieces != null);
		
		// Make sure we have a listener on this board to animate it
		for(MouseListener x : board.getMouseListeners())
			board.removeMouseListener(x);
		board.addMouseListener(new ChessBoardController(board, pieces, this.model));
		board.updateUI();

		for(int i = 0 ; i < pieces.size() ; ++i) {
			Piece piece = pieces.get(i);
			int square = PuzzlesModel.convertPositionToIdx(piece);
			addSinglePiece(board, square, piece);
		}
	}
	
	/**
	 * Add a particular piece to the board
	 * 
	 * @param board		The object which is the board
	 * @param where		Which square to place on
	 * @param piece		The piece to place on the board
	 */
	public static void addSinglePiece(JPanel board, int where, Piece piece) {
		assert(piece != null && board != null);
		
		BufferedImage image = null;
		String symbol = piece.getSymbol();
		
		if("Q".equalsIgnoreCase(symbol))
			image = MainGuiView.queenImg;
		else if("R".equalsIgnoreCase(symbol))
			image = MainGuiView.rookImg;
		else if("N".equalsIgnoreCase(symbol))
			image = MainGuiView.knightImg;
		else if("C".equalsIgnoreCase(symbol))
			image = MainGuiView.customImg;
		
		assert(image != null);
		
		JPanel square = (JPanel)board.getComponent(where);
		
		square.add(new JLabel(new ImageIcon(image)));
		
		// Paranoia
		for(MouseListener x : square.getMouseListeners())
			square.removeMouseListener(x);
		
		square.addMouseListener(new ChessSquareController(board, piece));
		square.updateUI();
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
		square.setBackground(MainGuiView.chooseColor(coords[0]+coords[1]));
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
	
	/**
	 * Get user input
	 * 
	 * @param msg	Input message
	 */
	public String requestInput(String msg) {
		return JOptionPane.showInputDialog("", msg);
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
