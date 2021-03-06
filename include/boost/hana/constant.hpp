/*!
@file
Defines `boost::hana::Constant`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONSTANT_HPP
#define BOOST_HANA_CONSTANT_HPP

#include <boost/hana/fwd/constant.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/config.hpp>
#include <boost/hana/core/common.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/detail/canonical_constant.hpp>
#include <boost/hana/detail/dispatch_if.hpp>

#include <type_traits>


namespace boost { namespace hana {
    //////////////////////////////////////////////////////////////////////////
    // value
    //////////////////////////////////////////////////////////////////////////
    template <typename C, typename>
    struct value_impl : value_impl<C, when<true>> { };

    template <typename C, bool condition>
    struct value_impl<C, when<condition>> : default_ {
        template <typename>
        static void apply(...);
    };

    template <typename T>
    constexpr decltype(auto) value() {
        using RawT = typename std::remove_cv<
            typename std::remove_reference<T>::type
        >::type;
        using C = typename datatype<RawT>::type;
        using Value = BOOST_HANA_DISPATCH_IF(
            value_impl<C>, _models<Constant, C>{}()
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(_models<Constant, C>{},
        "hana::value<T>() requires T to be a Constant");
    #endif

        return Value::template apply<RawT>();
    }

    //////////////////////////////////////////////////////////////////////////
    // models
    //////////////////////////////////////////////////////////////////////////
    template <typename C>
    struct models_impl<Constant, C>
        : _integral_constant<bool,
            !is_default<value_impl<C>>{}()
        >
    { };

    //////////////////////////////////////////////////////////////////////////
    // Conversion to the underlying data type
    //////////////////////////////////////////////////////////////////////////
    template <typename To, typename From>
    struct to_impl<To, From, when<
        _models<Constant, From>{}() && is_convertible<typename From::value_type, To>{}()
    >> : embedding<is_embedded<typename From::value_type, To>{}()> {
        template <typename X>
        static constexpr decltype(auto) apply(X const&)
        { return to<To>(hana::value<X>()); }
    };

    //////////////////////////////////////////////////////////////////////////
    // Provided common data types
    //////////////////////////////////////////////////////////////////////////
    namespace constant_detail {
        //! @todo
        //! This is an awful hack to avoid having
        //! @code
        //!     common<IntegralConstant<int>, IntegralConstant<long>>
        //!         ==
        //!     CanonicalConstant<long>
        //! @endcode
        template <typename A, typename B, typename C>
        struct which {
            using type = detail::CanonicalConstant<C>;
        };

        template <template <typename ...> class A, typename T, typename U, typename C>
        struct which<A<T>, A<U>, C> {
            using type = A<C>;
        };
    }

    template <typename A, typename B>
    struct common<A, B, when<
        _models<Constant, A>{}() &&
        _models<Constant, B>{}() &&
        has_common<typename A::value_type, typename B::value_type>{}()
    >> {
        using type = typename constant_detail::which<
            A, B,
            typename common<typename A::value_type,
                            typename B::value_type>::type
        >::type;
    };

    template <typename A, typename B>
    struct common<A, B, when<
        _models<Constant, A>{}() &&
        !_models<Constant, B>{}() &&
        has_common<typename A::value_type, B>{}()
    >> {
        using type = typename common<typename A::value_type, B>::type;
    };

    template <typename A, typename B>
    struct common<A, B, when<
        !_models<Constant, A>{}() &&
        _models<Constant, B>{}() &&
        has_common<A, typename B::value_type>{}()
    >> {
        using type = typename common<A, typename B::value_type>::type;
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONSTANT_HPP
