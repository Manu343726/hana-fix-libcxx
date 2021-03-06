/*!
@file
Defines `boost::hana::Map`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_MAP_HPP
#define BOOST_HANA_MAP_HPP

#include <boost/hana/fwd/map.hpp>

#include <boost/hana/comparable.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/make.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/operators.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/detail/erase_key_fwd.hpp>
#include <boost/hana/detail/insert_fwd.hpp>
#include <boost/hana/detail/keys_fwd.hpp>
#include <boost/hana/foldable.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/functional/demux.hpp>
#include <boost/hana/functional/partial.hpp>
#include <boost/hana/functor.hpp>
#include <boost/hana/fwd/constant.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/logical.hpp>
#include <boost/hana/product.hpp>
#include <boost/hana/searchable.hpp>
#include <boost/hana/tuple.hpp>

#include <type_traits>


namespace boost { namespace hana {
    //////////////////////////////////////////////////////////////////////////
    // _map
    //////////////////////////////////////////////////////////////////////////
    template <typename ...Pairs>
    struct _map : operators::adl, operators::Searchable_ops<_map<Pairs...>> {
        _tuple<Pairs...> storage;
        using hana = _map;
        using datatype = Map;

        _map() = default;
        _map(_map const&) = default;
        _map(_map&&) = default;
        _map(_map&) = default;
        template <typename ...Ys>
        explicit constexpr _map(Ys&& ...ys)
            : storage{static_cast<Ys&&>(ys)...}
        { }
    };

    namespace operators {
        template <>
        struct of<Map>
            : operators::of<Searchable, Comparable>
        { };
    }

    //////////////////////////////////////////////////////////////////////////
    // make<Map>
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct make_impl<Map> {
        template <typename ...Pairs>
        static constexpr auto apply(Pairs&& ...pairs) {
        #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
            constexpr bool are_pairs[] = {true, _models<Product, Pairs>{}()...};

            static_assert(hana::all(are_pairs),
            "hana::make<Map>(pairs...) requires all the 'pairs' to be Products");

            constexpr bool are_Comparable[] = {true,
                _models<Comparable, decltype(hana::first(pairs))>{}()...
            };

            static_assert(hana::all(are_Comparable),
            "hana::make<Map>(pairs...) requires all the keys to be Comparable");

            constexpr bool are_compile_time_Comparable[] = {true,
                _models<Constant, decltype(
                    hana::equal(hana::first(pairs), hana::first(pairs))
                )>{}()...
            };

            static_assert(hana::all(are_compile_time_Comparable),
            "hana::make<Map>(pairs...) requires all the keys to be "
            "Comparable at compile-time");
        #endif

            return _map<typename std::decay<Pairs>::type...>{
                static_cast<Pairs&&>(pairs)...
            };
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // keys
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct keys_impl<Map> {
        template <typename Map>
        static constexpr decltype(auto) apply(Map&& map) {
            return hana::transform(static_cast<Map&&>(map).storage, first);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // values
    //////////////////////////////////////////////////////////////////////////
    //! @cond
    template <typename Map>
    constexpr decltype(auto) _values::operator()(Map&& map) const {
        return hana::transform(static_cast<Map&&>(map).storage, second);
    }
    //! @endcond

    //////////////////////////////////////////////////////////////////////////
    // insert
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct insert_impl<Map> {
        template <typename M, typename P>
        static constexpr typename std::decay<M>::type
        insert_helper(M&& map, P&&, decltype(true_))
        { return static_cast<M&&>(map); }

        template <typename M, typename P>
        static constexpr decltype(auto)
        insert_helper(M&& map, P&& pair, decltype(false_)) {
            return hana::unpack(
                hana::append(static_cast<M&&>(map).storage,
                             static_cast<P&&>(pair)),
                hana::make<Map>
            );
        }

        template <typename M, typename P>
        static constexpr decltype(auto) apply(M&& map, P&& pair) {
            constexpr bool contains = hana::value<decltype(
                hana::contains(map, hana::first(pair))
            )>();
            return insert_helper(static_cast<M&&>(map),
                                 static_cast<P&&>(pair),
                                 hana::bool_<contains>);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // erase_key
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct erase_key_impl<Map> {
        template <typename M, typename Key>
        static constexpr decltype(auto) apply(M&& map, Key&& key) {
            return hana::unpack(
                hana::remove_if(
                    static_cast<M&&>(map).storage,
                    hana::compose(
                        hana::equal.to(static_cast<Key&&>(key)),
                        hana::first
                    )
                ),
                make<Map>
            );
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Comparable
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct equal_impl<Map, Map> {
        template <typename M1, typename M2>
        static constexpr auto equal_helper(M1 const&, M2 const&, decltype(false_)) {
            return false_;
        }

        template <typename M1, typename M2>
        static constexpr auto equal_helper(M1 const& m1, M2 const& m2, decltype(true_)) {
            return hana::all_of(hana::keys(m1), hana::demux(equal)(
                hana::partial(find, m1),
                hana::partial(find, m2)
            ));
        }

        template <typename M1, typename M2>
        static constexpr auto apply(M1 const& m1, M2 const& m2) {
            constexpr bool same_length = hana::value<decltype(
                hana::equal(hana::length(m1.storage), hana::length(m2.storage))
            )>();
            return equal_helper(m1, m2, hana::bool_<same_length>);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Searchable
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct find_if_impl<Map> {
        template <typename M, typename Pred>
        static constexpr auto apply(M&& map, Pred&& pred) {
            return hana::transform(
                hana::find_if(static_cast<M&&>(map).storage,
                    hana::compose(static_cast<Pred&&>(pred), first)),
                second
            );
        }
    };

    template <>
    struct any_of_impl<Map> {
        template <typename M, typename Pred>
        static constexpr auto apply(M map, Pred pred)
        { return hana::any_of(hana::keys(map), pred); }
    };

    //////////////////////////////////////////////////////////////////////////
    // Foldable
    //////////////////////////////////////////////////////////////////////////
    template <>
    struct unpack_impl<Map> {
        template <typename M, typename F>
        static constexpr decltype(auto) apply(M&& map, F&& f) {
            return hana::unpack(static_cast<M&&>(map).storage,
                                static_cast<F&&>(f));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Construction from a Foldable
    //////////////////////////////////////////////////////////////////////////
    template <typename F>
    struct to_impl<Map, F, when<_models<Foldable, F>{}()>> {
        template <typename Xs>
        static constexpr decltype(auto) apply(Xs&& xs) {
            return hana::fold_left(
                static_cast<Xs&&>(xs), hana::make<Map>(), hana::insert
            );
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_MAP_HPP
