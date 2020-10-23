#pragma once

#include <vector>
#include "Functional.h"

template<typename ...T>
using EventHandler = std::function<void(T...)>;
//using EventHandler = void(*)(T...);

template<typename ...T>
class Event
{
  public:
    EventHandler<T...> handler;
    Event()
    {
    }

    void operator=(EventHandler<T...> right)
    {
        handler = right;
    }

    void operator()(T ...args)
    {
        invoke(args...);
    }

    void invoke(T ...args)
    {
        if (handler != nullptr)
            handler(args...);
    }
};
