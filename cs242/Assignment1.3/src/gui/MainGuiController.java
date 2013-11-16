/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

import gui.feudal.MainFeudalView;
import gui.puzzles.MainPuzzlesView;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * Controller class for the main GUI options
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class MainGuiController implements ActionListener {
	private MainGuiView view;
	private MainPuzzlesView puzzles;
	private MainFeudalView feudal;
	
	/**
	 * Constructor
	 * 
	 * @param view		The parent view on which the controller works
	 */
	public MainGuiController(MainGuiView view) {
		this.view 	 = view;
		this.puzzles = null;
		this.feudal  = null;
	}

	/**
	 * Catches user events
	 * 
	 * @param arg0		The event caught
	 */
	@Override
	public void actionPerformed(ActionEvent arg0) {
		String act = arg0.getActionCommand();
		
		if("Chess Puzzles".equalsIgnoreCase(act)) {
			if(!isPlayingChessPuzzles())
				this.puzzles = new MainPuzzlesView(800, 600);
			else
				this.view.sendNotification("Already playing Chess Puzzles!");
		} else if("Feudal (Game)".equalsIgnoreCase(act)) {
			if(!isPlayingFeudal())
				this.feudal = new MainFeudalView(1000, 740);
			else
				this.view.sendNotification("Already playing Feudal!");
		} else if("Help".equalsIgnoreCase(act)) {
			displayHelp();
		} else if("About".equalsIgnoreCase(act)) {
			displayAbout();
		} else if("Exit".equalsIgnoreCase(act)) {
			System.exit(0);
		}
	}
	
	/**
	 * Check if playing feudal
	 * 
	 * @return	Returns true if playing, false otherwise
	 */
	public boolean isPlayingFeudal() {
		return this.feudal != null;
	}
	
	/**
	 * Check if playing Chess Puzzles
	 * 
	 * @return	Returns true if playing, false otherwise
	 */
	public boolean isPlayingChessPuzzles() {
		return this.puzzles != null;
	}
	
	/**
	 * Display the help message
	 */
	private void displayHelp() {
		assert(this.view != null);
		this.view.sendNotification("Click a button to start the activity you wish "
				+"to play");
	}
	
	/**
	 * Display the about message
	 */
	 private void displayAbout() {
		 assert(this.view != null);
		 this.view.sendNotification("This application was created by Dennis J. McWherter, Jr.\n"
		 		+"for a CS242 assignment at the University of Illinois at Urbana-Champaign");
	 }
}
