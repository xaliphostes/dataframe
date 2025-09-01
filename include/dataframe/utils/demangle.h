// simple_demangle.hpp
#pragma once

#include <string>
#include <typeinfo>

#ifdef __GNUC__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

namespace utils {

    inline std::string demangle(const char* name)
    {
        if (!name)
            return "";

#ifdef __GNUC__
        int status = 0;
        std::unique_ptr<char, void (*)(void*)> result(
            abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);

        if (status == 0 && result) {
            return std::string(result.get());
        }
#endif

        // Fallback: retourne le nom manglé sur Windows/MSVC
        return std::string(name);
    }

    template <typename T> std::string type_name() { return demangle(typeid(T).name()); }

} // namespace utils
