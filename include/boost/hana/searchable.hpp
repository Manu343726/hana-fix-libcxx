/*!
@file
Defines `boost::hana::Searchable`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_SEARCHABLE_HPP
#define BOOST_HANA_SEARCHABLE_HPP

#include <boost/hana/fwd/searchable.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/comparable.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/operators.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/detail/has_common_embedding.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/functional/id.hpp>
#include <boost/hana/functional/partial.hpp>
#include <boost/hana/logical.hpp>
#include <boost/hana/optional.hpp>

#include <cstddef>
#include <type_traits>


namespace boost { namespace hana {
    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    namespace operators {
        template <typename Derived>
        struct Searchable_ops {
            template <typename Key>
            constexpr decltype(auto) operator[](Key&& key) const& {
                return hana::at_key(
                    static_cast<Derived const&>(*this),
                    static_cast<Key&&>(key)
                );
            }

            template <typename Key>
            constexpr decltype(auto) operator[](Key&& key) & {
                return hana::at_key(
                    static_cast<Derived&>(*this),
                    static_cast<Key&&>(key)
                );
            }

            template <typename Key>
            constexpr decltype(auto) operator[](Key&& key) && {
                return hana::at_key(
                    static_cast<Derived&&>(*this),
                    static_cast<Key&&>(key)
                );
            }
        };
    }

    //////////////////////////////////////////////////////////////////////////
    // any_of
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct any_of_impl : any_of_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct any_of_impl<S, when<condition>> : default_ {
        static void apply(...) { }
    };

    //////////////////////////////////////////////////////////////////////////
    // any
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct any_impl : any_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct any_impl<S, when<condition>> : default_ {
        template <typename Xs>
        static constexpr decltype(auto) apply(Xs&& xs)
        { return hana::any_of(static_cast<Xs&&>(xs), id); }
    };

    //////////////////////////////////////////////////////////////////////////
    // all_of
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct all_of_impl : all_of_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct all_of_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Pred>
        static constexpr decltype(auto) apply(Xs&& xs, Pred&& pred) {
            return hana::not_(hana::any_of(static_cast<Xs&&>(xs),
                    hana::compose(not_, static_cast<Pred&&>(pred))));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // all
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct all_impl : all_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct all_impl<S, when<condition>> : default_ {
        template <typename Xs>
        static constexpr decltype(auto) apply(Xs&& xs)
        { return hana::all_of(static_cast<Xs&&>(xs), id); }
    };

    //////////////////////////////////////////////////////////////////////////
    // none_of
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct none_of_impl : none_of_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct none_of_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Pred>
        static constexpr decltype(auto) apply(Xs&& xs, Pred&& pred) {
            return hana::not_(hana::any_of(static_cast<Xs&&>(xs),
                                           static_cast<Pred&&>(pred)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // none
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct none_impl : none_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct none_impl<S, when<condition>> : default_ {
        template <typename Xs>
        static constexpr decltype(auto) apply(Xs&& xs)
        { return hana::none_of(static_cast<Xs&&>(xs), hana::id); }
    };

    //////////////////////////////////////////////////////////////////////////
    // contains
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct contains_impl : contains_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct contains_impl<S, when<condition>> : default_ {
        template <typename Xs, typename X>
        static constexpr decltype(auto) apply(Xs&& xs, X&& x) {
            return hana::any_of(static_cast<Xs&&>(xs),
                    hana::equal.to(static_cast<X&&>(x)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // find_if
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct find_if_impl : find_if_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct find_if_impl<S, when<condition>> : default_ {
        static void apply(...) { }
    };

    //////////////////////////////////////////////////////////////////////////
    // find
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct find_impl : find_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct find_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Key>
        static constexpr decltype(auto) apply(Xs&& xs, Key&& key) {
            return hana::find_if(static_cast<Xs&&>(xs),
                    hana::equal.to(static_cast<Key&&>(key)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // at_key
    //////////////////////////////////////////////////////////////////////////
    template <typename S, typename>
    struct at_key_impl : at_key_impl<S, when<true>> { };

    template <typename S, bool condition>
    struct at_key_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Key>
        static constexpr decltype(auto) apply(Xs&& xs, Key&& key) {
            return hana::from_just(hana::find(static_cast<Xs&&>(xs),
                                              static_cast<Key&&>(key)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // is_subset
    //////////////////////////////////////////////////////////////////////////
    template <typename S1, typename S2, typename>
    struct is_subset_impl : is_subset_impl<S1, S2, when<true>> { };

    template <typename S1, typename S2, bool condition>
    struct is_subset_impl<S1, S2, when<condition>> : default_ {
        static void apply(...);
    };

    template <typename S, bool condition>
    struct is_subset_impl<S, S, when<condition>> {
        template <typename Xs, typename Ys>
        static constexpr decltype(auto) apply(Xs&& xs, Ys&& ys) {
            return hana::all_of(static_cast<Xs&&>(xs),
                    hana::partial(contains, static_cast<Ys&&>(ys)));
        }
    };

    // Cross-type overload
    template <typename S1, typename S2>
    struct is_subset_impl<S1, S2, when<
        detail::has_nontrivial_common_embedding<Searchable, S1, S2>{}()
    >> {
        using C = typename common<S1, S2>::type;
        template <typename Xs, typename Ys>
        static constexpr decltype(auto) apply(Xs&& xs, Ys&& ys) {
            return hana::is_subset(hana::to<C>(static_cast<Xs&&>(xs)),
                                   hana::to<C>(static_cast<Ys&&>(ys)));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // models
    //////////////////////////////////////////////////////////////////////////
    template <typename S>
    struct models_impl<Searchable, S>
        : _integral_constant<bool,
            !is_default<any_of_impl<S>>{}() &&
            !is_default<find_if_impl<S>>{}()
        >
    { };

    //////////////////////////////////////////////////////////////////////////
    // Model for builtin arrays
    //////////////////////////////////////////////////////////////////////////
    template <typename T, std::size_t N>
    struct any_of_impl<T[N]> {
        template <typename Xs, typename Pred>
        static constexpr bool any_of_helper(bool cond, Xs&& xs, Pred&& pred) {
            if (cond) return true;
            for (std::size_t i = 1; i < N; ++i)
                if (pred(static_cast<Xs&&>(xs)[i]))
                    return true;
            return false;
        }

        // Since an array contains homogeneous data, if the predicate returns
        // a compile-time logical at any index, it must do so at every index
        // (because the type of the elements won't change)! In this case, we
        // then only need to evaluate the predicate on the first element.
        template <typename Xs, typename Pred>
        static constexpr auto
        any_of_helper(decltype(true_), Xs&& /*xs*/, Pred&&)
        { return true_; }

        template <typename Xs, typename Pred>
        static constexpr auto
        any_of_helper(decltype(false_), Xs&&, Pred&&)
        { return false_; }

        template <typename Xs, typename Pred>
        static constexpr decltype(auto) apply(Xs&& xs, Pred&& pred) {
            auto cond = hana::if_(pred(static_cast<Xs&&>(xs)[0]),
                true_, false_
            );
            return any_of_helper(cond, static_cast<Xs&&>(xs),
                                       static_cast<Pred&&>(pred));
        }
    };

    template <typename T, std::size_t N>
    struct find_if_impl<T[N]> {
        using Size = std::size_t;

        template <typename Xs, typename Pred, Size i>
        static constexpr decltype(auto)
        find_helper(Xs&& xs, Pred&& pred, _integral_constant<Size, i>, decltype(false_)) {
            auto cond = pred(static_cast<Xs&&>(xs)[i + 1]);
            constexpr bool truth_value = hana::if_(hana::value(cond), true, false);
            return find_helper(
                static_cast<Xs&&>(xs),
                static_cast<Pred&&>(pred),
                size_t<i + 1>, bool_<truth_value>
            );
        }

        template <typename Xs, typename Pred, Size i>
        static constexpr decltype(auto)
        find_helper(Xs&& xs, Pred&&, _integral_constant<Size, i>, decltype(true_))
        { return hana::just(static_cast<Xs&&>(xs)[i]); }

        template <typename Xs, typename Pred>
        static constexpr decltype(auto)
        find_helper(Xs&&, Pred&&, _integral_constant<Size, N-1>, decltype(false_))
        { return nothing; }

        template <typename Xs, typename Pred>
        static constexpr decltype(auto) apply(Xs&& xs, Pred&& pred) {
            auto cond = pred(static_cast<Xs&&>(xs)[0]);
            constexpr bool truth_value = hana::if_(hana::value(cond), true, false);
            return find_helper(
                static_cast<Xs&&>(xs),
                static_cast<Pred&&>(pred),
                size_t<0>, bool_<truth_value>
            );
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_SEARCHABLE_HPP
