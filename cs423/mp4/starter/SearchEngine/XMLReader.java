import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

import java.lang.StringBuilder;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Vector;
import java.util.StringTokenizer;

public class XMLReader {
	private String fileName;
	private NodeList nList;
    private String content;


	public XMLReader(String filename) {
		fileName = filename;
		nList = null;
        content = "";
    }

	// Code based on:
	// http://www.mkyong.com/java/how-to-read-xml-file-in-java-dom-parser/
	public void parse() throws Exception{
		
        File xmlFile = new File(fileName);
		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
		Document document = dBuilder.parse(xmlFile);
		//Vector<Page> pages = new Vector<Page>();

		document.getDocumentElement().normalize();
		nList = document.getElementsByTagName("page");

        StringBuilder out = new StringBuilder();

		for (int i = 0; i < nList.getLength(); ++i) {
			Node nNode = nList.item(i);
			if (nNode.getNodeType() == Node.ELEMENT_NODE) {
				Element elem = (Element) nNode;
				//pages.add(new Page(i, getTagValue("title", elem), getTagValue("text",elem)));
				out.append(getPageString(i, getTagValue("title", elem), getTagValue("text", elem)));
        out.append('\n');
				String title = getTagValue("title", elem);
				if (title != null && title.contains("ennis")){
					System.out.print(title+"\n");
				}
      }
		}

		content = out.toString();
	}

	private static String getTagValue(String tag, Element elem) {
		NodeList ndList = elem.getElementsByTagName(tag).item(0).getChildNodes();
		Node nVal = (Node) ndList.item(0);
		return nVal.getNodeValue();
	}

    private static String getPageString(int id, String title, String text){
	
        
		title  = title.replaceAll("\\\\n", " ");
		text  = text.replaceAll("\\\\n", " ");
		title  = title.replaceAll("\\\\t", " ");
		text  = text.replaceAll("\\\\t", " ");
		title  = title.replaceAll("\\\\r", " ");
		text  = text.replaceAll("\\\\r", " ");
		

		String[] titleTokens = title.toLowerCase().split("[^a-z\n]");
		String[] textTokens = text.toLowerCase().split("[^a-z\n]");
		
		StringBuilder titleBuilder = new StringBuilder(title.length());
		StringBuilder textBuilder = new StringBuilder(text.length());
		
		for (int i = 0; i < titleTokens.length; i++){
			titleBuilder.append(titleTokens[i]);
			titleBuilder.append(" ");
		}
		
		for (int i = 0; i < textTokens.length; i++){
			textBuilder.append(textTokens[i]);
			textBuilder.append(" ");
		}
		
		String titleString = titleBuilder.toString();
		String textString = textBuilder.toString();
		
		StringTokenizer titleToken = new StringTokenizer(titleString);
		StringTokenizer textToken = new StringTokenizer(textString);
		
		titleBuilder = new StringBuilder(text.length());
		textBuilder = new StringBuilder(text.length());
		
		titleBuilder.append(id+" ");
		
		while (titleToken.hasMoreTokens()){
			titleBuilder.append(titleToken.nextToken());
			titleBuilder.append(" ");
		}
		
		titleBuilder.append(": ");
		
		while (textToken.hasMoreTokens()){
			textBuilder.append(textToken.nextToken());
			textBuilder.append(" ");
		}
		
		titleBuilder.append(textBuilder.toString());
		
		return titleBuilder.toString();
	}

	
	public void writeNewFile(String filename){
		
        
		try {
            this.parse();
			FileWriter fstream = new FileWriter(filename);
			BufferedWriter out = new BufferedWriter(fstream);
			
            /*
            Page p;
			
			for (int i= 0; i < pages.size(); i++){
				p = pages.get(i);
				out.write(p.toString());
				out.write('\n');
			}
			*/

            out.write(content);

			out.close();
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
    
	}
}
