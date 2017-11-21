#include "neighbor-ap-information.h"
#include "ns3/log.h"
#include "ns3/address-utils.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("NeighborAPInformation");

NeighborAPInformation::NeighborAPInformation() {
	m_nAPs = 0;
}

NeighborAPInformation& NeighborAPInformation::operator= (const NeighborAPInformation& aps) {
	this->m_nAPs = aps.m_nAPs;
	memcpy(this->m_APs, aps.m_APs, 6*MAX_NEIGHBOR_APS);
	return (*this);
}

void NeighborAPInformation::AddNeighborAP(Mac48Address addr) {
	m_APs[m_nAPs] = addr;
	m_nAPs++;
}

uint8_t NeighborAPInformation::GetNAPs (void) const {
	return m_nAPs;
}

WifiInformationElementId NeighborAPInformation::ElementId () const {
	return IE_NEIGHBOR_AP_INFORMATION;
}

uint8_t NeighborAPInformation::GetInformationFieldSize () const {
	return 6 * m_nAPs;	
}

void NeighborAPInformation::SerializeInformationField(Buffer::Iterator i) const {
	for(unsigned j=0; j<m_nAPs; j++) {
		WriteTo(i, m_APs[j]);
	} 
}

uint8_t NeighborAPInformation::DeserializeInformationField(Buffer::Iterator i, uint8_t length) {
	m_nAPs = length / 6;
	for(unsigned j=0; j<m_nAPs; j++) {
		ReadFrom(i, m_APs[j]);
	}
	return m_nAPs;
}

}
