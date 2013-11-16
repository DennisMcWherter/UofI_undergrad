/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal;

import games.Board;

/**
 * Feudal game board
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalBoard extends Board {
	public final static int BOARD_SIZE = 24; // Regulation Feudal game size
	
	private boolean[] roughTerrain;
	private boolean[] mountains;
	private int[][] castles;
	private GameManager manager;
	
	/**
	 * Constructor
	 * 
	 * @param manager	The game manager for this board
	 */
	public FeudalBoard(GameManager manager) {
		super(BOARD_SIZE);
		this.roughTerrain = new boolean[BOARD_SIZE*BOARD_SIZE];
		this.mountains    = new boolean[BOARD_SIZE*BOARD_SIZE];
		this.castles	  = new int[GameManager.MAX_ARMIES][4];
		this.manager	  = manager;
		
		for(int i = 0 ; i < this.castles.length ; ++i) {
			this.castles[i][0] = -1;
			this.castles[i][1] = -1;
			this.castles[i][2] = -1;
			this.castles[i][3] = -1;
		}
		
		this.reset();
	}
	
	/**
	 * Reset board
	 */
	@Override
	public void reset() {
		for(int i = 0 ; i < this.manager.getArmyCount() ; ++i) {
			this.castles[i][0] = -1;
			this.castles[i][1] = -1;
			this.castles[i][2] = -1;
		}
		
		for(int i = 0 ; i < (BOARD_SIZE*BOARD_SIZE) ; ++i) {
			this.roughTerrain[i] = false;
			this.mountains[i]    = false;
		}
		
		super.reset();
	}
	
	/**
	 * Get the castles and their coords
	 * 
	 * @return	The castle array
	 */
	public int[][] getCastles() {
		return this.castles;
	}
	
	/**
	 * Determine rough terrain
	 * 
	 * @param x		X coordinate to check
	 * @param y		Y coordinate to check
	 * @return	True if rough terrain, false otherwise
	 */
	public boolean checkRough(int x, int y) {
		if(!validBounds(x, y))
			return false;
		int idx = coordsToIdx(x, y);
		return this.roughTerrain[idx];
	}
	
	/**
	 * Determine whether a mountain exists on square
	 * 
	 * @param x		X coordinate to check
	 * @param y		Y coordinate to check
	 * @return	True if a mountain exists, otherwise false
	 */
	public boolean checkMountain(int x, int y) {
		if(!validBounds(x, y))
			return false;
		int idx = coordsToIdx(x, y);
		return this.mountains[idx];
	}
	
	/**
	 * Change terrain to either rough or not rough (opposite of current value)
	 *  at a given location
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 */
	public void toggleRoughTerrain(int x, int y) {
		if(!validBounds(x, y))
			return;
		int idx = coordsToIdx(x, y);
		this.roughTerrain[idx] = !this.roughTerrain[idx];
	}
	
	/**
	 * Either turn a mountain on or off at a coordinate (opposite of current value)
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 */
	public void toggleMountain(int x, int y) {
		if(!validBounds(x, y))
			return;
		int idx = coordsToIdx(x, y);
		this.mountains[idx] = !this.mountains[idx];
	}
	
	/**
	 * Place a castle on the board
	 * 
	 *  @param x	X position
	 *  @param y	Y position
	 *  @param army	The army to which this castle belongs
	 *  @param ort	Orientation of the castle (0 = y+1, 1 = y-1) [unused]
	 *  @return	Returns true if castle could be placed, false otherwise
	 */
	public boolean setCastle(int x, int y, int army, int ort) {
		if(!validBounds(x, y) || army >= this.manager.getArmyCount() ||
				this.castles[army][0] != -1)
			return false;
		
		int check = (ort == 0) ? 1 : -1;
		
		if((y + check) > BOARD_SIZE || (y + check) < 0)
			return false;
		
		this.castles[army][0] = x; // x1
		this.castles[army][1] = y; // y
		this.castles[army][2] = x+1; // x2
		this.castles[army][3] = x+2; // x3
		//this.castles[army][2] = ort;
		
		return true;
	}
	
	/**
	 * Get a castle position
	 * 
	 * @param player	Get this particular player's castle
	 * @return	An integer array of the form (x, y) with the coordinates
	 * 			otherwise null if error
	 */
	public int[] getCastle(int player) {
		if(player < 0 || player >= GameManager.MAX_ARMIES)
			return null;
		int[] castle = {this.castles[player][0], this.castles[player][1]};
		return castle;
	}
	
	/**
	 * Check if castle on position
	 * 
	 * @param x		X coord
	 * @param y		Y coord
	 * @return	True if a castle is on this position, false otherwise
	 */
	public boolean checkCastle(int x, int y) {
		if(!validBounds(x, y))
			return false;
		
		for(int i = 0 ; i < this.manager.getArmyCount() ; ++i) {
			if(this.castles[i][0] == -1)
				continue;
			
			int ort = (this.castles[i][2] == 0) ? 1 : -1;
			
			if(this.castles[i][0] == x &&
			  (this.castles[i][1] == y || this.castles[i][1]+ort == y))
					return true;
		}
		return false;
	}
	
	/**
	 * Convert coordinates into an index
	 * 
	 * @param x		X coordinate to convert
	 * @param y		Y coordinate to convert
	 * @return	The index of the corresponding (x, y) pair on the
	 * 		board/maps
	 */
	public static int coordsToIdx(int x, int y) {
		return (BOARD_SIZE*y)+x;
	}
	
	/**
	 * Check whether coordinates are on the board or not
	 * 
	 * @param x		X coordinate
	 * @param y		Y coordinate
	 * @return	True if on board, false otherwise
	 */
	private boolean validBounds(int x, int y) {
		if(x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE)
			return false;
		return true;
	}
}
