import java.io.IOException;
import java.util.Vector;

import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;


public class TestReader {
	public static void main(String[] args) {
		XMLReader reader = new XMLReader("/Users/nemo/cs423-mps/mp4/data/1.xml");
		Vector<Page> pages = null;
		try {
			pages = reader.parse();
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
//		for(Page x : pages) {
//			System.out.println("Title: " + x.title);
//		}
		reader.writeNewFile("flat.txt");
	}
}
