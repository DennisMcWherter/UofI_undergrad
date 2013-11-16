/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.networking;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

/**
 * Vanilla protocol (read from some file)
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalProtocol {
	private Properties prop;
	private String newGame;
	private String nextTurn;
	private String gameOver;
	private String boardSetup;
	private String protocol;
	private String checkProto;
	private String upBoard;
	
	/**
	 * Constructor
	 * 
	 * @param file	File name of the protocol file to be read
	 * @throws IOException 
	 * @throws FileNotFoundException 
	 */
	public FeudalProtocol(String file) throws FileNotFoundException, IOException {
		this.prop = new Properties();
		
		this.prop.load(new FileInputStream(file));
		
		getProperties();
	}
	
	/**
	 * Determine which action should be done based on a server
	 * message
	 * 
	 * @param msg	Message received
	 * @return	The appropriate action to perform
	 */
	public GameActions findAction(String msg) {
		if(msg == null)
			return GameActions.UNHANDLED;
		if(this.newGame.equals(msg))
			return GameActions.NEWGAME;
		else if(this.nextTurn.equals(msg))
			return GameActions.NEXTTURN;
		else if(this.protocol.equals(msg))
			return GameActions.PROTOCOL;
		else if(this.boardSetup.equals(msg))
			return GameActions.BOARDSETUP;
		else if(this.gameOver.equals(msg))
			return GameActions.GAMEOVER;
		else if(this.upBoard.equals(msg))
			return GameActions.UPDATEBOARD;
		return GameActions.UNHANDLED;
	}
	
	/**
	 * Get a property value based on an action
	 * 
	 * @param action	The action
	 * @return	The property value of an action. If action does not
	 * 			exist, then null
	 */
	public String getProperty(GameActions action) {
		if(GameActions.NEWGAME.equals(action))
			return this.newGame;
		else if(GameActions.NEXTTURN.equals(action))
			return this.nextTurn;
		else if(GameActions.GAMEOVER.equals(action))
			return this.gameOver;
		else if(GameActions.CHECKPROTO.equals(action))
			return this.checkProto;
		else if(GameActions.PROTOCOL.equals(action))
			return this.protocol;
		else if(GameActions.BOARDSETUP.equals(action))
			return this.boardSetup;
		else if(GameActions.UPDATEBOARD.equals(action))
			return this.upBoard;
		return null;
	}
	
	/**
	 * Try to read all necessary protocol information
	 * 
	 * @throws IOException
	 */
	private void getProperties() throws IOException {
		this.newGame    = this.prop.getProperty("newGame");
		this.nextTurn   = this.prop.getProperty("nextTurn");
		this.gameOver   = this.prop.getProperty("gameOver");
		this.boardSetup = this.prop.getProperty("boardSetup");
		this.checkProto = this.prop.getProperty("checkProto");
		this.protocol	= this.prop.getProperty("protocol");
		this.upBoard	= this.prop.getProperty("updateBoard");
		
		if(this.newGame == null || this.nextTurn == null ||
				this.gameOver == null || this.protocol == null ||
				this.boardSetup == null || this.checkProto == null ||
				this.upBoard == null)
			throw new IOException("Invalid protocol");
	}
}
