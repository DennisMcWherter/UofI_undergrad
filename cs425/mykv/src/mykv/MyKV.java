/**
 * MyKV.java
 * 
 * Entry point for MyKV server
 */
package mykv;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import mykv.server.Server;

public class MyKV {

	/**
	 * Entry point for MyKV
	 * 
	 * @param args	Arguments to initialize server
	 */
	public static void main(String[] args) {
		if(args.length != 7) {
			System.err.println("Usage: ./program <server_id> <num_servers> <bind_ip> <internal_port> <public_port> <connect_ip> <connect_port>");
			return;
		}
		
		int serverId     = Integer.parseInt(args[0]);
		int numServers   = Integer.parseInt(args[1]);
		InetAddress addr = null;
		int inPort       = Integer.parseInt(args[3]);
		int pubPort      = Integer.parseInt(args[4]);
		InetAddress conn = null;
		int connPort     = Integer.parseInt(args[6]);
		try {
			addr = InetAddress.getByName(args[2]);
			conn = InetAddress.getByName(args[5]);
		} catch (UnknownHostException e) {
			
		}
		
		Server server = null;
		try {
			server = new Server(serverId, numServers, addr, inPort, pubPort, conn, connPort);
		} catch (IOException e) {
			System.err.println("Could not initialize server correctly.");
			return;
		}
		
		System.out.println("Running server");
		server.runServer();
	}

}
