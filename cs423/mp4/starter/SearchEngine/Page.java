import java.util.StringTokenizer;


public class Page {
	public int id;
	public String title;
	public String text;
	
	public Page(int id, String title, String text) {
		this.id    = id;
		this.title = title;
		this.text  = text;
	}
	
	public String toString(){
		
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
}
