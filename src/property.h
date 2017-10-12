
#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>

namespace lab
{

    class PropertyBase
    {
    public:
        PropertyBase(const char* name)
        : name(name) {}

        virtual ~PropertyBase() {}
        virtual void serialize() = 0;
        virtual void deserialize() = 0;

        std::string name;
    };

    class PropertyDict
    {
        std::unordered_map<std::string, PropertyBase*> props;

    public:
        PropertyDict& bind(PropertyBase* p)
        {
            props[p->name] = p;
            return *this;
        }

        void serialize()
        {
            std::for_each(props.begin(), props.end(), [](auto& p)
            {
                p.second->serialize();
            });
        }

        void deserialize()
        {
            std::for_each(props.begin(), props.end(), [](auto& p)
            {
                p.second->deserialize();
            });
        }

    };

    template<typename T, typename Serializer>
    class SerializableProperty : public PropertyBase
    {
        T val;

    public:
        SerializableProperty(PropertyDict* pd, const char* name, const T& v)
        : PropertyBase(name)
        , val(v)
        {
            pd->bind(this);
        }

        SerializableProperty(PropertyDict* pd, const char* name, T&& v)
        : PropertyBase(name)
        {
            val = std::move(val);
            pd->bind(this);
        }

        void set(T&& v)
        {
            val = std::move(v);
        }
        void set(const T& v)
        {
            val = v;
        }
        T get() const
        {
            return val;
        }

        virtual void serialize() override
        {
            Serializer::serialize(this, name, val);
        }

        virtual void deserialize() override
        {
            Serializer::deserialize(this, name);
        }

        static SerializableProperty* as(PropertyBase* pb)
        {
            SerializableProperty* r = dynamic_cast<SerializableProperty>(pb);
            if (!r)
                throw std::runtime_error("Invalid property cast");
            return r;
        }
    };


    class Sample : public PropertyDict
    {
        class Serializer
        {
        public:
            static void serialize(SerializableProperty<float, Serializer>*, const std::string& name, float v) {}
            static void deserialize(SerializableProperty<float, Serializer>*, const std::string&name) {}
        };

    public:
        template<typename T>
        using Property = typename SerializableProperty<T, Serializer>;

        Property<float> gain = { this, "gain", 1.f };
        Property<float> q =    { this, "q",    0.2f };
    };

}
