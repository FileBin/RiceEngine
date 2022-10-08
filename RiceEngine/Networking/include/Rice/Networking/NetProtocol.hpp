#include "stdafx.hpp"

NSP_NET_BEGIN

struct Request;
struct Response;

NSP_NET_END

#pragma once

#include "Rice/Engine/EngineBase.hpp"
#include "Rice/Scene/Object.hpp"
#include "Rice/Scene/SceneBase.hpp"
NSP_NET_BEGIN

#undef GetObject

struct ConnectionKey {
    byte hash[64] = {0}; // 512 bit secure hash

    std::string toStdString() {
        char buf[] = "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000"
                     "00000000";
        short buf_pos = 0x7f;
        for (uint i = 0; i < 64; i++) {
            byte b = hash[i];
            buf[buf_pos--] = '0' + b % 0x10;
            buf[buf_pos--] = '0' + b / 0x10;
        }
        return buf;
    }

    static ConnectionKey fromStdString(std::string str) {
        ConnectionKey key;
        short buf_pos = 0x7f;
        for (uint i = 0; i < 64; i++) {
            key.hash[i] = str[buf_pos--] - '0';
            key.hash[i] += (str[buf_pos--] - '0') * 0x10;
        }
        return key;
    }
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

class Request {
  public:
    enum Type {
        JOIN,
        GET_SCENE_STATE,
        GET_OBJECT,
        GET_SERVER_UUID,
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

    static Request getServerUUID() {
        return {GET_SERVER_UUID, {}, {}, nullptr};
    };

    static Request joinRequest(ClientInfo info, ConnectionKey key = {}) {
        return {JOIN, key, info, nullptr};
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
        SEND_UUID,
    };

  private:
    template <typename T> void init(Type ty, T obj) {
        type = ty;
        data = malloc(sizeof(T));
        data_len = sizeof(T);
        memcpy(data, (void *)&obj, sizeof(T));
    }

  public:
    Response(const Response &other)
        : type(other.type), data(other.data_copy()), data_len(other.data_len) {}

    const Response &operator=(const Response &other) {
        if (data)
            free(data);
        data = other.data_copy();
        data_len = other.data_len;
        type = other.type;
        return *this;
    }

    ~Response() {
        if (data)
            free(data);
    };

    struct JoinAccept {
        ConnectionKey key;
        PlayerData initial_data;
    };

    ptr<JoinAccept> asAccept() {
        if (type == JOIN_ACCEPT)
            return ptr<JoinAccept>(data_copyT<JoinAccept>());
        return nullptr;
    }

    struct JoinRefuse {
        String msg;
    };

    ptr<JoinRefuse> asRefuse() {
        if (type == JOIN_REFUSE)
            return ptr<JoinRefuse>(data_copyT<JoinRefuse>());
        return nullptr;
    }

    struct SendObject {
        // ObjectData object_data;
    };

    ptr<SendObject> asObjectData() {
        if (type == SEND_OBJECT)
            return ptr<SendObject>(data_copyT<SendObject>());
        return nullptr;
    }

    struct SendSceneState {
        num server_tick;
        vec<ObjectSendData> objects_state;
    };

    ptr<SendSceneState> asSceneState() {
        if (type == SEND_SCENE_STATE)
            return ptr<SendSceneState>(data_copyT<SendSceneState>());
        return nullptr;
    }

    ptr<UUID> asUUID() {
        if (type == SEND_UUID)
            return ptr<UUID>(data_copyT<UUID>());
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

    Response(UUID data) { init<UUID>(SEND_UUID, data); }

    bool is(Type ty) { return type == ty; };

  private:
    Type type;
    uint data_len;
    void *data;

    template <typename T> T *data_copyT() {
        T *copy;
        memcpy((void *)copy, data, sizeof(T));
        return copy;
    };

    void *data_copy() const {
        void *copy;
        memcpy((void *)copy, data, data_len);
        return copy;
    };
};

NSP_NET_END