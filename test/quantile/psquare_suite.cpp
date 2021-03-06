///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Bjorn Reese <breese@users.sourceforge.net>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <trial/online/detail/lightweight_test.hpp>
#include <trial/online/quantile/psquare.hpp>

using namespace trial::online::quantile;

using upper_decile_ratio = std::ratio<9, 10>;

//-----------------------------------------------------------------------------

namespace api_suite
{

void test_ctor()
{
    psquare_median<double> filter;
    filter.push(1);
    TRIAL_ONLINE_TEST(!filter.empty());

    // Copy constructor
    psquare_median<double> copy(filter);
    TRIAL_ONLINE_TEST(!copy.empty());

    // Copy assignment
    psquare_median<double> assign;
    assign = filter;
    TRIAL_ONLINE_TEST(!assign.empty());

    // Move constructor
    psquare_median<double> mover(std::move(copy));
    TRIAL_ONLINE_TEST(!mover.empty());

    // Move assignment
    psquare_median<double> massign;
    massign = std::move(assign);
    TRIAL_ONLINE_TEST(!massign.empty());
 }

void test_empty()
{
    psquare_median<double> quantile;
    TRIAL_ONLINE_TEST(quantile.empty());
    quantile.push(1);
    TRIAL_ONLINE_TEST(!quantile.empty());
}

void test_clear()
{
    psquare_median<double> quantile;
    quantile.push(1);
    quantile.push(2);
    quantile.push(3);
    TRIAL_ONLINE_TEST(!quantile.empty());
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 2);
    quantile.clear();
    TRIAL_ONLINE_TEST(quantile.empty());
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 0);
}

void test_value()
{
    const double tolerance = 1e-5;
    psquare_quartile<double> quantile;
    quantile.push(1);
    quantile.push(2);
    quantile.push(3);
    quantile.push(4);
    quantile.push(5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 4, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), quantile.value<median_ratio>(), tolerance);
    quantile.push(6);
    quantile.push(7);
    quantile.push(8);
    quantile.push(9);
    quantile.push(10);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 7, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 10, tolerance);
}

void test_get()
{
    const double tolerance = 1e-5;
    psquare_quartile<double> quantile;
    quantile.push(1);
    quantile.push(2);
    quantile.push(3);
    quantile.push(4);
    quantile.push(5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<0>(), 1, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<1>(), 2, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<2>(), 3, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<3>(), 4, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<4>(), 5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get(), quantile.get<2>(), tolerance);
    quantile.push(6);
    quantile.push(7);
    quantile.push(8);
    quantile.push(9);
    quantile.push(10);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<0>(), 1, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<1>(), 3, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<2>(), 5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<3>(), 7, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<4>(), 10, tolerance);
}

void run()
{
    test_ctor();
    test_empty();
    test_clear();
    test_value();
    test_get();
}

} // api_suite

//-----------------------------------------------------------------------------

namespace paper_example
{

void test_median()
{
    using quantile_type = psquare_median<double>;

    const double tolerance = 1e-5;
    auto with_tolerance =
        [tolerance]
        (const quantile_type::parameter_type& lhs, const quantile_type::parameter_type& rhs)
        {
            return ((lhs.position == rhs.position) &&
                    std::fabs(lhs.height - rhs.height) <= tolerance);
        };

    quantile_type quantile;

    auto params = quantile.parameters();
    decltype(params) expected = { {1, 0.0}, {2, 0.0}, {3, 0.0}, {4, 0.0}, {5, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.02);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.02, tolerance);
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.0}, {3, 0.0}, {4, 0.0}, {5, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.26, tolerance);
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.0}, {4, 0.0}, {5, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.74);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.5, tolerance);
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 0.0}, {5, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(3.39);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.62, tolerance);
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 3.39}, {5, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.83);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance);
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 0.83}, {5, 3.39} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(22.37);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 0.785
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {5, 8.04}, {6, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.15);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 3.25333, tolerance); // 0.83
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {4, 3.25333}, {6, 14.41222}, {7, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.43);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 7.53148, tolerance); // 2.11
    params = quantile.parameters();
    expected = { {1, 0.02}, {3, 1.57778}, {5, 7.53148}, {7, 19.3584}, {8, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(38.62);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 7.53148, tolerance); // 3.39
    params = quantile.parameters();
    expected = { {1, 0.02}, {3, 1.57778}, {5, 7.53148}, {7, 19.3584}, {9, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.92);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 11.2658, tolerance); // 6.77
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 4.00514}, {6, 11.2658}, {7, 13.91597}, {10, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(34.60);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 11.2658, tolerance); // 10.15
    params = quantile.parameters();
    expected = { {1, 0.02}, {3, 1.75598}, {6, 11.2658}, {8, 17.97648}, {11, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.28);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 8.34625, tolerance); // 10.215
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 3.3787}, {6, 8.34625}, {10, 23.63396}, {12, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(1.47);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 8.34625, tolerance); // 10.15
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 1.71698}, {7, 8.34625}, {10, 17.97648}, {13, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.4);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 5.85354, tolerance); // 6.77
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 0.527512}, {7, 5.85354}, {10, 13.29556}, {14, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.05);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 5.85354, tolerance); // 3.39
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 0.527512}, {8, 5.85354}, {12, 17.97648}, {15, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);
}

void test_lower_quartile()
{
    using quantile_type = psquare<double, lower_quartile_ratio>;

    const double tolerance = 1e-5;
    auto with_tolerance =
        [tolerance]
        (const quantile_type::parameter_type& lhs, const quantile_type::parameter_type& rhs)
        {
            return ((lhs.position == rhs.position) &&
                    std::fabs(lhs.height - rhs.height) <= tolerance);
        };

    quantile_type quantile;
    quantile.push(0.02);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.02, tolerance);
    quantile.push(0.5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.14, tolerance);
    quantile.push(0.74);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.26, tolerance);
    quantile.push(3.39);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.38, tolerance);
    quantile.push(0.83);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.5, tolerance);
    auto params = quantile.parameters();
    decltype(params) expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 0.83}, {5, 3.39} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(22.37);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 0.56
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {5, 8.04}, {6, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.15);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 0.62
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 3.51125}, {7, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.43);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 0.68
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {5, 7.05987}, {8, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(38.62);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 0.74
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {6, 12.58486}, {9, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.92);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 2.83414, tolerance); // 0.7625
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {4, 2.83414}, {7, 18.27693}, {10, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(34.60);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 2.83414, tolerance); // 0.785
    params = quantile.parameters();
    expected = { {1, 0.02}, {3, 1.43805}, {4, 2.83414}, {8, 23.3892}, {11, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.28);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 1.24651, tolerance); // 0.8075
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 1.24651}, {8, 18.92848}, {12, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(1.47);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 2.62173, tolerance); // 0.83
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {4, 2.62173}, {8, 14.92867}, {13, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.4);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 1.13148, tolerance); // 0.7625
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.054784}, {4, 1.13148}, {10, 18.8754}, {14, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.05);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.195157, tolerance); // 0.62
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.054784}, {4, 0.195157}, {10, 14.96996}, {15, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);
}

void test_upper_quartile()
{
    using quantile_type = psquare<double, upper_quartile_ratio>;

    const double tolerance = 1e-5;
    auto with_tolerance =
        [tolerance]
        (const quantile_type::parameter_type& lhs, const quantile_type::parameter_type& rhs)
        {
            return ((lhs.position == rhs.position) &&
                    std::fabs(lhs.height - rhs.height) <= tolerance);
        };

    quantile_type quantile;
    quantile.push(0.02);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.02, tolerance);
    quantile.push(0.5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.38, tolerance);
    quantile.push(0.74);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.62, tolerance);
    quantile.push(3.39);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 1.4025, tolerance);
    quantile.push(0.83);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.83, tolerance);
    auto params = quantile.parameters();
    decltype(params) expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {4, 0.83}, {5, 3.39} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(22.37);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 0.74, tolerance); // 2.75
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {3, 0.74}, {5, 8.04}, {6, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.15);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 3.25333, tolerance); // 6.77
    params = quantile.parameters();
    expected = { {1, 0.02}, {2, 0.5}, {4, 3.25333}, {6, 14.41222}, {7, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.43);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 7.531481, tolerance); // 11.47
    params = quantile.parameters();
    expected = { {1, 0.02}, {3, 1.57778}, {5, 7.531481}, {7, 19.3584}, {8, 22.37} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(38.62);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 12.64923, tolerance); // 15.43
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 4.005139}, {6, 12.64923}, {8, 28.01532}, {9, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.92);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 17.67064, tolerance); // 15.7975
    params = quantile.parameters();
    expected = { {1, 0.02}, {5, 7.72845}, {7, 17.67064}, {8, 21.0322}, {10, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(34.60);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 17.67064, tolerance); // 19.145
    params = quantile.parameters();
    expected = { {1, 0.02}, {4, 4.27935}, {7, 17.67064}, {9, 25.64428}, {11, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.28);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 21.58474, tolerance); // 17.5325
    params = quantile.parameters();
    expected = { {1, 0.02}, {5, 6.80087}, {9, 21.58474}, {11, 31.32269}, {12, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(1.47);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 21.58474, tolerance); // 15.92
    params = quantile.parameters();
    expected = { {1, 0.02}, {5, 4.40479}, {10, 21.58474}, {11, 25.64428}, {13, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.4);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 17.77648, tolerance); // 15.7975
    params = quantile.parameters();
    expected = { {1, 0.02}, {5, 2.50422}, {10, 17.77648}, {13, 31.49365}, {14, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.05);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 21.98825, tolerance); // 15.675
    params = quantile.parameters();
    expected = { {1, 0.02}, {7, 4.53562}, {12, 21.98825}, {13, 25.94973}, {15, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);
}

void test_quartiles()
{
    using quantile_type = psquare_quartile<double>;

    const double tolerance = 1e-5;
    auto with_tolerance =
        [tolerance]
        (const quantile_type::parameter_type& lhs, const quantile_type::parameter_type& rhs)
        {
            return ((lhs.position == rhs.position) &&
                    std::fabs(lhs.height - rhs.height) <= tolerance);
        };

    quantile_type quantile;

    auto params = quantile.parameters();
    decltype(params) expected = {
        {1, 0.0}, {2, 0.0}, {3, 0.0},
        {4, 0.0}, {5, 0.0}, {6, 0.0},
        {7, 0.0}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.02);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.02, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.02, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 0.02, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.0}, {3, 0.0},
        {4, 0.0}, {5, 0.0}, {6, 0.0},
        {7, 0.0}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.14, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.26, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 0.38, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.0},
        {4, 0.0},  {5, 0.0}, {6, 0.0},
        {7, 0.0},  {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.74);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.26, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 0.62, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.0}, {5, 0.0}, {6, 0.0},
        {7, 0.0}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(3.39);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.38, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.62, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 1.4025, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 3.39}, {5, 0.0}, {6, 0.0},
        {7, 0.0}, {8, 0.0},  {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.83);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.74, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 0.83, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 0.0},
        {7, 0.0}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(22.37);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.56, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.785, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 2.75, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 22.37},
        {7, 0.0}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.15);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.62, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 0.83, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 6.77, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 10.15},
        {7, 22.37}, {8, 0.0}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.43);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.68, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 2.11, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 11.47, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 10.15},
        {7, 15.43}, {8, 22.37}, {9, 0.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(38.62);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.74, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.39, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 15.43, tolerance);
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 10.15},
        {7, 15.43}, {8, 22.37}, {9, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(15.92);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.74, tolerance); // 0.7625
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.39, tolerance); // 6.77
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 19.50333, tolerance); // 15.7975
    params = quantile.parameters();
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {6, 10.15},
        {8, 19.50333}, {9, 22.37}, {10, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(34.60);
    params = quantile.parameters();
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.74, tolerance); // 0.785
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.39, tolerance); // 10.15
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 19.50333, tolerance); // 19.145
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {5, 3.39}, {7, 15.52111},
        {8, 19.50333}, {10, 28.74222}, {11, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(10.28);
    params = quantile.parameters();
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.74, tolerance); // 0.8075
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 6.69185, tolerance); // 10.215
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 23.9995, tolerance); // 17.5325
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {4, 0.83}, {6, 6.69185}, {7, 11.25061},
        {10, 23.99951}, {11, 28.74222}, {12, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(1.47);
    params = quantile.parameters();
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.74, tolerance); // 0.83
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 6.69185, tolerance); // 10.15
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 19.63704, tolerance); // 15.92
    expected = {
        {1, 0.02}, {2, 0.5}, {3, 0.74},
        {5, 1.851975}, {7, 6.69185}, {9, 15.65481},
        {10, 19.63704}, {11, 22.41457}, {13, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.4);
    params = quantile.parameters();
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 1.21699, tolerance); // 0.7525
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.75653, tolerance); // 6.77
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 16.05907, tolerance); // 15.7675
    expected = {
        {1, 0.02}, {2, 0.26}, {5, 1.21699},
        {6, 1.851975}, {7, 3.75653}, {9, 11.68066},
        {10, 16.05907}, {13, 29.28604}, {14, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    quantile.push(0.05);
    params = quantile.parameters();
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.72432, tolerance); // 0.62
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.75653, tolerance); // 3.39
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 20.40031, tolerance); // 15.7975
    expected = {
        {1, 0.02}, {2, 0.1002}, {5, 0.72432},
        {6, 0.84124}, {8, 3.75653}, {9, 7.57981},
        {12, 20.40031}, {13, 23.21281}, {15, 38.62} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);

    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 0.02, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 0.72432, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.75653, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 20.40031, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 38.62, tolerance);

    TRIAL_ONLINE_TEST_CLOSE(quantile.get<0>(), quantile.value<minimum_ratio>(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<1>(), quantile.value<lower_quartile_ratio>(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<2>(), quantile.value<median_ratio>(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<3>(), quantile.value<upper_quartile_ratio>(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<4>(), quantile.value<maximum_ratio>(), tolerance);
    // Defaults
    TRIAL_ONLINE_TEST_CLOSE(quantile.get(), quantile.value<median_ratio>(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get<2>(), quantile.value(), tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.get(), quantile.value(), tolerance);
}

void run()
{
    test_median();
    test_lower_quartile();
    test_upper_quartile();
    test_quartiles();
}

} // namespace paper_example

//-----------------------------------------------------------------------------

namespace double_quartile_suite
{

void test_linear_increase()
{
    const double tolerance = 1e-5;
    psquare_quartile<double> quantile;

    quantile.push(1);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 1.0, tolerance);

    quantile.push(2);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 1.25, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 1.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 1.75, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 2.0, tolerance);

    quantile.push(3);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 1.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 2.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 2.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 3.0, tolerance);

    quantile.push(4);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 1.75, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 2.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 3.25, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 4.0, tolerance);

    quantile.push(5);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 2.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 4.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 5.0, tolerance);

    quantile.push(6);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 2.25, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 3.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 4.75, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 6.0, tolerance);

    quantile.push(7);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 2.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 4.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 5.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 7.0, tolerance);

    quantile.push(8);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 2.75, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 4.5, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 6.25, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 8.0, tolerance);

    quantile.push(9);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 5.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 7.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 9.0, tolerance);

    // Switch to approximation

    quantile.push(10);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance); // 3.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 5.0, tolerance); // 5.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 7.0, tolerance); // 7.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 10.0, tolerance);

    quantile.push(11);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance); // 3.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 5.0, tolerance); // 6.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 8.0, tolerance); // 8.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 11.0, tolerance); // 11.0

    quantile.push(12);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance); // 3.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 5.0, tolerance); // 6.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 9.0, tolerance); // 9.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 12.0, tolerance); // 12.0

    quantile.push(13);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance); // 4.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 6.0, tolerance); // 7.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 10.0, tolerance); // 10.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 13.0, tolerance); // 13.0

    quantile.push(14);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 3.0, tolerance); // 4.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 7.0, tolerance); // 7.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 11.0, tolerance); // 10.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 14.0, tolerance); // 14.0

    quantile.push(15);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 4.0, tolerance); // 4.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 8.0, tolerance); // 8.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 12.0, tolerance); // 11.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 15.0, tolerance); // 15.0

    quantile.push(16);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 5.0, tolerance); // 4.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 9.0, tolerance); // 8.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 13.0, tolerance); // 12.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 16.0, tolerance); // 16.0

    quantile.push(17);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 5.0, tolerance); // 5.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 9.0, tolerance); // 9.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 13.0, tolerance); // 13.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 17.0, tolerance); // 17.0

    quantile.push(18);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 6.0, tolerance); // 5.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 10.0, tolerance); // 9.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 14.0, tolerance); // 13.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 18.0, tolerance); // 18.0

    quantile.push(19);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 5.0, tolerance); // 5.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 10.0, tolerance); // 10.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 15.0, tolerance); // 14.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 19.0, tolerance); // 19.0

    quantile.push(20);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 6.0, tolerance); // 5.75
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 11.0, tolerance); // 10.5
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 16.0, tolerance); // 15.25
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 20.0, tolerance); // 20.0

    quantile.push(21);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<minimum_ratio>(), 1.0, tolerance); // 1.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<lower_quartile_ratio>(), 6.0, tolerance); // 6.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<median_ratio>(), 11.0, tolerance); // 11.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<upper_quartile_ratio>(), 16.0, tolerance); // 16.0
    TRIAL_ONLINE_TEST_CLOSE(quantile.value<maximum_ratio>(), 21.0, tolerance); // 21.0
}

void run()
{
    test_linear_increase();
}

} // namespace double_quartile_suite

//-----------------------------------------------------------------------------

namespace double_90_suite
{

void test_empty()
{
    psquare<double, upper_decile_ratio> quantile;
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 0.0);
}

void test_linear_increase()
{
    psquare<double, upper_decile_ratio> quantile;
    quantile.push(1.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 1.0);
    quantile.push(2.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 1.9);
    quantile.push(3.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 2.8);
    quantile.push(4.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 3.7);
    quantile.push(5.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 4.6);

    // Switch to approximation

    quantile.push(6.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 3.0); // 5.5
    quantile.push(7.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 4.0); // 6.4
    quantile.push(8.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 5.0); // 7.3
    quantile.push(9.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 6.0); // 8.2
    quantile.push(10.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 7.0); // 9.1
    quantile.push(11.0);
    TRIAL_ONLINE_TEST_EQUAL(quantile.value(), 8.0); // 10.0
}

void run()
{
    test_empty();
    test_linear_increase();
}

} // namespace double_90_suite

//-----------------------------------------------------------------------------

namespace double_approx_suite
{

void test_count_100()
{
    using quantile_type = psquare_median<double>;

    const double tolerance = 1e-5;
    auto with_tolerance =
        [tolerance]
        (const quantile_type::parameter_type& lhs, const quantile_type::parameter_type& rhs)
        {
            return ((lhs.position == rhs.position) &&
                    std::fabs(lhs.height - rhs.height) <= tolerance);
        };

    quantile_type quantile;
   // Approximate normal distribution with expectation 1 and spread 1 after 100 iterations.
   decltype(quantile.parameters()) expected = {
            { 20, -2.0 },
            { 40, 0.5 },
            { 60, 1.0 },
            { 80, 1.5 },
            { 100, 4.0 } };
   quantile.parameters(expected);

    quantile.push(1000.0);
    TRIAL_ONLINE_TEST_CLOSE(quantile.value(), 1.023782, tolerance);
    auto params = quantile.parameters();
    expected = { {20, -2.0}, {41, 0.5725}, {61, 1.023782}, {81, 25.28634}, {101, 1000.0} };
    TRIAL_ONLINE_TEST_ALL_WITH(params.begin(), params.end(), expected.begin(), expected.end(), with_tolerance);
}

void run()
{
    test_count_100();
}

} // namespace double_approx_suite

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main()
{
    api_suite::run();
    paper_example::run();
    double_quartile_suite::run();
    double_90_suite::run();
    double_approx_suite::run();

    return boost::report_errors();
}
