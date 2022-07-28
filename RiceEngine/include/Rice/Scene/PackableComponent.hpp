#include "../stdafx.hpp"
#include "Rice/Engine/Log.hpp"
#include "Rice/namespaces.h"
#include <tuple>

NSP_COMPONENTS_BEGIN
template <typename... Types> struct RefTuple;
NSP_COMPONENTS_END
#pragma once

#include "Component.hpp"

NSP_COMPONENTS_BEGIN
template <typename Origin, typename TupleFields, typename TupleRefs>
struct PackableComponentData;

template <typename... Types> struct RefTuple {
    typedef std::tuple<Types...> tuple_t;
    typedef std::tuple<Types &...> ref_tuple_t;

    RefTuple(Types &...refs) : refs(refs...) {}

    ref_tuple_t refs;

    tuple_t getData() { return refs; }

    void setData(tuple_t data) { refs = data; }
};

template <typename Origin, typename TupleFields, typename TupleRefs>
class PackableComponent : public Component {
    template <typename O, typename TF, typename TR>
    friend struct PackableComponentData;

  private:
    TupleRefs refs;
    TupleFields fields;

  public:
    PackableComponent(TupleFields fields, TupleRefs refs)
        : fields(fields), refs(refs) {}
    pComponentData pack() override {
        return new_ref<PackableComponentData<Origin, TupleFields, TupleRefs>>(
            refptr_this(), fields, refs);
    }
};

template <typename Origin, typename TupleFields, typename TupleRefs>
struct PackableComponentData : public ComponentData {
    typedef PackableComponent<Origin, TupleFields, TupleRefs> component_t;
    typedef RefPtr<component_t> p_component_t;

    typename TupleRefs::tuple_t refs;
    typename TupleFields::tuple_t fields;

    PackableComponentData(pComponent origin, TupleFields fields, TupleRefs refs)
        : ComponentData(origin), fields(fields.refs), refs(refs.refs) {}

    virtual pComponent createComponent() override {
        p_component_t component = new_ref<Origin>();
        component->refs.setData(refs);
        component->fields.setData(fields);
        return component;
    }

    virtual data_t pack() override {
        // TODO make packing
        Log::log(Log::Warning, "Packing not implemented");
        return {};
    }
};

NSP_COMPONENTS_END

#include "Object.hpp"
#include "../Engine/Engine.hpp"