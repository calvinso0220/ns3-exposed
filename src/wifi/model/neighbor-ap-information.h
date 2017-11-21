#ifndef NEIGHBOR_AP_INFORMATION_H
#define NEIGHBOR_AP_INFORMATION_H

#include <stdint.h>
#include <ostream>
#include "ns3/buffer.h"
#include "ns3/wifi-information-element.h"
#include "ns3/mac48-address.h"

namespace ns3 {

class NeighborAPInformation : public WifiInformationElement {
public:
	NeighborAPInformation();
	NeighborAPInformation& operator= (const NeighborAPInformation&);

	static const uint8_t MAX_NEIGHBOR_APS = 32;
	void AddNeighborAP(Mac48Address addr);
	uint8_t GetNAPs(void) const;
	
	WifiInformationElementId ElementId () const;
	uint8_t GetInformationFieldSize () const;
	void SerializeInformationField(Buffer::Iterator i) const;
	uint8_t DeserializeInformationField(Buffer::Iterator i, uint8_t length);

//private:
	uint8_t m_nAPs;
	Mac48Address m_APs[MAX_NEIGHBOR_APS];
};

// not implemented yet
//std::ostream &operator << (std::ostream &os, const NeighborAPInformation &aps);

}

#endif
