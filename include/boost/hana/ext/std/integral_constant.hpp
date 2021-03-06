/*!
@file
Defines `boost::hana::ext::std::IntegralConstant`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_EXT_STD_INTEGRAL_CONSTANT_HPP
#define BOOST_HANA_EXT_STD_INTEGRAL_CONSTANT_HPP

#include <boost/hana/comparable.hpp>
#include <boost/hana/constant.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/is_a.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/enumerable.hpp>
#include <boost/hana/group.hpp>
#include <boost/hana/integral_domain.hpp>
#include <boost/hana/logical.hpp>
#include <boost/hana/monoid.hpp>
#include <boost/hana/orderable.hpp>
#include <boost/hana/ring.hpp>

#include <type_traits>


namespace boost { namespace hana {
    namespace ext { namespace std {
        template <typename T>
        struct IntegralConstant { using value_type = T; };
    }}

    namespace std_ic_detail {
        template <typename T, T v>
        constexpr bool
        is_std_integral_constant(std::integral_constant<T, v>*)
        { return true; }

        constexpr bool is_std_integral_constant(...)
        { return false; }
    }

    template <typename T>
    struct datatype<T, when<std_ic_detail::is_std_integral_constant((T*)0)>> {
        using type = ext::std::IntegralConstant<
            typename datatype<typename T::value_type>::type
        >;
    };

    //////////////////////////////////////////////////////////////////////////
    // Constant
    //////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct value_impl<ext::std::IntegralConstant<T>> {
        template <typename C>
        static constexpr auto apply()
        { return C::value; }
    };

    template <typename T, typename C>
    struct to_impl<ext::std::IntegralConstant<T>, C, when<
        _models<Constant, C>{}() &&
        std::is_integral<typename C::value_type>{}()
    >>
        : embedding<is_embedded<typename C::value_type, T>{}>
    {
        static_assert(std::is_integral<T>{},
        "trying to convert a Constant to a std::integral_constant of a "
        "non-integral type; std::integral_constant may only hold "
        "integral types");

        template <typename X>
        static constexpr auto apply(X const&) {
            constexpr T v = hana::value<X>();
            return std::integral_constant<T, v>{};
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_EXT_STD_INTEGRAL_CONSTANT_HPP
