#include "../stdafx.hpp"

NSP_NET_BEGIN

struct Request;
struct Response;
typedef RefPtr<Request> pRequest;
typedef RefPtr<Request> pResponse;

NSP_NET_END

#pragma once

#include "../Engine/Engine.hpp"
#include "../Scene/Scene.hpp"

NSP_NET_BEGIN

#undef GetObject

struct ConnectionKey {
    uint64_t connection_id = 0;
    uint64_t hash = 0;
};

struct ClientInfo {
    String clientVersion;
    String nickname;
    String email; // for join from other device or account recovery
};

struct ObjectSendData {
    UUID objectUUID;
    struct TickData {
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
    } tick_data, prev_tick_data;
};


struct PlayerData {
    ObjectSendData obj_data;
    dbl renderDistance;
};

typedef RefPtr<ConnectionKey> pConnectionKey;

class Request {
  public:
    enum Type {
        JOIN,
        GET_SCENE_STATE,
        GET_OBJECT,
        SEND_PLAYER_DATA,
    };

    struct GetObject {
        UUID uuid;
    };

  private:
    Request() {}
    Request(Type type, ConnectionKey key, ClientInfo info, void *data)
        : type(type), key(key), data(data), info(info) {}
    void init(Type ty) {}

  public:
    ~Request() { free(data); }

    static Request joinRequest(ClientInfo info) {
        return {JOIN, {}, info, nullptr};
    };

    static Request getSceneState(ConnectionKey key, ClientInfo info) {
        return {GET_SCENE_STATE, key, info, nullptr};
    };

    static Request getObject(ConnectionKey key, ClientInfo info,
                             UUID objectUUID) {
        return {GET_SCENE_STATE, key, info, new GetObject({objectUUID})};
    };

    static Request sendPlayerData(ConnectionKey key, ClientInfo info,
                                  PlayerData data) {
        return {SEND_PLAYER_DATA, key, info, new PlayerData({data})};
    };

    typedef RefPtr<ClientInfo> pClientInfo;

  private:
    Type type;
    void *data;
    ConnectionKey key;
    ClientInfo info;
};

class Response {
  public:
    enum Type {
        JOIN_ACCEPT,
        JOIN_REFUSE,
        SEND_OBJECT,
        SEND_SCENE_STATE,
        SEND_ERROR,
        SEND_OK,
    };

  private:
    template <typename T> void init(Type ty, T obj) {
        type = ty;
        data = malloc(sizeof(T));
        memcpy(data, (void *)&obj, sizeof(T));
    }

  public:
    ~Response() { free(data); };

    struct JoinAccept {
        ConnectionKey key;
        PlayerData initial_data;
    };

    RefPtr<JoinAccept> getAcceptData() {
        if (type == JOIN_ACCEPT)
            return RefPtr<JoinAccept>(data_copy<JoinAccept>());
        return nullptr;
    }

    struct JoinRefuse {
        String msg;
    };

    RefPtr<JoinRefuse> getRefuseData() {
        if (type == JOIN_REFUSE)
            return RefPtr<JoinAccept>(data_copy<JoinAccept>());
        return nullptr;
    }

    struct SendObject {
        ObjectData object_data;
    };

    RefPtr<SendObject> getObject() {
        if (type == SEND_OBJECT)
            return RefPtr<SendObject>(data_copy<SendObject>());
        return nullptr;
    }

    struct SendSceneState {
        num server_tick;
        vec<ObjectSendData> objects_state;
    };

    RefPtr<SendSceneState> getSceneState() {
        if (type == SEND_SCENE_STATE)
            return RefPtr<SendSceneState>(data_copy<SendSceneState>());
        return nullptr;
    }

    struct SendError {
        String msg;
    };

    Response(JoinAccept data) { init<JoinAccept>(JOIN_ACCEPT, data); }
    Response(JoinRefuse data) { init<JoinRefuse>(JOIN_REFUSE, data); }
    Response(SendObject data) { init<SendObject>(SEND_OBJECT, data); }
    Response(SendSceneState data) {
        init<SendSceneState>(SEND_SCENE_STATE, data);
    }
    Response(SendError data) { init<SendError>(SEND_ERROR, data); }

    bool is(Type ty) { return type == ty; };

  private:
    Type type;
    void *data;

    template <typename T> T *data_copy() {
        T *copy;
        memcpy((void *)copy, data, sizeof(T));
        return copy;
    };
};

NSP_NET_END