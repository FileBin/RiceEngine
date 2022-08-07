#include "Rice/Scene/PackableComponent.hpp"
#include "Rice/Util/ByteStream.hpp"
#include "Rice/defines.h"

NSP_COMPONENTS_BEGIN

Util::RegisterCollection<ComponentPacker> PackableComponent::packers;

data_t PackableComponent::pack() {
    data_t data;
    ByteStream stream(data);
    auto &packer = getPacker();
    stream.writeBinary(packer.id);

    auto packable_comp =
        std::dynamic_pointer_cast<PackableComponent>(shared_from_this());

    packer.pack(packable_comp, stream);
    return stream.getData();
}

ptr<PackableComponent> PackableComponent::unpack(ByteStream stream) {
    uint id = stream.readBinary<uint>();
    return packers.get(id).unpack(stream);
}

ComponentPacker::ComponentPacker() {
    id = PackableComponent::packers.registerPtr(shared_from_this());
}

NSP_COMPONENTS_END