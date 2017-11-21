#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimulationScript");

// global variables
NetDeviceContainer staDevice;


static void PrintStatus(void) {
	for(unsigned i=0; i<staDevice.GetN(); i++) {
		Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(staDevice.Get(i));
		Ptr<StaWifiMac> mac = DynamicCast<StaWifiMac>(device->GetMac());
		mac->PrintNeighborStatus();	
	}
}

int main(int argc, char *argv[]) {

	// parameters
	uint16_t num_sta_nodes = 2;
	uint16_t num_ap_nodes = 2;

	double begin_time = 60.0;
	double sim_time = 6.0;

	uint32_t payloadSize = 1472;

	// command line arguments
	CommandLine cmd;
    cmd.Parse (argc,argv);

	// nodes
	NodeContainer wifiStaNodes;
	wifiStaNodes.Create(num_sta_nodes);
	NodeContainer wifiApNodes;
	wifiApNodes.Create(num_ap_nodes);

	// PHY + channel
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue(20));
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    phy.SetChannel (channel.Create ());
	phy.Set("ShortGuardEnabled", BooleanValue(1));
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));
    phy.Set("CcaMode1Threshold", DoubleValue(-82.0));
    phy.Set("EnergyDetectionThreshold", DoubleValue(-82.0));

	// MAC
	WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
    WifiMacHelper mac;
    std::string phyMode("OfdmRate54Mbps");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyMode), "ControlMode", StringValue(phyMode));

	Ssid ssid = Ssid("jungmin");
	mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
	NetDeviceContainer apDevice;
	apDevice = wifi.Install(phy, mac, wifiApNodes);

	mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
	//NetDeviceContainer staDevice;		// moved to global variables
	staDevice = wifi.Install(phy, mac, wifiStaNodes);

	// Node position
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add(Vector(0.0, 0.0, 0.0));
	positionAlloc->Add(Vector(120.0, 0.0, 0.0));
	positionAlloc->Add(Vector(30.0, 0.0, 0.0));
	positionAlloc->Add(Vector(90.0, 0.0, 0.0));

	mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNodes);
    mobility.Install(wifiStaNodes);

	// Internet stack 
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;

    address.SetBase ("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staNodeInterface;
    Ipv4InterfaceContainer apNodeInterface;

    staNodeInterface = address.Assign (staDevice);
    apNodeInterface = address.Assign (apDevice);

	// Applications
	ApplicationContainer serverAppA, clientAppA;
	UdpServerHelper myServerA(9);	// Port number
	serverAppA = myServerA.Install(wifiApNodes.Get(0));
	serverAppA.Start(Seconds(begin_time));
	serverAppA.Stop(Seconds(begin_time + sim_time));

	UdpClientHelper myClientA(apNodeInterface.GetAddress(0), 9);
    myClientA.SetAttribute("MaxPackets", UintegerValue (4294967295u));
    myClientA.SetAttribute("Interval", TimeValue (Seconds(0.0001))); //packets/s
    myClientA.SetAttribute("PacketSize", UintegerValue (payloadSize));
	clientAppA = myClientA.Install(wifiStaNodes.Get(0));
	clientAppA.Start(Seconds(begin_time + 0.1));
	clientAppA.Stop(Seconds(begin_time + 0.1 + sim_time));

	ApplicationContainer serverAppB, clientAppB;
	UdpServerHelper myServerB(9);	// Port number
	serverAppB = myServerB.Install(wifiApNodes.Get(1));
	serverAppB.Start(Seconds(begin_time));
	serverAppB.Stop(Seconds(begin_time + sim_time));

	UdpClientHelper myClientB(apNodeInterface.GetAddress(1), 9);
    myClientB.SetAttribute("MaxPackets", UintegerValue (4294967295u));
    myClientB.SetAttribute("Interval", TimeValue (Seconds(0.0001))); //packets/s
    myClientB.SetAttribute("PacketSize", UintegerValue (payloadSize));
	clientAppB = myClientB.Install(wifiStaNodes.Get(1));
	clientAppB.Start(Seconds(begin_time + 0.1));
	clientAppB.Stop(Seconds(begin_time + 0.1 + sim_time));

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	// Predefined Schedules
	Simulator::Schedule(Seconds(30.0), &PrintStatus);

	// Run simulation
	Simulator::Stop(Seconds(begin_time + sim_time + 1.0));
    Simulator::Run();
    Simulator::Destroy();

	// Throughput calculation
	double throughputA = 0;
	uint32_t totalPacketsThroughA = DynamicCast<UdpServer>(serverAppA.Get(0))->GetReceived();
	throughputA = totalPacketsThroughA * payloadSize * 8 / (sim_time * 1000000.0);	// Mbps

	double throughputB = 0;
	uint32_t totalPacketsThroughB = DynamicCast<UdpServer>(serverAppB.Get(0))->GetReceived();
	throughputB = totalPacketsThroughB * payloadSize * 8 / (sim_time * 1000000.0);	// Mbps

	NS_LOG_UNCOND("throughputA: " << throughputA << " Mbps");
	NS_LOG_UNCOND("throughputB: " << throughputB << " Mbps");
	return 0;
}



