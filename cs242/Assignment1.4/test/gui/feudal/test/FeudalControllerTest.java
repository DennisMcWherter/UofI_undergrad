/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package gui.feudal.test;

import static org.junit.Assert.*;

import java.io.File;

import gui.feudal.FeudalController;
import gui.feudal.FeudalModel;

import org.junit.Before;
import org.junit.Test;

/**
 * Tests the Feudal Controller
 * 
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
public class FeudalControllerTest {
	public FeudalController ctrl;
	public File file;

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		ctrl = new FeudalController(new FeudalModel(null), null);
		file = new File("test.xml");
	}

	/**
	 * Test method to read the file
	 */
	@Test
	public void testReadFile() {
		assertTrue(ctrl.initGame(file, 2));
	}

}
