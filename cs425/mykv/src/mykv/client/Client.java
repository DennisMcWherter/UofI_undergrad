/**
 * Client.java
 * 
 * Client implementation
 */
package mykv.client;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Vector;

import mykv.Protocol;

public class Client {
	private Socket conn;
	private BufferedReader reader;
	private PrintWriter writer;
	private static boolean close = false;
	private Vector<Long> insertTime;
	private Vector<Long> lookupTime;
	private AnalysisRun analysis;
	
	// Messages
	private static final String WELCOME_MSG_BEGIN ="Welcome to MyKV client!\n" +
			"You are connected to ";
	
	private static final String WELCOME_MSG_END = "\nPlease use \"help\" for a list of commands.\n";
	
	private static final String HELP_MSG = "\n==========\nCommands\n==========\n\n" +
			"INSERT <id> <data>\t\tInsert data into specified id\n" +
			"REMOVE <id>\t\t\tRemove data at specified id\n" +
			"LOOKUP <id>\t\t\tLookup data at specified id\n" +
			"EXPORT\t\t\t\tExport relative timing statistics\n" +
			"HELP\t\t\t\tBring up this menu\n" +
			"EXIT\t\t\t\tClose connection to server and exit application.\n\n";
	
	private static final String INVALID_REQ = "Invalid request. Try \"help\" for more information";

	/**
	 * Constructor
	 * 
	 * @param host		Host to connect to
	 * @param portStr	Port to connect to
	 * @throws IOException
	 */
	public Client(String host, String portStr, AnalysisRun an) throws IOException {
		int port = Integer.parseInt(portStr);
		InetAddress addr = InetAddress.getByName(host);
		conn   = new Socket(addr, port);
		reader = new BufferedReader(new InputStreamReader(conn.getInputStream()));
		writer = new PrintWriter(conn.getOutputStream(), true);
		insertTime = new Vector<Long>();
		lookupTime = new Vector<Long>();
		analysis = an;
	}
	
	/**
	 * Begins the server to allow for transactions, etc.
	 */
	public void run()
	{
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
		String inLine = null;
		
		System.out.println(WELCOME_MSG_BEGIN+conn.getInetAddress().toString()+":"+conn.getPort()+WELCOME_MSG_END);
		
		if(analysis.runAnalysis)
			performAnalysis();
		
		while(!close) {
			try {
				inLine = in.readLine();
			} catch (IOException e) {
				// Ignore
				continue;
			}
			
			if(inLine == null) {
				System.err.println("Unrecoverable error reading input.");
				break;
			}
			
			inLine = inLine.trim();
			
			if(inLine.equalsIgnoreCase("exit")) {
				writer.println(Protocol.EXIT_REQ.getStringCode());
				break;
			}
			
			processInputLine(inLine);
		}
		
		System.out.println("Closing connection...");
		try {
			conn.close();
		} catch (IOException e) {
			// Ignore, exiting
		}
		System.out.println("Exiting client...");
	}
	
	/**
	 * Process an input line from client
	 * 
	 * @param line		The input line
	 */
	private void processInputLine(String line)
	{
		if(line == null)
			return;
		
		if(line.equalsIgnoreCase("help")) { // Help
			System.out.println(HELP_MSG);
		} else if(line.startsWith("insert")) { // Handle insert
			insert(line.split(" ", 3));
		} else if(line.startsWith("remove")) { // Handle remove
			remove(line.split(" ", 2));
		} else if(line.startsWith("lookup")) { // Handle lookup
			lookup(line.split(" ", 2));	
		} else if(line.startsWith("export")) {
			export();
		} else {
			System.out.println(INVALID_REQ);
		}
	}
	
	/**
	 * Insert a value
	 * 
	 * @param vals	A string array with id at position 1 and data at position 2
	 */
	private void insert(String[] vals)
	{
		if(vals == null || vals.length != 3) {
			System.err.println(INVALID_REQ);
			return;
		}
		String resp = null;
		String id   = vals[1];
		String data = vals[2];
		System.out.println("Inserting data at id "+id+": "+data);
		
		// Send request to server
		long start = System.currentTimeMillis();
		writer.println(Protocol.INSERT_REQ.getStringCode()+":"+id+":"+data);
		try {
			resp = reader.readLine();
		} catch (IOException e) {
			System.out.println("Insertion failed (exception caught): "+e.toString());
			return;
		}
		
		if(resp == null) {
			System.out.println("Connection closed by host.");
			close = true;
			return;
		}
		long end  = System.currentTimeMillis();
		long time = end - start; 
		
		insertTime.add(time);
		
		if(resp.startsWith(Protocol.INSERT_SUC.getStringCode())) {
			System.out.println("Id "+id+" inserted successfully in "+time+"ms");
		} else if(resp.startsWith(Protocol.INSERT_FAIL.getStringCode())) {
			System.out.println("Insertion of id "+id+" failed in "+time+"ms");
		}
	}
	
	/**
	 * Remove a value
	 * 
	 * @param vals	A string array with id at position 1
	 */
	private void remove(String[] vals)
	{
		if(vals == null || vals.length != 2) {
			System.err.println(INVALID_REQ);
			return;
		}
		String id = vals[1];
		String resp = null;
		System.out.println("Removing "+id);
		
		writer.println(Protocol.REMOVE_REQ.getStringCode()+":"+id);
		try {
			resp = reader.readLine();
		} catch (IOException e) {
			System.out.println("Removal failed (exception caught): "+e.toString());
			return;
		}
		
		if(resp == null) {
			System.out.println("Connection closed by host.");
			close = true;
			return;
		}
		
		if(resp.startsWith(Protocol.REMOVE_SUC.getStringCode())) {
			System.out.println("Id "+id+" removed successfully!");
		} else if(resp.startsWith(Protocol.REMOVE_FAIL.getStringCode())) {
			System.out.println("Removal of id "+id+" failed");
		}
	}
	
	/**
	 * Lookup a value at an id
	 * 
	 * @param vals	A string array with id at position 1
	 */
	private void lookup(String[] vals)
	{
		if(vals == null || vals.length != 2) {
			System.err.println(INVALID_REQ);
			return;
		}
		String id = vals[1];
		String resp = null;
		
		long start = System.currentTimeMillis();
		writer.println(Protocol.LOOKUP_REQ.getStringCode()+":"+id);
		try {
			resp = reader.readLine();
		} catch (IOException e) {
			System.out.println("Lookup failed (exception caught): "+e.toString());
			return;
		}
		long end  = System.currentTimeMillis();
		long time = end - start;
		
		lookupTime.add(time);
		
		if(resp == null) {
			System.out.println("Connection closed by host.");
			close = true;
			return;
		}
		
		if(resp.startsWith(Protocol.LOOKUP_SUC.getStringCode())) {
			String[] data = resp.split(":", 2);
			if(data.length != 2) {
				System.out.println("Unexpected error interpreting lookup data");
				return;
			}
			System.out.println("Lookup for id "+id+" succeeded ("+time+"ms): "+data[1]);
		} else if(resp.startsWith(Protocol.LOOKUP_FAIL.getStringCode())) {
			System.out.println("Lookup for id"+id+" failed ("+time+"ms)");
		} else if(resp.startsWith(Protocol.LOOKUP_NOEXIST.getStringCode())) {
			System.out.println("Id "+id+" does not exist on this network ("+time+"ms)");
		}
	}
	
	/**
	 * Export analysis data in CSV format
	 */
	private void export()
	{
		String name = "analysis_"+System.currentTimeMillis()+".csv";
		File input = new File(name);
		BufferedWriter file = null;
		try {
			file = new BufferedWriter(new FileWriter(input));
			file.write("InsertTime,LookupTime\n");
			int numEntries = ((insertTime.size() > lookupTime.size()) ? insertTime.size() : lookupTime.size());
			for(int i = 0 ; i < numEntries ; ++i) {
				if(i < insertTime.size())
					file.write(insertTime.get(i).toString());
				else
					file.write("NaN");
				file.write(",");
				if(i < lookupTime.size())
					file.write(lookupTime.get(i).toString());
				else
					file.write("NaN");
				file.write("\n");
			}
			file.close();
		} catch (IOException e) {
			System.err.println("Could not open writer to export analysis data.");
			return;
		}
		System.out.println("Exported "+name+" successfully!");
	}
	
	/**
	 * Perform analysis operation
	 */
	private void performAnalysis()
	{
		int numReqs = analysis.numRequests;
		int numKeys = analysis.numKeys;
		System.out.println("Performing Analysis...");
		
		System.out.println("Starting "+numReqs+" random inserts");
		for(int i = 0 ; i < numReqs ; ++i) {
			int num = (int)(Math.random() * numKeys);
			processInputLine("insert "+num+" random_insert");
		}
		
		System.out.println("Starting "+numReqs+" random lookups");
		for(int i = 0 ; i < numReqs ; ++i) {
			int num = (int)(Math.random() * numKeys);
			processInputLine("lookup "+num);
		}
		
		export();
	}
}
