#include "../stdafx.hpp"

#include "Component.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/Util/RegisterCollection.hpp"
#include "Rice/defines.h"
#include <functional>

#pragma once

NSP_ENGINE_BEGIN
class Object;
class Engine;
NSP_ENGINE_END

NSP_COMPONENTS_BEGIN

class ComponentPacker;

class PackableComponent : virtual public Component {
    friend class ComponentPacker;
    static Util::RegisterCollection<ComponentPacker> packers;

  public:
    data_t pack() override;
    static ptr<PackableComponent> unpack(ByteStream data);
    virtual ComponentPacker &getPacker() = 0;
};

class ComponentPacker : public enable_ptr<ComponentPacker> {
    friend class PackableComponent;

  private:
    uint id;

  public:
    virtual void pack(ptr<PackableComponent> component, ByteStream stream) = 0;
    virtual ptr<PackableComponent> unpack(Util::ByteStream stream) = 0;

    ComponentPacker();
};

NSP_COMPONENTS_END