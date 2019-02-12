///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2019 Bjorn Reese <breese@users.sourceforge.net>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <utility>

namespace trial
{
namespace online
{
namespace quantile
{

template <typename T, std::size_t W, typename... Quantiles>
interim_psquare<T, W, Quantiles...>::interim_psquare() noexcept
    : active(std::addressof(first)),
      inactive(nullptr)
{
}

template <typename T, std::size_t W, typename... Quantiles>
void interim_psquare<T, W, Quantiles...>::clear() noexcept
{
    first.clear();
    second.clear();
    active = std::addressof(first);
    inactive = nullptr;
}

template <typename T, std::size_t W, typename... Quantiles>
void interim_psquare<T, W, Quantiles...>::push(value_type input) noexcept
{
    assert(active);

    if (active->size() >= W / 2)
    {
        if (inactive)
        {
            inactive->clear();
            std::swap(active, inactive);
        }
        else
        {
            inactive = active;
            active = std::addressof(second);
        }
    }
    active->push(input);
}

template <typename T, std::size_t W, typename... Quantiles>
bool interim_psquare<T, W, Quantiles...>::empty() const noexcept
{
    return active->empty() && ((inactive == nullptr) || inactive->empty());
}

template <typename T, std::size_t W, typename... Quantiles>
template <typename Q>
auto interim_psquare<T, W, Quantiles...>::value() const noexcept -> value_type
{
    using index_type = boost::mp11::mp_find<QuantileList, Q>;

    return get<index_type::value>();
}

template <typename T, std::size_t W, typename... Quantiles>
template <std::size_t Index>
auto interim_psquare<T, W, Quantiles...>::get() const noexcept -> value_type
{
    assert(active);

    if (inactive)
    {
        const auto sz = active->size();
        return (inactive->template get<Index>() * (W / 2 - sz) + active->template get<Index>() * (2 * sz))
            / value_type(W / 2 + sz);
    }
    return active->template get<Index>();
}

} // namespace quantile
} // namespace online
} // namespace trial
