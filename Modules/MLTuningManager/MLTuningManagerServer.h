#ifndef PARAMETER_PERFORMANCE_MANAGER_SERVER_H
#define PARAMETER_PERFORMANCE_MANAGER_SERVER_H

#include "Server.h"

class MLTuningManagerServer : public Server {
public:
    using Server::Server; // Inherit constructors

protected:
    void HandleRequest(tcp::socket socket) override;
};

#endif // PARAMETER_PERFORMANCE_MANAGER_SERVER_H
