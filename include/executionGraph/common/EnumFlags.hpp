// =========================================================================================
//  ExecutionGraph
//  Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
//
//  @date Mon Dec 30 2019
//  @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
// =========================================================================================

#pragma once

#include <type_traits>
#include <external/bitset2/bitset2.hpp>
#include <meta/meta.hpp>
#include "executionGraph/common/EnumClassHelper.hpp"
#include "executionGraph/common/SfinaeMacros.hpp"

namespace executionGraph
{
    template<typename TEnum>
    class EnumFlags final
    {
    public:
        using Enum     = TEnum;
        using Integral = std::underlying_type_t<Enum>;

    private:
        static constexpr auto nBits = sizeof(Integral) * 8;
        using FlagsInternal         = Bitset2::bitset2<nBits, Integral>;

    private:
        template<typename... T>
        static constexpr bool isFlags = (... && std::is_same_v<T, EnumFlags>);
        template<typename... T>
        static constexpr bool isEnum = (... && std::is_same_v<T, Enum>);

        template<typename... T>
        static constexpr bool isFlagsOrEnum = (... && (isFlags<T> || isEnum<T>));

    public:
        constexpr EnumFlags() noexcept = default;
        constexpr EnumFlags(const EnumFlags&) noexcept = default;
        constexpr EnumFlags(EnumFlags&&) noexcept = default;

        constexpr explicit EnumFlags(Integral i) noexcept
            : m_flags(i) {}

        constexpr EnumFlags(Enum e) noexcept
            : m_flags(enumToInt(e)) {}

        template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr EnumFlags(F f, FF... ff) noexcept
            : EnumFlags(ff...)
        {
            set(EnumFlags{f});
        }

        constexpr auto value() const { return m_flags.value(); }

        constexpr auto& reset() noexcept
        {
            m_flags.reset();
            return *this;
        }

        constexpr auto& set(EnumFlags f) noexcept
        {
            m_flags |= f.m_flags;
            return *this;
        }

        constexpr auto& unset(EnumFlags f) noexcept
        {
            m_flags &= ~f.m_flags;
            return *this;
        }

        template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr auto& set(F f, FF... ff) noexcept
        {
            return set(f).set(ff...);
        }

        template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr auto& unset(F f, FF... ff) noexcept
        {
            return unset(f).unset(f).unset(ff...);
        }

        constexpr bool isAllSet() const noexcept { return m_flags.all(); }
        constexpr bool isAnySet() const noexcept { return m_flags.any(); }
        constexpr bool isNone() const noexcept { return m_flags.none(); }

        constexpr bool isSet(EnumFlags f) const noexcept
        {
            return (m_flags & f.m_flags) == f.m_flags;
        }

       template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr bool isSet(F f, FF... ff) const noexcept
        {
            return (isSet(f) && ... && isSet(ff));
        }

        constexpr bool isUnset(EnumFlags f) const noexcept
        {
            return EnumFlags{*this}.unset(f) == *this;
        }

       template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr bool isUnset(F f, FF... ff) const noexcept
        {
            return (isUnset(f) && ... && isUnset(ff));
        }

       template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr bool isAnySet(F f, FF... ff) const noexcept
        {
            return (isSet(f) ||... || isSet(ff));
        }

        template<typename F,
                 typename... FF,
                 EG_ENABLE_IF(isFlagsOrEnum<F, FF...> && sizeof...(FF) > 0)>
        constexpr bool isAnyUnset(F f, FF... ff) const noexcept
        {
            return (isUnset(f) ||...|| isUnset(ff));
        }

        constexpr bool operator==(const EnumFlags& other) const noexcept
        {
            return m_flags == other.m_flags;
        }

        constexpr bool operator!=(const EnumFlags& other) const noexcept
        {
            return m_flags != other.m_flags;
        }

    private:
        EnumFlags(FlagsInternal&& flags) noexcept
            : m_flags(std::forward<FlagsInternal>(flags)) 
        {
        }

        FlagsInternal m_flags;
    };
}  // namespace executionGraph