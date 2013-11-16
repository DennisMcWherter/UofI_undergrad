/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package games.feudal.networking;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Client sockets
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class Client {
	private Socket socket;
	private BufferedReader input;
	private PrintWriter output;
	private FeudalProtocol protocol;
	
	/**
	 * Constructor 
	 * 
	 * @param addr	The address of the server
	 * @param port	The port of the server
	 * @param file	The file name of the protocol file
	 * @throws UnknownHostException
	 * @throws IOException
	 */
	public Client(String addr, int port, String file) throws UnknownHostException, IOException { 
		this.socket   = new Socket(addr, port);
		this.protocol = new FeudalProtocol(file);
		initIO(); 
	}
	
	/**
	 * Constructor
	 * 
	 * @param client	An already open client socket
	 * @param file		The file name of the protocol file
	 * @throws IOException 
	 */
	public Client(Socket client, String file) throws IOException {
		assert(client != null);
		this.socket   = client;
		this.protocol = new FeudalProtocol(file);
		initIO();
	}
	
	/**
	 * Get a message from the other client
	 * 
	 * @return	How to handle this message from the other client
	 */
	public GameActions getResponse() {
		assert(this.output != null && this.protocol != null);
		
		return this.protocol.findAction(getMessage());
	}
	
	/**
	 * Get the next message from server
	 * 
	 * @return	The next message from the server. null if error
	 */
	public String getMessage() {
		String retval = null;
		try {
			retval = this.input.readLine();
		} catch (IOException e) {
			System.err.println("Exception Caught: "+ e.toString());
		}
		return retval;
	}
	
	/**
	 * Send a message
	 * 
	 * @param msg	The ASCII message to send
	 */
	public void sendMessage(String msg) {
		this.output.write(msg+"\r");
		this.output.flush();
	}
	
	/**
	 * Send a message
	 * 
	 * @param action	The action to send
	 */
	public void sendMessage(GameActions action) {
		String msg = this.protocol.getProperty(action);
		if(msg != null)
			sendMessage(msg);
	}
	
	/**
	 * Initialize IO components
	 * 
	 * @throws IOException
	 */
	private void initIO() throws IOException {
		assert(this.socket != null);
		
		this.input = new BufferedReader(
				new InputStreamReader(this.socket.getInputStream()));
		
		this.output = new PrintWriter(this.socket.getOutputStream(), true);
	}
}
