/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import java.awt.BorderLayout;
import java.awt.Dimension;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.UIManager;


/**
 * Class for the main application view
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainGuiView {
	private MainGuiController controller;
	
	/**
	 * Constructor
	 * 
	 * @param x		X Dimension for view
	 * @param y		Y Dimension for view
	 */
	public MainGuiView(int x, int y) {
		this.controller = new MainGuiController(this);
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch(Exception e) {
			// Don't worry about it
		}
		
		JFrame window = new JFrame("Game Selector");
		window.setSize(x, y);
		window.setLayout(new BorderLayout());
		
		JPanel panel  = initMainPane(x, y);
		
		setupMenu(window);
		initButtons(panel);
		
		window.setContentPane(panel);
		window.setVisible(true);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}
	
	/**
	 * Create the main pane
	 * 
	 * @param x		The x dimension of the window
	 * @param y		The y dimension of the window
	 * @return	The pane which was initialized
	 */
	private JPanel initMainPane(int x, int y) {
		JPanel pane = new JPanel();
		pane.setPreferredSize(new Dimension(x, y));
		pane.setLayout(new BorderLayout());
		return pane;
	}
	
	/**
	 * Initialize buttons
	 * 
	 * @param panel		The panel to add buttons to
	 */
	private void initButtons(JPanel panel) {
		assert(this.controller != null && panel != null);
		
		JButton feudal  = new JButton("Feudal (Game)");
		JButton puzzles = new JButton("Chess Puzzles");
		
		panel.add(feudal, BorderLayout.NORTH);
		panel.add(puzzles, BorderLayout.SOUTH);
		
		feudal.addActionListener(this.controller);
		puzzles.addActionListener(this.controller);
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
		JMenu help  = new JMenu("Help");
		
		// Menu items
		JMenuItem exit = new JMenuItem("Exit");
		
		JMenuItem helpit = new JMenuItem("Help");
		JMenuItem about  = new JMenuItem("About");
		
		// Add menu items to menus
		exit.addActionListener(controller);
		helpit.addActionListener(this.controller);
		about.addActionListener(this.controller);
		
		file.add(exit);
		
		help.add(helpit);
		help.add(about);
		
		// Add menus to menu bar
		menuBar.add(file);
		menuBar.add(help);
		
		window.setJMenuBar(menuBar);
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
}
