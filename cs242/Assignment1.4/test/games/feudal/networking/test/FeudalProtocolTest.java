package games.feudal.networking.test;

import static org.junit.Assert.*;
import games.feudal.networking.FeudalProtocol;
import games.feudal.networking.GameActions;

import org.junit.Before;
import org.junit.Test;

public class FeudalProtocolTest {
	FeudalProtocol proto;

	/**
	 * Setup
	 * @throws Exception
	 */
	@Before
	public void setUp() throws Exception {
		proto = new FeudalProtocol("feudal.prototest");
	}

	/**
	 * Test to be certain we have the proper action parsing
	 */
	@Test
	public void testFindAction() {
		assertEquals(GameActions.NEWGAME, proto.findAction("NEWGAME"));
		assertEquals(GameActions.UNHANDLED, proto.findAction("w00t"));
		assertEquals(GameActions.PROTOCOL, proto.findAction("FEUDALDENNIS"));
		assertEquals(GameActions.BOARDSETUP, proto.findAction("BOARDSETUP"));
	}

	/**
	 * Test properties work
	 */
	@Test
	public void testGetProperty() {
		assertEquals("NEWGAME", proto.getProperty(GameActions.NEWGAME));
		assertNull(proto.getProperty(GameActions.UNHANDLED));
		assertEquals("FEUDALDENNIS", proto.getProperty(GameActions.PROTOCOL));
	}

}
