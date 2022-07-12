#include "../stdafx.hpp"

NSP_NET_BEGIN

class VirtualServer;
typedef RefPtr<VirtualServer> pVirtualServer;

NSP_NET_END

#pragma once

#include "../Engine/Engine.hpp"
#include "../Scene/Scene.hpp"
#include "NetProtocol.hpp"
#include "IServer.hpp"

NSP_NET_BEGIN

class VirtualServer : public IServer {
  public:
    struct IManager {
      private:
        friend class VirtualServer;
        pVirtualServer server;
        pScene scene;

      protected:
        pVirtualServer getServer();
        pScene getScene();
        virtual Response onJoin(ClientInfo client, Request request);
        virtual void onLeave(ClientInfo cleint);
    };

    typedef RefPtr<IManager> pIManager;

    VirtualServer(pIManager manager);

    void setScene(pScene scene);

    Response response(Request request) override;

  private:
    pIManager manager;
    pScene scene;
    RegisterCollection<ClientInfo> clients;
};

NSP_NET_END