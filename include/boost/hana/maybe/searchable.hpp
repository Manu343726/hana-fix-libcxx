/*!
@file
Defines the instance of `boost::hana::Searchable` for `boost::hana::Maybe`.

@copyright Louis Dionne 2014
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_MAYBE_SEARCHABLE_HPP
#define BOOST_HANA_MAYBE_SEARCHABLE_HPP

#include <boost/hana/bool.hpp>
#include <boost/hana/logical/logical.hpp>
#include <boost/hana/maybe/maybe.hpp>
#include <boost/hana/searchable/mcd.hpp>


namespace boost { namespace hana {
    //! Instance of `Searchable` for `Maybe`s.
    //!
    //! Searching a `Maybe` is equivalent to searching a list containing
    //! `x` for `just(x)` and an empty list for `nothing`.
    //!
    //! ### Example
    //! @snippet example/maybe/searchable.cpp main
    template <>
    struct Searchable::instance<Maybe> : Searchable::mcd {
        template <typename M, typename Pred>
        static constexpr auto find_impl(M m, Pred p) {
            return maybe(
                nothing,
                [=](auto x) { return if_(p(x), just(x), nothing); },
                m
            );
        }

        template <typename M, typename Pred>
        static constexpr auto any_impl(M m, Pred p)
        { return maybe(false_, p, m); }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_MAYBE_SEARCHABLE_HPP