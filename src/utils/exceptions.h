#pragma once

#include <exception>
#include <string>

namespace Wizrd {
    class BaseException: std::exception
    {
    public:
        inline BaseException(const std::string what)
            :what_(what.c_str())
        {
        }
        inline const char* what() noexcept
        {
            return what_;
        }
    private:
        const char* what_;
    };
}
