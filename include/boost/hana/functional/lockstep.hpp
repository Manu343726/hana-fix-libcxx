/*!
@file
Defines `boost::hana::lockstep`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP
#define BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP

#include <boost/hana/config.hpp>
#include <boost/hana/detail/closure.hpp>
#include <boost/hana/detail/create.hpp>

#include <utility>


namespace boost { namespace hana {
    //! @ingroup group-functional
    //! Invoke a function with the result of invoking other functions on its
    //! arguments, in lockstep.
    //!
    //! Specifically, `lockstep(f)(g1, ..., gN)` is a function such that
    //! @code
    //!     lockstep(f)(g1, ..., gN)(x1, ..., xN) == f(g1(x1), ..., gN(xN))
    //! @endcode
    //!
    //! Since each `g` is invoked on its corresponding argument in lockstep,
    //! the number of arguments must match the number of `g`s.
    //!
    //!
    //! Example
    //! -------
    //! @snippet example/functional.cpp lockstep
#ifdef BOOST_HANA_DOXYGEN_INVOKED
    constexpr auto lockstep = [](auto&& f, auto&& ...g) {
        return [perfect-capture](auto&& ...x) -> decltype(auto) {
            return forwarded(f)(forwarded(g)(forwarded(x))...);
        };
    };
#else
    template <typename F, typename G>
    struct _lockstep;

    template <typename F, typename ...G>
    struct _lockstep<F, detail::closure_impl<G...>> {
        F f;
        detail::closure_impl<G...> g;

        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) const& {
            return f(static_cast<G const&>(g).get(static_cast<X&&>(x))...);
        }

#ifndef BOOST_HANA_CONFIG_CONSTEXPR_MEMBER_FUNCTION_IS_CONST
        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) & {
            return f(static_cast<G&>(g).get(static_cast<X&&>(x))...);
        }
#endif

        template <typename ...X>
        constexpr decltype(auto) operator()(X&& ...x) && {
            return f(static_cast<G&&>(g).get(static_cast<X&&>(x))...);
        }
    };

    template <typename F>
    struct _pre_lockstep {
        F f;
        template <typename ...G>
        constexpr decltype(auto) operator()(G&& ...g) const& {
            return detail::create<_lockstep>{}(f,
                detail::create<detail::closure>{}(static_cast<G&&>(g)...)
            );
        }

        template <typename ...G>
        constexpr decltype(auto) operator()(G&& ...g) && {
            return detail::create<_lockstep>{}(std::move(f),
                detail::create<detail::closure>{}(static_cast<G&&>(g)...)
            );
        }
    };

    constexpr detail::create<_pre_lockstep> lockstep{};
#endif
}} // end namespace boost::hana

#endif // !BOOST_HANA_FUNCTIONAL_LOCKSTEP_HPP
