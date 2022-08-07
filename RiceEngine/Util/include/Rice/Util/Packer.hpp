#include "stdafx.hpp"

#include "ByteStream.hpp"
#include "Interfaces.hpp"
#include <functional>
#include <type_traits>

NSP_UTIL_BEGIN

template <typename T> class BinaryPacker : public IPacker {
  private:
    std::function<T()> getter;
    std::function<void(T)> setter;

  public:
    BinaryPacker(std::function<T()> getter, std::function<void(T)> setter)
        : getter(getter), setter(setter) {}
    virtual ~BinaryPacker() = default;
    void pack(ByteStream &stream) override {
        constexpr auto l = sizeof(T);
        data_t data(l);
        T value = getter();
        memcpy(data.data(), &value, l);
        stream.write(data);
    }

    void unpack(ByteStream &stream) override {
        constexpr auto l = sizeof(T);
        data_t data = stream.read(l);
        T value;
        memcpy(&value, data.data(), l);
        setter(value);
    }
};
// TODO make VectorPacker
// TODO make UniquePtrPacker

class CustomPacker : public IPacker {
  private:
    std::function<void(ByteStream &)> packerFunc;
    std::function<void(ByteStream &)> unpackerFunc;

  public:
    CustomPacker(std::function<void(ByteStream &)> packerFunc,
                 std::function<void(ByteStream &)> unpackerFunc)
        : packerFunc(packerFunc), unpackerFunc(unpackerFunc) {}

    void pack(ByteStream &stream) override { packerFunc(stream); }

    void unpack(ByteStream &stream) override { unpackerFunc(stream); }
};

class PackerChain : public IPacker {
  private:
    vec<ptr<IPacker>> packers;

  public:
    void pack(ByteStream &stream) override {
        for (auto p : packers) {
            p->pack(stream);
        }
    }

    void unpack(ByteStream &stream) override {
        for (auto p : packers) {
            p->unpack(stream);
        }
    }
    PackerChain(vec<ptr<IPacker>> packers) : packers(packers) {}
};

NSP_UTIL_END