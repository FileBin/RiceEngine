#include "../stdafx.hpp"

NSP_NET_BEGIN

class VirtualClient;
typedef RefPtr<VirtualClient> pVirtualClient;

NSP_NET_END

#pragma once

#include "../Engine/Engine.hpp"
#include "../Scene/Scene.hpp"
#include "VirtualServer.hpp"

NSP_NET_BEGIN

class VirtualClient {
  private:
    pScene scene;
    pVirtualServer server;

  public:
    void join(pVirtualServer server);
};

NSP_NET_END