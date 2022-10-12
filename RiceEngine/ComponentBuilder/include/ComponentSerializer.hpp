#include "SerializableComponent.hpp"
#include "component.hpp"
//#include <boost/mp11.hpp>
//#include <boost/mp11/algorithm.hpp>
#include <boost/pfr/core.hpp>
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
typedef PFX PPCAT(moc__, COMPONENT_NAME) component_moc;

#ifndef CUSTOM_SERIALIZER

#include "serialization_helper.hpp"

template <> struct component_serializer<component> {
    data_t to_bytes(p_component t) {
        component_moc::Private *pri = t->getPrivate();
        component_moc::Public *pub = t->getPublic();
        data_t data;

        boost::pfr::for_each_field(*pri, [&data](auto field) {
            data_t d = serializer<decltype(field)>().to_bytes(field);
            data.insert(data.end(), d.begin(), d.end());
        });
        boost::pfr::for_each_field(*pub, [&data](auto field) {
            data_t d = serializer<decltype(field)>().to_bytes(field);
            data.insert(data.end(), d.begin(), d.end());
        });
        return data;
    }
    size_t from_bytes(data_t data, p_component &t) {
        t = new ::component;
        component_moc::Private *pri = t->getPrivate();
        component_moc::Public *pub = t->getPublic();
        size_t sum = 0;
        boost::pfr::for_each_field(*pri, [&data, &sum](auto &&field) {
            size_t n = serializer<typename normalize_type<decltype(field)>::type>().from_bytes(data, field);
            sum += n;
            data.erase(0, n);
        });
        boost::pfr::for_each_field(*pub, [&data, &sum](auto &&field) {
            size_t n = serializer<typename normalize_type<decltype(field)>::type>().from_bytes(data, field);
            sum += n;
            data.erase(0, n);
        });
        return sum;
    }
};

#endif