///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Bjorn Reese <breese@users.sourceforge.net>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <limits>

namespace trial
{
namespace online
{
namespace cumulative
{

template <typename T>
void correlation<T>::clear()
{
    covariance.clear();
    x_moment.clear();
    y_moment.clear();
}

template <typename T>
auto correlation<T>::size() const -> size_type
{
    return covariance.size();
}

template <typename T>
void correlation<T>::push(value_type x, value_type y)
{
    covariance.push(x, y);
    x_moment.push(x);
    y_moment.push(y);
}

template <typename T>
auto correlation<T>::value() const -> value_type
{
    const value_type variance_product = x_moment.variance() * y_moment.variance();
    if (variance_product < std::numeric_limits<value_type>::epsilon())
        return value_type(1);
    return covariance.value() / std::sqrt(variance_product);
}

} // namespace cumulative
} // namespace online
} // namespace trial