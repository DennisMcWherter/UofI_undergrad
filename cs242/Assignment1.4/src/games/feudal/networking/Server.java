/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.networking;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * The server class
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Server {
	private ServerSocket serverSocket;
	private Socket clientSocket;

	/**
	 * Constructor
	 * 
	 * @param port		Port to bind to
	 * @throws IOException
	 */
	public Server(int port) throws IOException {
		this.serverSocket = new ServerSocket(port);
		this.clientSocket = null;
	}
	
	/**
	 * Wait for a client
	 * 
	 * @return	The new client socket
	 * @throws IOException
	 */
	public Socket waitForClient() throws IOException {
		assert(this.serverSocket != null);
		
		if(this.clientSocket != null)
			this.clientSocket.close();
		
		return (this.clientSocket = this.serverSocket.accept());
	}
}
