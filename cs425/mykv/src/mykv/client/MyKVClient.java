/**
 * MyKVClient.java
 * 
 * Main client implementation
 */
package mykv.client;

import java.io.IOException;

public class MyKVClient {
	
	/**
	 * Entry point for MyKV client
	 * 
	 * @param args	Arguments to initialize server
	 */
	public static void main(String[] args) {
		if(args.length != 2 && args.length != 4) {
			System.err.println("Usage: ./program <host_ip> <host_port> [<number_of_random_inserts_and_lookups> <num_keys>]");
			return;
		}
		
		Client client = null;
		AnalysisRun analysis = new AnalysisRun();
		
		if(args.length == 4) {
			analysis.runAnalysis = true;
			analysis.numRequests = Integer.parseInt(args[2]);
			analysis.numKeys     = Integer.parseInt(args[3]);
		}
		
		try {
			client = new Client(args[0], args[1], analysis);
		} catch (IOException e) {
			System.err.println("Could not connect to server ("+args[0]+":"+args[1]+")");
			return;
		}
		
		client.run();
	}
}
