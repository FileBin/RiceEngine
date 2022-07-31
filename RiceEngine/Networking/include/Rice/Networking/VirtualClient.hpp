#include "stdafx.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <stop_token>
#include <thread>

NSP_NET_BEGIN

class VirtualClient;

NSP_NET_END

#pragma once

#include "Rice/Engine/Engine.hpp"
#include "Rice/Scene/Scene.hpp"
#include "IServer.hpp"
#include "NetProtocol.hpp"

NSP_NET_BEGIN

class VirtualClient {
  public:
    struct JoinResult {
        bool accepted;
        String msg;
    };
  private:
    ptr<Scene> scene;
    ptr<IServer> server;
    PlayerData player_data;
    ClientInfo info;
    ConnectionKey key; //TODO: add local key storage
    
    static std::filesystem::path config_path;
    static std::filesystem::path config_file;

    std::filesystem::path key_storage_path;

    num client_tick = 0;

    std::chrono::high_resolution_clock::time_point client_clock;

    std::chrono::high_resolution_clock::duration tick_duration;

    void clientThreadFn(std::stop_token stoken);

    std::jthread client_thread;
    std::mutex client_mutex;

    dbl getTickOffset();

    void updateClock(num new_tick);

  public:
    VirtualClient(ClientInfo info, std::chrono::high_resolution_clock::duration update_rate = std::chrono::milliseconds(20));
    ~VirtualClient();
    JoinResult join(ptr<IServer> server);
    void leave();
    private:
    bool tryGetKey(UUID servUUID, ConnectionKey &out_key);
};

NSP_NET_END