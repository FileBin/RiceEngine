#include "SerializableComponent.hpp"
#include "component.hpp"
#include <MetaCompiler/ReflectionHelper.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

#ifdef COMPONENT_NSP
#define PFX COMPONENT_NSP::
#else
#define PFX
#endif
#define STRINGIZE(STR) #STR
#define COMPONENT_STR STRINGIZE(COMPONENT_NAME)

typedef PFX COMPONENT_NAME component;

typedef component *p_component;

#ifndef CUSTOM_SERIALIZER

#include ".cache/component_meta.hpp"
#include "serialization_helper.hpp"

template <> struct component_serializer<component> {
    data_t to_bytes(p_component t) {
        data_t data;

        auto type = Meta::TypeOf<component>().type();
        auto members = type.getMembers();
        Meta::for_each(members, [&](const auto &member) {
            auto p = member.getMemberPointer();
            auto &mem = t->*p;
            data_t d = serializer<typename normalize_type<decltype(mem)>::type>().to_bytes(mem);
            data.insert(data.end(), d.begin(), d.end());
        });
        return data;
    }
    size_t from_bytes(data_t data, p_component &t) {
        t = new ::component;
        size_t sum = 0;

        auto type = Meta::TypeOf<component>().type();
        auto members = type.getMembers();

        Meta::for_each(members, [&](const auto &member) {
            auto p = member.getMemberPointer();
            auto &mem = t->*p;
            size_t n = serializer<typename normalize_type<decltype(mem)>::type>().from_bytes(data, mem);
            sum += n;
            data.erase(0, n);
        });

        return sum;
    }

    nlohmann::json to_json(p_component t) {
        using namespace nlohmann;
        json object = json::object();
        auto type = Meta::TypeOf<component>().type();
        object["name"] = type.getShortName();
        auto &arr = object["members"] = json::array();
        auto members = type.getMembers();
        Meta::for_each(members, [&](const auto &member) {
            std::string name = member.getName();
            auto p = member.getMemberPointer();
            auto &mem = t->*p;
            arr.push_back(json::object({
                {"name", name},
                {"value", mem},
            }));
        });
        return object;
    }
};

#endif