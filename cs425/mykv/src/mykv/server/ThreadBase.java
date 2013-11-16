/**
 * ThreadBase.java
 * 
 * Base class for the threads
 */
package mykv.server;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;
//import java.util.concurrent.ConcurrentLinkedQueue;

import mykv.Protocol;

public abstract class ThreadBase implements Runnable {
	// Common variables both threads may use
	protected volatile Socket next;
	protected BufferedReader reader;
	protected BufferedReader nextReader;
	protected PrintWriter writer;
	protected PrintWriter nextWriter;
	protected ConcurrentHashMap<Integer, String> memdb;
	protected int serverId;
	protected int numServers;
	protected long lookupNum;

	// Assume (based on MP spec) keys range [1, 1000000]
	private static final int maxKeys = 1000000;
	
	/**
	 * Constructor
	 * 
	 * @param db		In memory database object
	 * @param sid		The server id
	 * @param nservs	Number of servers on the internal network
	 */
	protected ThreadBase(ConcurrentHashMap<Integer, String> db, int sid, int nservs)
	{
		next       = null;
		reader     = null;
		writer     = null;
		memdb      = db;
		serverId   = sid;
		numServers = nservs;
	}
	
	/**
	 * Update the next server which you are connected to in the ring
	 * @param sock	The open socket to the server
	 */
	public void updateNext(Socket sock)
	{
		if(sock == null)
			return;
		next = sock;
		try {
			nextReader = new BufferedReader(new InputStreamReader(sock.getInputStream()));
			nextWriter = new PrintWriter(sock.getOutputStream(), true);
		} catch (IOException e) {
			// Ignore
		}
	}
	
	/**
	 * Setup the reader
	 * 
	 * @param sock	Open socket to setup the reader on
	 * @return	true if successful, false otherwise
	 */
	protected boolean setupBufferedReader(Socket sock)
	{
		try {
			reader = new BufferedReader(new InputStreamReader(sock.getInputStream()));
		} catch (IOException e) {
			return false;
		}
		
		return true;
	}
	
	/**
	 * Setup the writer
	 * 
	 * @param sock	Open socket to setup the writer on
	 * @return	true if successful, false otherwise.
	 */
	protected boolean setupPrintWriter(Socket sock)
	{
		try {
			writer = new PrintWriter(sock.getOutputStream(), true);
		} catch (IOException e) {
			return false;
		}
		
		return true;
	}
	
	/**
	 * Process an input line
	 * 
	 * @param line		The line to process
	 * @return	True if connection is to be closed. False otherwise.
	 */
	protected boolean processInputLine(String line)
	{
		if(line == null)
			return false; // paranoia - should never happen
		
		if(line.startsWith(Protocol.EXIT_REQ.getStringCode()))
			return true;
		
		if(line.startsWith(Protocol.INSERT_REQ.getStringCode()))
			handleInsert(line);
		else if(line.startsWith(Protocol.REMOVE_REQ.getStringCode()))
			handleRemove(line);
		else if(line.startsWith(Protocol.LOOKUP_REQ.getStringCode()))
			handleLookup(line);
		
		return false;
	}
	
	/**
	 * Handle an insert request
	 * 
	 * @param line	The input line (i.e. the request)
	 */
	protected void handleInsert(String line)
	{
		String vals[] = line.split(":", 3);
		
		if(vals.length != 3) {
			System.out.println("Invalid insert request");
			writer.println(Protocol.INSERT_FAIL.getStringCode());
			return;
		}
		
		int id = Integer.parseInt(vals[1]);
		String data = vals[2];
		
		if(!idInRange(id) || id < 0) { // Forward the request
			String fmsg = "Requested id: "+id+" not on this server but we have no connection to any other server";
			forwardRequest(line, fmsg, Protocol.INSERT_FAIL);
			return;
		}
		
		memdb.put(id, data);
		
		if(data.compareTo(memdb.get(id)) != 0) {
			writer.println(Protocol.INSERT_FAIL.getStringCode());
		} else {
			writer.println(Protocol.INSERT_SUC.getStringCode());
		}
	}
	
	/**
	 * Handle a remove request
	 * 
	 * @param line	The input line (i.e. the request)
	 */
	protected void handleRemove(String line)
	{
		String[] vals = line.split(":", 2);
		
		if(vals.length != 2) {
			writer.println(Protocol.REMOVE_FAIL.getStringCode());
			return;
		}
		
		int id = Integer.parseInt(vals[1]);
		
		if(!idInRange(id)) { // Forward the request
			String fmsg = "Requested id: "+id+" not on this server but we have no connection to any other server";
			forwardRequest(line, fmsg, Protocol.REMOVE_FAIL);
			return;
		}
		
		memdb.remove(id);
		
		if(memdb.get(id) != null) {
			writer.println(Protocol.REMOVE_FAIL.getStringCode());
		} else {
			writer.println(Protocol.REMOVE_SUC.getStringCode());
		}
	}
	
	/**
	 * Handle lookup requests
	 * 
	 * @param line	The input line (i.e. the request)
	 */
	protected void handleLookup(String line)
	{
		String[] vals = line.split(":", 2);
		
		if(vals.length != 2) {
			writer.println(Protocol.LOOKUP_FAIL.getStringCode());
			return;
		}
		
		int id = Integer.parseInt(vals[1]);
		
		lookupNum++;
		
		if(!idInRange(id)) { // Forward the request
			if(id < 0) {
				if((line = processNumReq(id, line)) == null) {
					writer.println(Protocol.LOOKUP_SUC.getStringCode());
					return;
				}
			}
			String fmsg = "Requested id: "+id+" not on this server but we have no connection to any other server";
			forwardRequest(line, fmsg, Protocol.LOOKUP_FAIL);
			return;
		}
		
		String res = memdb.get(id);
		
		if(res == null) {
			writer.println(Protocol.LOOKUP_NOEXIST.getStringCode());
		} else {
			writer.println(Protocol.LOOKUP_SUC.getStringCode()+":"+res);
		}
	}
	
	/**
	 * Forward a given request
	 * 
	 * @param req		The request line
	 * @param srvmsg	The error message for the server
	 * @param fail		Fail protocol to send to client
	 */
	protected void forwardRequest(String req, String srvmsg, Protocol fail)
	{
		System.out.println("Forwarding request: "+req);
		
		if(next == null || nextReader == null || nextWriter == null) { // Problematic
			writer.println(fail.getStringCode());
			System.out.println(srvmsg);
			return;
		}
		
		String res = null;
		synchronized(next) {
			synchronized(nextWriter) {
				synchronized(nextReader) {
					nextWriter.println(req);
					try {
						res = nextReader.readLine();
					} catch (IOException e) {
						System.out.println("Unrecoverable error (could not read from next");
						writer.println(fail.getStringCode());
						return;
					}
					
					if(res == null) {
						writer.println(fail.getStringCode());
					} else {
						writer.println(res);
					}
				}
			}
		}
	}
	
	/**
	 * Check if an ID is on this server
	 * 
	 * @param id	The ID to check
	 * @return 	true if key is stored on this server, false otherwise.
	 */
	private boolean idInRange(int id)
	{
		int keysPer = maxKeys / numServers;
		int min = ((serverId - 1) * keysPer) + 1;
		int max = serverId * keysPer;
		if(id < 1 || id < min || id > max)
			return false;
		return true;
	}
	
	/**
	 * Special function for analysis purposes.
	 * This will appropriately give the number of
	 * keys on a given server.
	 * 
	 * @param id	The current value of keys
	 * @param line	The line to update
	 * @return	Returns null if last server to receive, otherwise updated line
	 */
	private String processNumReq(int id, String line)
	{
		if(id > -1)
			return null; // Should never get here
		
		System.out.println("Number of keys on server: "+memdb.size()+"\nNumber of of received lookup messages: "+lookupNum);
		
		if(Math.abs(id) > (numServers - 1))
			return null;
		
		return new String("LOOKUP:"+(id-1));
	}
}
