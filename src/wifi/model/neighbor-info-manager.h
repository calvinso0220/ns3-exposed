#ifndef NEIGHBOR_INFO_MANAGER_H
#define NEIGHBOR_INFO_MANAGER_H

#include <vector>
#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "neighbor-ap-information.h"


namespace ns3 {

struct NeighborInfo {
	Mac48Address addr;
	Mac48Address assoc_ap;
	std::vector<Mac48Address> neighbor_aps;
};

class NeighborInfoManager : public Object {
public:
	static TypeId GetTypeId(void);

	NeighborInfoManager();
	virtual ~NeighborInfoManager();

	void Initialize(Mac48Address myaddr);
	void BeaconReceived(Mac48Address bsaddr);
	void Associated(Mac48Address bsaddr);
	void Deassociated();
	void PrintTable();
	NeighborAPInformation GetNeighborAPInformation(void);

protected:
	virtual void DoDispose(void);

private:
	NeighborInfo m_myinfo;
	std::vector<NeighborInfo> m_neighbors;
};




} // namespace ns3

#endif
