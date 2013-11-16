/**
 * InternalThread.java
 * 
 * Implementation for the internal server thread
 */
package mykv.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

public class InternalThread extends ThreadBase {
	private ServerSocket server;
	private Socket client;
	
	/**
	 * Constructor
	 * 
	 * @param sock			Open server socket
	 * @param serverId		Server ID
	 * @param numServers	Number of servers on internal network
	 */
	public InternalThread(ServerSocket sock, ConcurrentHashMap<Integer, String> db, int serverId, int numServers)
	{
		super(db, serverId, numServers);
		server = sock;
		client = null;
	}
	
	/**
	 * Thread entry point
	 */
	@Override
	public void run()
	{
		System.out.println("Waiting for server clients on "+server.getInetAddress().toString()+":"+server.getLocalPort());

		while(true) {
			try {
				client = server.accept();
			} catch (IOException e1) {
				// Ignore
				client = null; // paranoia
			}
			
			handleRequests();
			
			// Check if we need to exit
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				break; // Exit thread
			}
		}
		System.out.println("Server handling thread exiting...");
	}
	
	/**
	 * Handle all incoming requests
	 */
	private void handleRequests()
	{
		String input = null;
		boolean closeConn = false;

		if(client == null)
			return;
		
		// Make sure we get these setup
		if(!setupBufferedReader(client) || !setupPrintWriter(client))
			return;

		System.out.println("Server (" + client.getInetAddress().toString() + ":"
				+ client.getLocalPort() + ") connected");

		try {
			while ((input = reader.readLine()) != null && !closeConn) {
				System.out.println("Received server-side request: "+input);
				closeConn = processInputLine(input);
			}
		} catch (IOException e) {
			// Ignore
		}

		try {
			nextWriter.close();
			nextReader.close();
			next.close();
		} catch (IOException e) {
			// Ignore
		}
		nextWriter = null;
		nextReader = null;
		next       = null;
	}
}
