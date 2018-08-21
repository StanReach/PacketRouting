package Routing;

import java.io.*;
import java.util.Scanner;

class DataPacket {
	String source;
	String destination;
	String contents;
}

class RoutingPacket {
	String source;
	int receptionDelay;
	int numberOfEntries;
	
	String destination;
	int delay;
}

class RoutingTable {
	int[][] vectorTable = new int[3][20];
	
	String[] neighbourNames = new String[20];
	String[] destinationNames = new String[20];
			
	boolean addNeighbour(String name) {
		for (int i = 0; i < neighbourNames.length; i++) {
			if (neighbourNames[i] == null) {
				neighbourNames[i] = name;
				return true;
			}
		}
		return false;
	}
	
	int findNeighbourName(String name) {
		for (int i = 0; i < neighbourNames.length; i++) {
			if (neighbourNames[i] != null && neighbourNames[i].equals(name)) {
				return i;
			}
		}
		return -1;
	}
	
	void addDestination(String name) {
		for (int i = 0; i < destinationNames.length; i++) {
			if (destinationNames[i] == null) {
				destinationNames[i] = name;
				break;
			}
		}
	}
	
	int findDestinationName(String name) {
		for (int i = 0; i < destinationNames.length; i++) {
			if (destinationNames[i] != null && destinationNames[i].equals(name)) {
				return i;
			}
		}
		return -1;
	}
	
	void updateTable(String neighbourName, String destinationName, int delay) {
		vectorTable[findNeighbourName(neighbourName)][findDestinationName(destinationName)] = delay;
	}
	
	int returnDelayFromTable(String neighbourName, String destinationName) {
		return vectorTable[findNeighbourName(neighbourName)][findDestinationName(destinationName)];
	}
	
	void updateDelayEstimates(String personalAddress, String neighbourToUpdate) {		
		
		int delayToNeighbour = returnDelayFromTable(neighbourToUpdate, personalAddress);
		
			for (int x = 0; x < destinationNames.length; x++) {
				if (destinationNames[x] != null && !destinationNames[x].equals(personalAddress)) {
						updateTable(neighbourToUpdate, destinationNames[x], returnDelayFromTable(neighbourToUpdate, destinationNames[x]) + delayToNeighbour);
				}
			}
		}
	}


class PacketRouting {
	
	public static String personalAddress;	
	
	public static final int ADDRESS_DOESNT_EXIST = -1;
	
	public static void main(String[] args) {
		
		Scanner networkPacketContents = new Scanner(System.in);
		
		personalAddress = networkPacketContents.next();
		
		int numberOfPackets = networkPacketContents.nextInt();
		
		readNetworkPacket(networkPacketContents, personalAddress);
	}
	
	private static void routeDataPackets(DataPacket dataPacket, Scanner networkPacketContents, RoutingTable vectorTable) {
		
		int delayTime;
		
		dataPacket.source = networkPacketContents.next();				
		dataPacket.destination = networkPacketContents.next();
		dataPacket.contents = networkPacketContents.next();
		
		if (vectorTable.findNeighbourName(dataPacket.destination) != ADDRESS_DOESNT_EXIST) {
			
			delayTime = vectorTable.returnDelayFromTable(dataPacket.destination, personalAddress);
			
			System.out.println(dataPacket.destination + " " + delayTime);
		} else if (vectorTable.findDestinationName(dataPacket.destination) == ADDRESS_DOESNT_EXIST) {
			
			System.out.println("NO ROUTE");
		} else {
			
			String nextNeighbour = findNeighbourWithQuickestRoute(vectorTable, dataPacket);
			
			delayTime = vectorTable.returnDelayFromTable(nextNeighbour, dataPacket.destination);
			
			System.out.println(nextNeighbour + " " + delayTime);
		}
	}
	
	private static String findNeighbourWithQuickestRoute(RoutingTable vectorTable, DataPacket dataPacket) {
		
		int bestDelay = 0;
		int delayTime;
		
		String nextNeighbour = null;
		
		for (int i = 0; i < vectorTable.neighbourNames.length; i++) {
			
			if (vectorTable.neighbourNames[i] != null) {
				
				bestDelay = vectorTable.returnDelayFromTable(vectorTable.neighbourNames[i], dataPacket.destination);
				nextNeighbour = vectorTable.neighbourNames[i];
				break;
			}
		}
		for (int i = 0; i < vectorTable.neighbourNames.length; i++) {
			if (vectorTable.neighbourNames[i] != null) {

				delayTime = vectorTable.returnDelayFromTable(vectorTable.neighbourNames[i], dataPacket.destination);
				
				if (delayTime < bestDelay) {
					
					bestDelay = delayTime;
					
					nextNeighbour = vectorTable.neighbourNames[i];
				}
			}
		}
		return nextNeighbour;
	}
	
	private static void addDelayToDestinationToTable(RoutingPacket routingPacket, Scanner networkPacketContents, RoutingTable vectorTable) {
		
		for (int i = 0; i < routingPacket.numberOfEntries; i++) {
			routingPacket.destination = networkPacketContents.next();
			routingPacket.delay = networkPacketContents.nextInt();
			
			if (vectorTable.findDestinationName(routingPacket.destination) == ADDRESS_DOESNT_EXIST) {

				vectorTable.addDestination(routingPacket.destination);
			}
			
			vectorTable.updateTable(routingPacket.source, routingPacket.destination, routingPacket.delay);
		}
	}
	
	private static void analyseRoutingPacket(RoutingPacket routingPacket, Scanner networkPacketContents, RoutingTable vectorTable) {
		
		routingPacket.source = networkPacketContents.next();
		routingPacket.receptionDelay = networkPacketContents.nextInt();
		routingPacket.numberOfEntries = networkPacketContents.nextInt();
		
		if (vectorTable.findNeighbourName(routingPacket.source) == ADDRESS_DOESNT_EXIST) {
			
			vectorTable.addNeighbour(routingPacket.source);
		}
		
		addDelayToDestinationToTable(routingPacket, networkPacketContents, vectorTable);
		
		vectorTable.updateDelayEstimates(personalAddress, routingPacket.source);
		
		System.out.println("THANK YOU");
	}
	
	private static void readNetworkPacket(Scanner networkPacketContents, String personalAddress) {
		
		DataPacket dataPacket = new DataPacket();					//create instances of the different packet types
		RoutingPacket routingPacket = new RoutingPacket();
		RoutingTable vectorTable = new RoutingTable();	
		
		String packetType;
		
		while (networkPacketContents.hasNext() == true) {
			
			packetType = networkPacketContents.next();
			
			if (packetType.equals("D")) {
				
				routeDataPackets(dataPacket, networkPacketContents, vectorTable);
				
			} else if (packetType.equals("R")) {
				
				analyseRoutingPacket(routingPacket, networkPacketContents, vectorTable);
				
			}
		}
	}
}

