/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal;

import gui.MenuItems;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

/**
 * Basic controller for Feudal
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalController implements ActionListener {
	private FeudalModel model;
	private MainFeudalView view;
	private boolean mapLoaded;
	
	/**
	 * Construct
	 * 
	 * @param model			Model on which this view operates
	 * @param view	 		View which controller uses
	 */
	public FeudalController(FeudalModel model, MainFeudalView view) {
		this.model = model;
		this.view  = view;
		this.mapLoaded = false;
	}

	/**
	 * Catch an event which has been signaled on the GUI
	 * 
	 * @param arg0		The event which was caught
	 */
	@Override
	public void actionPerformed(ActionEvent arg0) {
		String what = arg0.getActionCommand();
		
		if(MenuItems.FEUDAL_FILE_LOAD.equalsIgnoreCase(what)) {
			File f = this.view.openFile();
			
			this.mapLoaded = initGame(f, 2); // Only 2 players supported
			
			String x1s = this.view.requestInput("What is the X coordinate of Castle 1?", "0");
			String y1s = this.view.requestInput("What is the Y coordinate of Castle 1?", "0");
			String x2s = this.view.requestInput("What is the X coordinate of Castle 2?", "23");
			String y2s = this.view.requestInput("What is the Y coordinate of Castle 2?", "23");
			
			int x1 = Integer.parseInt(x1s.trim());
			int x2 = Integer.parseInt(x2s.trim());
			int y1 = Integer.parseInt(y1s.trim());
			int y2 = Integer.parseInt(y2s.trim());
			
			this.model.setCastle(0, x1, y1);
			this.model.setCastle(1, x2, y2);
			
			this.view.initialPlace(1);
		} else if(MenuItems.FEUDAL_GAME_START.equalsIgnoreCase(what)) {
			if(!this.mapLoaded) {
				this.view.sendNotification("You must first load a map!");
				return;
			}
			this.model.setupNextMove(false);
		} else if(MenuItems.FEUDAL_BUTTON_COMP.equalsIgnoreCase(what)) {
			this.model.completeTurn();
		} else if(MenuItems.FEUDAL_BUTTON_AI.equalsIgnoreCase(what)) { 
			this.model.makeAIMove();
		} else if(MenuItems.FILE_EXIT.equalsIgnoreCase(what)) {
			System.exit(0);
		} else if(MenuItems.HELP_HELP.equalsIgnoreCase(what)) {
			displayHelp();
		} else if(MenuItems.FEUDAL_CONNECT.equalsIgnoreCase(what)) {
			String p = getProtocol();
			if(p == null) {
				this.view.sendNotification("Canceling... Could not find valid protocol file");
				return;
			}
			
			String h  = this.view.requestInput("Hostname to Connect", "localhost");
			String pS = this.view.requestInput("Port number", "5555");
			
			int port  = Integer.parseInt(pS.trim());
			
			String xS = this.view.requestInput("What is the X coordinate of Castle?", "23");
			String yS = this.view.requestInput("What is the Y coordinate of Castle?", "23");
			
			int x = Integer.parseInt(xS.trim());
			int y = Integer.parseInt(yS.trim());
			
			int[] castle = {x, y};
			
			this.model.connectToServer(h, port, p, castle);
		} else if(MenuItems.FEUDAL_HOST.equalsIgnoreCase(what)) {
			String p = getProtocol();
			if(p == null) {
				this.view.sendNotification("Canceling... Could not find valid protocol file");
				return;
			}
			this.view.sendNotification("Please load a map file...");
			File f = this.view.openFile();
			this.mapLoaded = initGame(f, 2); // Only 2 players supported
			
			String pS = this.view.requestInput("Listen on Port", "5555");
			
			int port = Integer.parseInt(pS.trim());
			
			String xS = this.view.requestInput("What is the X coordinate of Castle?", "0");
			String yS = this.view.requestInput("What is the Y coordinate of Castle?", "0");
			
			int x = Integer.parseInt(xS.trim());
			int y = Integer.parseInt(yS.trim());
			
			int[] castle = {x, y};
			
			this.model.startServer(p, port, castle);
		}
	}
	
	/**
	 * Choose a protocol file
	 * 
	 * @return	The name of the selected file
	 */
	public String getProtocol() {
		this.view.sendNotification("Please select a protocol file");
		File f = this.view.openFile();
		if(f != null)
			return f.getName();
		return null;
	}
	
	/**
	 * Init game
	 * 
	 * @param file		The file to be parsed
	 * @param armies	Number of armies to play the game on
	 * @return	True if file read successfully, false otherwise
	 */
	public boolean initGame(File file, int armies) {
		if(file == null)
			return false;
		return this.model.parseFile(file, armies);
	}
	
	/**
	 * Display the help
	 */
	private void displayHelp() {
		this.view.sendNotification("To use this application, begin by loading a map from the \"File\" menu.\n"+
				"Then, place pieces on the board for player 1 (they will in the order below:)\n\n"+
				"King, Prince, Duke, 2x Knights, 2x Sergeants, Squire, Archer, 4x Pikeman\n\n"+
				"Once both teams have set their pieces, use the \"Start Game\" button.\nFrom here,"+
				"the game shall progress accordingly to the rules of the game \"Feudal.\" To move a piece,\n"+
				"click the piece and click the position on the board you wish to move it to. For an archer, hold\n"+
				"the \"CTRL\" key to shoot rather than move.");
	}
}
