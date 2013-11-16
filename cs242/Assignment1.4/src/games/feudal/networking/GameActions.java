/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.networking;

/**
 * Specific game actions which clients should be able to handle
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public enum GameActions {
	NEWGAME, BOARDSETUP, GAMEOVER, NEXTTURN, PROTOCOL, CHECKPROTO, 
	UPDATEBOARD, UNHANDLED;
}
