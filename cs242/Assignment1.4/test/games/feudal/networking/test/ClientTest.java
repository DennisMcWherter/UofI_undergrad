package games.feudal.networking.test;

import static org.junit.Assert.*;

import java.io.IOException;
import java.net.Socket;

import games.feudal.networking.Client;
import games.feudal.networking.GameActions;
import games.feudal.networking.Server;

import org.junit.BeforeClass;
import org.junit.Test;

public class ClientTest {
	public static Client client;
	public static Client connectedClient;
	public static Thread serverThread;
	
	// NOTE: It is possible for this to fail (improperly)
	// due to a bind error. If this occurs when running with
	// the whole test suite, try to run it separately
	public static final int THEPORT = 5555;

	/**
	 * Setup
	 * @throws Exception
	 */
	@BeforeClass
	public static void setUp() throws Exception {
		connectedClient = null;
		serverThread = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					Server server = new Server(ClientTest.THEPORT);
					System.out.println("Waiting");
					Socket s = server.waitForClient();
					System.out.println("Connected");
					connectedClient = new Client(s, "feudal.prototest");
				} catch (IOException e) {
					// Ignore
					System.err.println("Ah! Could not do something!\n"+
					e.toString());
				}
			} 
		});
		
		serverThread.start();
		
		int i = 0;
		while(connectedClient == null) {
			if(++i > 50) {
				client = new Client("localhost", ClientTest.THEPORT, "feudal.prototest");
				System.out.println("Attempted connection");
			}
			Thread.sleep(100);
		}
	}
	
	/**
	 * Test basic connection
	 */
	@Test
	public void testConnects() {
		assertNotNull(client);
		assertNotNull(connectedClient);
	}
	
	/**
	 * Test get response
	 */
	@Test
	public void testGetResponse() {
		connectedClient.sendMessage(GameActions.BOARDSETUP);
		assertEquals(GameActions.BOARDSETUP, client.getResponse());
		
		connectedClient.sendMessage(GameActions.NEXTTURN);
		assertEquals(GameActions.NEXTTURN, client.getResponse());
	}
	
	/**
	 * Test get message
	 */
	@Test
	public void testGetMsg() {
		connectedClient.sendMessage("w00t");
		assertEquals("w00t", client.getMessage());
		
		String someString = "with arbitrary contents can be passed";
		connectedClient.sendMessage(someString);
		assertEquals(someString, client.getMessage());
	}
	
	/**
	 * Test send message
	 */
	@Test
	public void testSendMsg() {
		client.sendMessage(GameActions.GAMEOVER);
		assertEquals(GameActions.GAMEOVER, connectedClient.getResponse());
		
		client.sendMessage("Test");
		assertEquals("Test", connectedClient.getMessage());
	}
}
