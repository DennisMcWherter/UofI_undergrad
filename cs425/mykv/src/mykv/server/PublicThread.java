/**
 * PublicThread.java
 * 
 * Handles all public client connections
 */
package mykv.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

public class PublicThread extends ThreadBase {
	private ServerSocket server; /* Server socket to listen for clients */
	private Socket client; /* Current connected client */
	
	/**
	 * Constructor
	 * 
	 * @param sock			Open server socket
	 * @param db			In-memory database
	 * @param serverId		Server ID
	 * @param numServers	Number of servers on internal network
	 * @param cli		True if this thread handles incoming clients, false if for servers.
	 */
	public PublicThread(ServerSocket sock, ConcurrentHashMap<Integer, String> db, int serverId, int numServers)
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
		System.out.println("Waiting for public clients on "+server.getInetAddress().toString()+":"+server.getLocalPort());

		while(true) {
			try {
				client = server.accept();
			} catch (IOException e1) {
				// Ignore
				client = null; // paranoia
			}
			
			handleClient();
			
			// Check if we need to exit
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				break; // Exit thread
			}
		}
		System.out.println("Client handling thread exiting...");
	}
	
	/**
	 * Handle a connected client
	 */
	private void handleClient()
	{
		String input = null;
		boolean closeConn = false;
		
		if(client == null)
			return;
		
		// Make sure we get these setup
		if(!setupBufferedReader(client) || !setupPrintWriter(client))
			return;
		
		System.out.println("Client ("+client.getInetAddress().toString()+":"+client.getLocalPort()+") connected");
		
		try {
			while((input = reader.readLine()) != null && !closeConn) {
				System.out.println("Received client-side request: "+input);
				closeConn = processInputLine(input);
			}
		} catch (IOException e) {
			// Ignore
		}
		
		try {
			writer.close();
			reader.close();
			client.close();
		} catch (IOException e) {
			// Ignore
		}
		client = null;
		writer = null;
		reader = null;
	}
}
