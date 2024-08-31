#ifndef IPENDINGORDERFILTER_H
#define IPENDINGORDERFILTER_H

#include <vector>
#include "CommonTypes.h"

class IPendingOrderFilter {
public:
    virtual std::vector<PendingOrder> FilterPendingOrders(const std::vector<PendingOrder>& pendingOrders) = 0;
    virtual ~IPendingOrderFilter() = default;
};

#endif // IPENDINGORDERFILTER_H
