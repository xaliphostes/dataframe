#pragma once

#include <string>
#include <iostream>

namespace df
{

    inline void info(const std::string &msg)
    {
        std::cout << "\033[1;32mInfo : \033[0m" << msg << std::endl;
    }

    inline void warning(const std::string &msg)
    {
        std::cerr << "\033[1;34mWarn : \033[0m" << msg << std::endl;
    }

    inline void error(const std::string &msg)
    {
        std::cerr << "\033[1;31mError: \033[0m" << msg << std::endl;
    }

    template <typename Arg, typename... Args>
    inline void log(Arg &&arg, Args &&...args)
    {
        std::cout << std::forward<Arg>(arg);
        using expander = int[];
        (void)expander{0, (void(std::cout << ' ' << std::forward<Args>(args)), 0)...};
    }

}