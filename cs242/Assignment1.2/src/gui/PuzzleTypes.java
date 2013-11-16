/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui;

/**
 * Enum for puzzle types
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public enum PuzzleTypes {
	N_QUEENS("N Queens"), N_ROOKS("N Rooks"),
	M_QUEENSKNIGHTS("M Queens and Knights"),
	M_CUSTOM("M Custom Pieces");
	
	private String value;
	
	/**
	 * Private constructor to hold values
	 * 
	 * @param str	The particular puzzle's string identifier
	 */
	private PuzzleTypes(String str) {
		value = str;
	}
	
	/**
	 * Test whether or not an ASCII string is equal to the enum
	 * 
	 * @return	True if equal, false otherwise
	 */
	public boolean isEqual(String test) {
		return this.value.equalsIgnoreCase(test);
	}
}
