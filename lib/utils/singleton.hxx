#pragma once

#include "utils/singleton.hh"

template <typename T>
T& Singleton<T>::instance()
{
    static T instance;
    return instance;
}