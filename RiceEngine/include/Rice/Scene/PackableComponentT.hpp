#include "Rice/Util/Exceptions/Exception.hpp"
#include "Rice/stdafx.hpp"

#include "PackableComponent.hpp"
#include "fmt/xchar.h"

NSP_COMPONENTS_BEGIN

template <typename T>
class PackableComponentT : virtual public PackableComponent {
    ComponentPacker &getPacker() override { return T::getPacker(); }
};

template <typename CompT> class CustomPacker;
template <typename CompT, typename FieldT> class FieldPacker;

template <typename CompT> class PackerChain : virtual public ComponentPacker {
  private:
    uptr<PackerChain<CompT>> next;

  public:
    void pack(ptr<PackableComponent> component, ByteStream stream) override {
        auto comp = std::dynamic_pointer_cast<CompT>(component);
        if (!comp) {
            THROW_EXCEPTION(fmt::format("PackableComponent is not of type {}",
                                        typeid(CompT).name())
                                .c_str());
        }
        PackerChain<CompT> *ptr = this;
        while (ptr != nullptr) {
            ptr->pack_part(comp, stream);
            ptr = ptr->next.get();
        }
    }
    ptr<PackableComponent> unpack(Util::ByteStream stream) override {
        auto comp = std::make_shared<CompT>();
        PackerChain<CompT> *ptr = this;
        while (ptr != nullptr) {
            ptr->unpack_part(comp, stream);
            ptr = ptr->next.get();
        }
        return std::dynamic_pointer_cast<PackableComponent>(comp);
    }

    virtual void pack_part(ptr<CompT> component, ByteStream stream) = 0;
    virtual void unpack_part(ptr<CompT>, ByteStream stream) = 0;

    CustomPacker<CompT> &
    add(std::function<void(ptr<CompT>, ByteStream)> reader,
        std::function<void(ptr<CompT>, ByteStream)> writer);

    template <typename FieldT>
    FieldPacker<CompT, FieldT> &
    add(std::function<FieldT &(ptr<CompT>)> ref_getter);

    PackerChain() = default;
};

template <typename CompT, typename FieldT>
class FieldPacker : virtual public PackerChain<CompT> {
    typedef std::function<FieldT &(ptr<CompT>)> getter_t;
    getter_t ref_getter;

  public:
    void pack_part(ptr<CompT> comp, ByteStream stream) override {
        FieldT &value = ref_getter(comp);
        stream.writeBinary(value);
    }
    void unpack_part(ptr<CompT> comp, ByteStream stream) override {
        FieldT &value = ref_getter(comp);
        value = stream.readBinary<FieldT>();
    }
};

template <typename CompT>
class CustomPacker : virtual public PackerChain<CompT> {
    friend class PackerChain<CompT>;

  private:
    typedef std::function<void(ptr<CompT>, ByteStream)> func_t;
    func_t reader, writer;

  public:
    CustomPacker(func_t reader, func_t writer)
        : reader(reader), writer(writer){};

    void pack_part(ptr<CompT> comp, ByteStream stream) override {
        writer(comp, stream);
    }
    void unpack_part(ptr<CompT> comp, ByteStream stream) override {
        reader(comp, stream);
    }
};

template <typename CompT>
inline CustomPacker<CompT> &
PackerChain<CompT>::add(std::function<void(ptr<CompT>, ByteStream)> reader,
                        std::function<void(ptr<CompT>, ByteStream)> writer) {
    auto chain = std::make_unique<CustomPacker<CompT>>();
    chain->reader = reader;
    chain->writer = writer;
    auto ptr = chain.release();
    next.reset(static_cast<PackerChain<CompT> *>(ptr));
    return *ptr;
}

template <typename CompT>
template <typename FieldT>
inline FieldPacker<CompT, FieldT> &
PackerChain<CompT>::add(std::function<FieldT &(ptr<CompT>)> ref_getter) {
    auto chain = std::make_unique<FieldPacker<CompT, FieldT>>();
    chain->ref_getter = ref_getter;
    auto ptr = chain.release();
    next.reset(static_cast<PackerChain<CompT> *>(ptr));
    return *ptr;
}

NSP_COMPONENTS_END