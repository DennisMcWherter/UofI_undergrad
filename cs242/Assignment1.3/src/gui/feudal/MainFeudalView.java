/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal;

import games.feudal.FeudalBoard;
import gui.MenuItems;
import gui.View;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.UIManager;

/**
 * The main view for the Feudal game
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainFeudalView {
	private FeudalModel model;
	private FeudalController controller;
	private JPanel board;
	private JPanel main;
	private JFrame window;
	private JButton start;
	private JButton finish;
	
	public static BufferedImage KING_IMG;
	public static BufferedImage PRINCE_IMG;
	public static BufferedImage DUKE_IMG;
	public static BufferedImage KNIGHT_IMG;
	public static BufferedImage SQUIRE_IMG;
	public static BufferedImage SERG_IMG;
	public static BufferedImage PIKE_IMG;
	public static BufferedImage ARCHER_IMG;
	
	/**
	 * Constructor
	 * 
	 * @param x		x dimension of window
	 * @param y		y dimension of window
	 */
	public MainFeudalView(int x, int y) {
		this.model = new FeudalModel(this);
		this.controller = new FeudalController(this.model, this);
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch(Exception e) {
			// Don't worry about it
		}
		
		this.window = new JFrame("Feudal (Game)");
		this.window.setLayout(new BorderLayout());
		this.window.setSize(x, y);
		
		this.board = initPanel(x, y);
		
		setupMenu(window);
		
		this.window.setVisible(true);
		this.window.setContentPane(this.board);
		this.window.pack();
		this.window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		this.finish = new JButton(MenuItems.FEUDAL_BUTTON_COMP);
		this.finish.addActionListener(this.controller);
		this.finish.setEnabled(false);
		
		this.start = new JButton(MenuItems.FEUDAL_GAME_START);
		this.start.addActionListener(this.controller);
		this.start.setEnabled(false);
		
		try {
			KING_IMG   = ImageIO.read(new File("images/feudal_king.png"));
			PRINCE_IMG = ImageIO.read(new File("images/feudal_prince.png"));
			DUKE_IMG   = ImageIO.read(new File("images/feudal_duke.png"));
			KNIGHT_IMG = ImageIO.read(new File("images/feudal_knight.png"));
			SQUIRE_IMG = ImageIO.read(new File("images/feudal_squire.png"));
			SERG_IMG   = ImageIO.read(new File("images/feudal_sergeant.png"));
			ARCHER_IMG = ImageIO.read(new File("images/feudal_archer.png"));
			PIKE_IMG   = ImageIO.read(new File("images/feudal_pikeman.png"));
		} catch(IOException e) {
			// Ignore
		}
	}
	
	/**
	 * Initialize main jpanel
	 * 
	 * @param x	 X dimension
	 * @param y	 Y dimension
	 * @return	A jpanel object setup to be main 
	 */
	private JPanel initPanel(int x, int y) {
		JPanel panel = new JPanel();
		
		panel.setPreferredSize(new Dimension(x, y));
		
		return panel;
	}
	
	/**
	 * Setup menubar
	 * 
	 * @param window	The window to add menu to
	 */
	private void setupMenu(JFrame window) {
		JMenuBar menuBar = new JMenuBar();
		
		// Main menus
		JMenu file = new JMenu("File");
		JMenu help = new JMenu("Help");
		
		// Menu items
		JMenuItem exit   = new JMenuItem(MenuItems.FILE_EXIT);
		JMenuItem load   = new JMenuItem(MenuItems.FEUDAL_FILE_LOAD);
		JMenuItem helpit = new JMenuItem(MenuItems.HELP_HELP);
		
		// Add items respectively
		menuBar.add(file);
		menuBar.add(help);
		
		file.add(load);
		file.add(exit);
		help.add(helpit);
		
		// Add event handling
		load.addActionListener(this.controller);
		exit.addActionListener(this.controller);
		helpit.addActionListener(this.controller);
		
		window.setJMenuBar(menuBar);
	}
	
	/**
	 * Re-enable buttons and menus
	 */
	public void enableGUI() {
		this.start.setEnabled(true);
	}
	
	/**
	 * Prepare next move
	 * 
	 * @param indices	The indices where current player's pieces are
	 */
	public void setupNextMove(List<Integer> indices) {
		this.start.setEnabled(false);
		this.finish.setEnabled(true);
		for(int i = 0 ; i < FeudalBoard.BOARD_SIZE*FeudalBoard.BOARD_SIZE ; ++i) {
			JPanel square = (JPanel)this.main.getComponent(i);
			if(indices.contains(i))
				square.addMouseListener(new PieceController(this.model, true));
			else
				square.addMouseListener(new PieceController(this.model, false));
		}
	}
	
	/**
	 * Open a map file
	 * 
	 * @return	The particular map file to be parsed. Else null
	 */
	public File openFile() {
		final JFileChooser fc = new JFileChooser();
		if(fc.showOpenDialog(board) == JFileChooser.APPROVE_OPTION)
			return fc.getSelectedFile();
		return null;
	}
	
	/**
	 * Request user input
	 * 
	 * @param msg	The input to be asked for
	 * @param hint	A hint to the user
	 * @return	The value
	 */
	public String requestInput(String msg, String hint) {
		return JOptionPane.showInputDialog(msg, hint);
	}
	
	/**
	 * Send a notification to the user
	 * 
	 * @param msg	The message to send to the user
	 */
	public void sendNotification(String msg) {
		JOptionPane.showMessageDialog(null, msg);
	}
	
	// TODO: Draw board method which takes arguments
	//  such that it can be read from a file
	public void drawMap() {
		this.board.removeAll();
		
		List<Integer>[] terrain = this.model.getTerrain();
		int[][] castles = this.model.getCastles();
		
		this.main = new JPanel();
		
		this.board.add(this.main);
		this.board.add(this.start, BorderLayout.NORTH);
		this.board.add(this.finish, BorderLayout.SOUTH);
		
		main.setPreferredSize(this.board.getPreferredSize());
		main.setLayout(new GridLayout(FeudalBoard.BOARD_SIZE, FeudalBoard.BOARD_SIZE));
		
		for(int i = 0 ; i < FeudalBoard.BOARD_SIZE*FeudalBoard.BOARD_SIZE ; ++i) {
			JPanel square = new JPanel();
			
			square.setBorder(BorderFactory.createLineBorder(Color.DARK_GRAY));
			
			if(terrain[0].contains(i))
				square.setBackground(chooseColor(1));
			else if(terrain[1].contains(i))
				square.setBackground(chooseColor(2));
			else
				square.setBackground(chooseColor(0));
			
			for(int[] c : castles) {
				if(c[0] == i)
					square.setBackground(chooseColor(3));
				else if(c[1] == i || c[2] == i)
					square.setBackground(chooseColor(4));
			}
			
			main.add(square);
		}
		
		View.addPieces(this.model,
				null, null,
				main, this.model.getPieces(),
				FeudalBoard.BOARD_SIZE);
		
		main.updateUI();
	}
	
	/**
	 * Initialize game board such that pieces can be placed (initially)
	 * 
	 * @param half		The half (1-2 i.e. top or bottom) which is available
	 */
	public void initialPlace(int half) {
		assert(this.main != null);
		
		start.setEnabled(false);
		
		int xStart = 0;
		int xEnd   = FeudalBoard.BOARD_SIZE;
		int yStart = 0;
		int yEnd   = FeudalBoard.BOARD_SIZE/2;
		
		if(half == 2) {
			yStart = FeudalBoard.BOARD_SIZE/2;
			yEnd   = FeudalBoard.BOARD_SIZE;
		}
		
		for(int i = xStart ; i < xEnd ; ++i) {
			for(int j = yStart ; j < yEnd ; ++j) {
				int idx = (j*24)+i;
				JPanel square = (JPanel)this.main.getComponent(idx);
				square.addMouseListener(new PlacePieceController(this.model));
			}
		}
	}
	
	/**
	 * Choose a color for a square
	 * 
	 * @param type	0 = normal, 1 = rough terrain, 2 = mountain, 3 = castle,
	 * 				4 = castle green
	 * @return	The appropriate color for the type
	 */
	private Color chooseColor(int type) {
		if(type == 1)
			return new Color(0, 100, 0);
		if(type == 2)
			return new Color(135, 206, 250);
		if(type == 3)
			return Color.RED;
		if(type == 4)
			return Color.CYAN;
		return new Color(143, 188, 143);
	}
}
