// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Wed Apr 22 2020
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================
#pragma once

#include <functional>
#include <type_traits>
#include <utility>
#include "executionGraph/common/Exception.hpp"
#include "executionGraph/common/MetaCommon.hpp"
#include "executionGraph/common/MoveOnly.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"

namespace executionGraph
{
    template<typename T>
    class AnyInvocable final : private std::function<T>,
                               private MoveOnly
    {
        template<typename F, typename En = void>
        struct Wrapper;

        //! Specialization for CopyConstructible F
        template<typename F>
        struct Wrapper<F,
                       EG_ENABLE_IF_CLASS(std::is_copy_constructible_v<F>&&
                                              std::is_move_constructible_v<F>)>
        {
            template<typename... Args>
            auto operator()(Args&&... args)
            {
                return func(std::forward<Args>(args)...);
            }

            F func;
        };

        //! Specialization for MoveConstructible-only F
        template<typename F>
        struct Wrapper<F,
                       EG_ENABLE_IF_CLASS(!std::is_copy_constructible_v<F> &&
                                          std::is_move_constructible_v<F>)>
        {
            Wrapper(F&& func)
                : func(std::forward<F>(func)) {}

            Wrapper(Wrapper&&) = default;
            Wrapper& operator=(Wrapper&&) = default;

            // These two functions are instantiated by `std::function`
            // and are never called
            Wrapper(const Wrapper& rhs) : func(std::move(const_cast<Wrapper&>(rhs).func))
            {
                EG_THROW("You copied a move-only function !");
            }

            Wrapper& operator=(Wrapper&)
            {
                EG_THROW("You copied a move-only function !");
            }

            template<typename... Args>
            decltype(auto) operator()(Args&&... args)
            {
                return func(std::forward<Args>(args)...);
            }

            F func;
        };

        using Base = std::function<T>;

    public:
        AnyInvocable() noexcept = default;

        explicit AnyInvocable(std::nullptr_t) noexcept
            : Base(nullptr) {}

        template<typename F>
        AnyInvocable(F&& f)
            : Base(Wrapper<naked<F>>{std::forward<F>(f)})
        {}

        template<typename F>
        AnyInvocable& operator=(F&& f)
        {
            Base::operator=(Wrapper<naked<F>>{std::forward<F>(f)});
            return *this;
        }

        AnyInvocable& operator=(std::nullptr_t)
        {
            Base::operator=(nullptr);
            return *this;
        }

        bool operator==(std::nullptr_t)
        {
            return static_cast<Base&>(*this) == nullptr;
        }

        bool operator!=(std::nullptr_t)
        {
            return static_cast<Base&>(*this) != nullptr;
        }

        template<typename... Args>
        decltype(auto) operator()(Args&&... args)
        {
            return Base::operator()(std::forward<Args>(args)...);
        }
    };
}  // namespace executionGraph