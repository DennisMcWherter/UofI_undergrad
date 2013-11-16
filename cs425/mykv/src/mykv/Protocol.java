/**
 * Protocol.java
 * 
 * Protocol messages
 */
package mykv;

public enum Protocol {
	EXIT_REQ("EXIT"), INSERT_REQ("INSERT"), REMOVE_REQ("REMOVE"), LOOKUP_REQ("LOOKUP"),
	EXIT_SUC("EXIT_SUCCESS"), INSERT_SUC("INSERT_SUCCESS"), REMOVE_SUC("REMOVE_SUCCESS"), LOOKUP_SUC("LOOKUP_SUCCESS"),
	EXIT_FAIL("EXIT_FAIL"), INSERT_FAIL("EXIT_FAIL"), REMOVE_FAIL("REMOVE_FAIL"), LOOKUP_FAIL("LOOKUP_FAIL"),
	LOOKUP_NOEXIST("LOOKUP_NOEXIST");
	
	private String codeString;
	
	private Protocol(String c)
	{
		codeString = c;
	}
	
	public String getStringCode()
	{
		return codeString;
	}
}
