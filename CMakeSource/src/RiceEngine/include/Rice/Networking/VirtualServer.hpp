#include "../stdafx.hpp"

NSP_NET_BEGIN

class VirtualServer;
typedef RefPtr<VirtualServer> pVirtualServer;

NSP_NET_END

#pragma once

#include "../Engine/Engine.hpp"
#include "../Scene/Scene.hpp"
#include "NetProtocol.hpp"
#include "VirtualClient.hpp"

NSP_NET_BEGIN

class VirtualServer {
  public:
    struct IManager {
      private:
        friend class VirtualServer;
        pVirtualServer server;
        pScene scene;

      protected:
        pVirtualServer getServer();
        pScene getScene();
        virtual Response onJoin(pVirtualClient client, Request request);
        virtual void onLeave(pVirtualClient cleint);
    };

    typedef RefPtr<IManager> pIManager;

    VirtualServer(pIManager manager);

    void setScene(pScene scene);

    Response getResponse(Request request);

  private:
    pIManager manager;
    pScene scene;
    RegisterCollection<pVirtualClient> clients;
};

NSP_NET_END