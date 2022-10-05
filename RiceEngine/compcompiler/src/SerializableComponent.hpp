/*
 * file: SerializableComponent.hpp
 * author: filebin
 * date: 2022-10-06 00:52:29
 */

#include "Rice/Scene/Component.hpp"

NSP_COMPONENTS_BEGIN
template <typename PublicT, typename PrivateT> class SerializableComponent;
NSP_COMPONENTS_END

#pragma once

NSP_COMPONENTS_BEGIN

template <typename PublicT, typename PrivateT>
class SerializableComponent : public virtual Component,
                              public PublicT,
                              protected PrivateT {
  public:
    SerializableComponent();
    ~SerializableComponent();

  private:
};

NSP_COMPONENTS_END