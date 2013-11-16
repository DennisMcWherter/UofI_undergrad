/**
 * @author Dennis J. McWherter, Jr.
 * @version 1.0
 */
package puzzle.test;

import static org.junit.Assert.*;

import org.junit.Test;

import puzzle.Board;
import puzzle.Queen;

/**
 * Tests Piece class
 * @author Dennis J. McWherter, Jr.
 * 
 */
public class PieceTest {

	/**
	 * Clean Test method for {@link puzzle.Piece#getX()}.
	 */
	@Test
	public void testGetXClean() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(2, 4, p);
		
		assertEquals(2, p.getX());
		
		b.placePiece(4, 2, p);
		assertEquals(4, p.getX());
	}
	
	/**
	 * Dirty Test method for {@link puzzle.Piece#getX()}
	 */
	@Test
	public void testGetXDirty() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(2, 4, p);
		
		b.placePiece(-1, 4, p);
		assertEquals(2, p.getX());
		
		b.placePiece(10, 4, p);
		assertEquals(2, p.getX());
	}

	/**
	 * Clean Test method for {@link puzzle.Piece#getY()}.
	 */
	@Test
	public void testGetYClean() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(0, 2, p);
		
		assertEquals(2, p.getY());
		
		b.placePiece(0, 3, p);
		assertEquals(3, p.getY());
	}
	
	/**
	 * Dirty Test method for {@link puzzle.Piece#getY()}
	 */
	@Test
	public void testGetYDirty() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(0, 2, p);
		
		b.placePiece(0, -1, p);		
		assertEquals(2, p.getY());
		
		b.placePiece(0, 10, p);
		assertEquals(2, p.getY());
	}

	/**
	 * Clean Test method for {@link puzzle.Piece#setX(int)}.
	 */
	@Test
	public void testSetXClean() {
		Queen p = new Queen();
		Board b = new Board();
		
		// placePiece relies on setX()
		b.placePiece(2, 2, p);
		assertEquals(2, p.getX());
		
		b.placePiece(0, 0, p);
		assertEquals(0, p.getX());
	}
	
	/**
	 * Dirty Test method for {@link puzzle.Piece#setX(int)}
	 */
	@Test
	public void testSetXDirty() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(-1, 0, p);
		assertEquals(-1, p.getX());
		assertNull(p.getParentBoard());
		
		b.placePiece(1, 0, p);
		
		b.placePiece(10, 0, p);
		assertEquals(1, p.getX());
		assertSame(b, p.getParentBoard());
	}

	/**
	 * Test method for {@link puzzle.Piece#setY(int)}.
	 */
	@Test
	public void testSetYClean() {
		Queen p = new Queen();
		Board b = new Board();
		
		// placePiece relies on SetY
		b.placePiece(2, 2, p);
		assertEquals(2, p.getY());
		
		b.placePiece(2, 3, p);
		assertEquals(3, p.getY());
	}
	
	/**
	 * Dirty Test method for {@link puzzle.Piece#setY(int)}
	 */
	@Test
	public void testSetYDirty() {
		Queen p = new Queen();
		Board b = new Board();
		
		b.placePiece(0, 10, p);
		assertEquals(-1, p.getY());
		assertNull(p.getParentBoard());
		
		b.placePiece(0, 0, p);
		
		b.placePiece(0, -1, p);
		assertEquals(0, p.getY());
		assertSame(b, p.getParentBoard());
	}

	/**
	 * Test method for {@link puzzle.Piece#getParentBoard()}.
	 */
	@Test
	public void testGetParentBoard() {
		Queen p = new Queen();
		Board b = new Board();
		
		p.setParentBoard(b);
		
		assertEquals(b, p.getParentBoard());
	}

	/**
	 * Test method for {@link puzzle.Piece#setParentBoard(puzzle.Board)}.
	 */
	@Test
	public void testSetParentBoard() {
		Queen p = new Queen();
		Board b = new Board();
		
		assertNull(p.getParentBoard());
		
		p.setParentBoard(b);
		assertSame(b, p.getParentBoard());
	}

}
