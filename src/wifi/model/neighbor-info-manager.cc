#include "neighbor-info-manager.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("NeighborInfoManager");
NS_OBJECT_ENSURE_REGISTERED(NeighborInfoManager);

TypeId NeighborInfoManager::GetTypeId(void) {
	static TypeId tid = TypeId("ns3::NeighborInfoManager")
		.SetParent<Object>()
		.SetGroupName("Wifi")
		.AddConstructor<NeighborInfoManager>()
	;
	return tid;
}

NeighborInfoManager::NeighborInfoManager() {
	NS_LOG_FUNCTION(this);
}

NeighborInfoManager::~NeighborInfoManager() { }

void NeighborInfoManager::Initialize(Mac48Address myaddr) {
	m_myinfo.addr = myaddr;
	m_myinfo.assoc_ap = Mac48Address::GetBroadcast();
	m_myinfo.neighbor_aps.clear();
}

void NeighborInfoManager::DoDispose(void) {
	m_neighbors.clear();
}
	
void NeighborInfoManager::BeaconReceived(Mac48Address bsaddr) {

	bool found = false;
	for(unsigned i=0; i<m_myinfo.neighbor_aps.size(); i++) {
		if(m_myinfo.neighbor_aps[i] == bsaddr) {
			found = true;
			break;
		}
	}
	
	if(!found) {
		m_myinfo.neighbor_aps.push_back(bsaddr);	
	}
}

void NeighborInfoManager::Associated(Mac48Address bsaddr) {
	m_myinfo.assoc_ap = bsaddr;
}

void NeighborInfoManager::Deassociated() {
	m_myinfo.assoc_ap = Mac48Address::GetBroadcast();
}

NeighborAPInformation NeighborInfoManager::GetNeighborAPInformation(void) {

	NeighborAPInformation info;
	for(unsigned i=0; i<m_myinfo.neighbor_aps.size(); i++) {
		info.AddNeighborAP(m_myinfo.neighbor_aps[i]);
	}

	return info;
}

void NeighborInfoManager::PrintTable() {

	NS_LOG_UNCOND("my address: " << m_myinfo.addr);
	NS_LOG_UNCOND("my AP:      " << m_myinfo.assoc_ap);
	
	NS_LOG_UNCOND("neighboring APs");
	for(unsigned i=0; i<m_myinfo.neighbor_aps.size(); i++) {
		NS_LOG_UNCOND(m_myinfo.neighbor_aps[i]);
	}
}








} // namespace ns3
