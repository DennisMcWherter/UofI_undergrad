
public class Formatter {

    public static void main(String[] args) throws Exception {

        XMLReader x = new XMLReader(args[0]);
        x.writeNewFile(args[1]+"/flat.txt");
    }

}
