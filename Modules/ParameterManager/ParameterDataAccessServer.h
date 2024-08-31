#ifndef PARAMETER_DATA_ACCESS_SERVER_H
#define PARAMETER_DATA_ACCESS_SERVER_H

#include "Server.h"

class ParameterDataAccessServer : public Server {
public:
    using Server::Server; // Inherit constructors

protected:
    void HandleRequest(tcp::socket socket) override;
};

#endif // PARAMETER_DATA_ACCESS_SERVER_H