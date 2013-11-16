/**
 * Server.java
 * MyKV Server implementation
 */
package mykv.server;

/* Java constructs */
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;

public class Server {
	// Important networking constructs
	private ServerSocket internal; /* Another server connects to this socket */
	private ServerSocket server; /* Clients connect to this socket */
	private volatile Socket inClient; /* Connect to another server internally with this */
	
	// Important concurrent data structures for inter-thread communication
	private ConcurrentHashMap<Integer, String> memdb; /* In-memory DB specified by a map */
	
	// Threads
	private InternalThread inThread; /* This will process incoming requests from other servers */
	private PublicThread pubThread; /* This will process incoming requests from external client(s) */
	private Thread threadIn; /* The actual thread object for internal */
	private Thread threadPub; /* The actual thread object for public */
	
	// Other variables
	private InetAddress nextServer;
	private int nextPort;
	
	// Information for this server
	private int serverId;
	private int numServers;
	
	/**
	 * Constructor
	 * 
	 * @param serverId		ID of the current server
	 * @param numServers	Number of servers on the internal network
	 * @param bindAddr		Address to bind to
	 * @param inPort		Internal port for another server to connect to
	 * @param pubPort		Public port for external clients to send requests to
	 * @param next			The next server's IP in the ring (who to connect to)
	 * @param nextp			The next server's port to connect to
	 * @throws IOException
	 */
	public Server(int sid, int nservs, InetAddress bindAddr, int inPort, int pubPort, InetAddress next, int nextp) throws IOException
	{
		serverId   = sid;
		numServers = nservs;
		internal   = new ServerSocket(inPort, 1, bindAddr);
		server     = new ServerSocket(pubPort, 10, bindAddr); 
		memdb      = new ConcurrentHashMap<Integer, String>();
		nextServer = next;
		nextPort   = nextp;
		inClient = null;
	}
	
	/**
	 * Main method to run the server
	 */
	public void runServer()
	{
		// Start threads
		inThread  = new InternalThread(internal, memdb, serverId, numServers);
		pubThread = new PublicThread(server, memdb, serverId, numServers);
		threadIn  = new Thread(inThread);
		threadPub = new Thread(pubThread);
		
		threadIn.start();
		threadPub.start();
		
		// Try to connect to the next server in the ring
		while(inClient == null) {
			try {
				inClient = new Socket(nextServer, nextPort);
			} catch (IOException e) {
				// Do nothing, try again soon.
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
					// Do nothing
				}
			}
		}
		
		System.out.println("Connected to next server ("+inClient.getInetAddress().toString()+":"+inClient.getPort()+")");
		
		// Internally connected, let our public thread know - internal thread should receive the request from elsewhere
		inThread.updateNext(inClient);
		pubThread.updateNext(inClient);
		
		// Sleep this thread and let the other two do the work.
		synchronized(Thread.currentThread()) {
			try {
				Thread.currentThread().wait();
			} catch (InterruptedException e) {
				// This is probably not good.
				System.out.println("Exception Caught: "+e.toString());
			}
		}
		
		threadIn.interrupt();
		threadPub.interrupt();
	}
}
