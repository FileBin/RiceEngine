/*
 * file: SerializableComponent.hpp
 * author: filebin
 * date: 2022-10-06 00:52:29
 */
#pragma once

#include "Rice/Scene/Component.hpp"
#include "serialization_helper.hpp"

NSP_COMPONENTS_BEGIN
template <typename PublicT, typename PrivateT> class SerializableComponent;
NSP_COMPONENTS_END

NSP_COMPONENTS_BEGIN

template <typename PublicT, typename PrivateT>
class SerializableComponent : public virtual Component,
                              public PublicT,
                              protected PrivateT {
  public:
    SerializableComponent();
    ~SerializableComponent();

  protected:
    PrivateT *getPrivate() { return static_cast<PrivateT *>(this); }
    PublicT *getPublic() { return static_cast<PublicT *>(this); }
    template <typename T> friend struct ::serializator;
};

NSP_COMPONENTS_END