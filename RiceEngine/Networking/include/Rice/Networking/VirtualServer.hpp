#include "stdafx.hpp"
#include <thread>

NSP_NET_BEGIN

class VirtualServer;

NSP_NET_END

#pragma once

#include "Rice/Engine/Engine.hpp"
#include "Rice/Scene/Scene.hpp"
#include "NetProtocol.hpp"
#include "IServer.hpp"

NSP_NET_BEGIN

class VirtualServer : public IServer {
  public:
    struct IManager {
      private:
        friend class VirtualServer;
        ptr<VirtualServer> server;
        ptr<Scene> scene;

      protected:
        ptr<VirtualServer> getServer();
        ptr<Scene> getScene();
        virtual Response onJoin(ClientInfo client, Request request);
        virtual void onLeave(ClientInfo cleint);
    };

    VirtualServer(ptr<IManager> manager);

    void setScene(ptr<Scene> scene);

    Response response(Request request) override;

  private:
    std::jthread server_thread;

    ptr<IManager> manager;
    ptr<Scene> scene;
    Util::RegisterCollection<ClientInfo> clients;
};

NSP_NET_END