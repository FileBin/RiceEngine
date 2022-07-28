#include "../stdafx.hpp"

NSP_NET_BEGIN

struct IServer;
typedef RefPtr<IServer> pIServer;

NSP_NET_END

#pragma once

#include "NetProtocol.hpp"


NSP_NET_BEGIN

struct IServer {
    virtual Response response(Request request);
};

NSP_NET_END