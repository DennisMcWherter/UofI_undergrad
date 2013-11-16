package games.feudal.networking.test;

import games.feudal.networking.Server;

import org.junit.Before;
import org.junit.Test;

public class ServerTest {
	Server server;

	@Before
	public void setUp() throws Exception {
		server = new Server(5555);
	}

	/**
	 * Test wait for client
	 */
	@Test
	public void testWaitForClient() {
		// Must be done manually
	}

}
