/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_TEST_LAWS_ITERABLE_HPP
#define BOOST_HANA_TEST_LAWS_ITERABLE_HPP

#include <boost/hana/assert.hpp>
#include <boost/hana/bool.hpp>
#include <boost/hana/comparable.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/core/operators.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/foldable.hpp>
#include <boost/hana/functional/capture.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/range.hpp>

#include <laws/base.hpp>


namespace boost { namespace hana { namespace test {
    template <typename It, typename = when<true>>
    struct TestIterable : TestIterable<It, laws> {
        using TestIterable<It, laws>::TestIterable;
    };

    template <typename It>
    struct TestIterable<It, laws> {
        template <typename Xs>
        TestIterable(Xs xs) {
            hana::for_each(xs, [](auto xs) {
                static_assert(_models<Iterable, decltype(xs)>{}, "");

                BOOST_HANA_CONSTANT_CHECK(
                    hana::is_empty(xs) ^iff^ hana::is_empty(hana::to<Tuple>(xs))
                );

                only_when_(hana::not_(hana::is_empty(xs)), hana::lazy([](auto xs) {
                    BOOST_HANA_CHECK(hana::equal(
                        hana::head(xs),
                        hana::head(hana::to<Tuple>(xs))
                    ));

                    BOOST_HANA_CHECK(hana::equal(
                        hana::to<Tuple>(hana::tail(xs)),
                        hana::tail(hana::to<Tuple>(xs))
                    ));

                    // methods
                    // drop(xs, 1) == tail(xs) unless xs is empty
                    BOOST_HANA_CHECK(hana::equal(
                        hana::drop(xs, size_t<1>),
                        hana::tail(xs)
                    ));

                    // last(xs) == at(xs, length(xs)-1)
                    BOOST_HANA_CHECK(hana::equal(
                        hana::last(xs),
                        hana::at(xs, hana::pred(hana::length(xs)))
                    ));

                })(xs));

                // drop(xs, 0) == xs
                BOOST_HANA_CHECK(hana::equal(
                    hana::drop(xs, size_t<0>),
                    xs
                ));

                // at(xs, n) == head(drop(xs, n))
                hana::for_each(hana::make_range(size_t<0>, hana::length(xs)),
                hana::capture(xs)([](auto xs, auto n) {
                    BOOST_HANA_CHECK(hana::equal(
                        hana::at(xs, n),
                        hana::head(hana::drop(xs, n))
                    ));
                }));

                // operators
                only_when_(has_operator<It, decltype(at)>, hana::lazy([](auto xs) {
                    hana::for_each(hana::make_range(size_t<0>, hana::length(xs)),
                    hana::capture(xs)([](auto xs, auto n) {
                        BOOST_HANA_CHECK(hana::equal(
                            hana::at(xs, n),
                            xs[n]
                        ));
                    }));
                })(xs));

                // Searchable
                hana::eval_if(hana::is_empty(xs),
                    hana::lazy([](auto xs) {
                        BOOST_HANA_CONSTANT_CHECK(
                            hana::not_(hana::any_of(xs, hana::always(true_)))
                        );

                        BOOST_HANA_CONSTANT_CHECK(hana::equal(
                            hana::find_if(xs, hana::always(true_)),
                            nothing
                        ));
                    })(xs),
                    hana::lazy([](auto xs) {
                        BOOST_HANA_CHECK(
                            hana::any_of(xs, hana::always(true_))
                        );
                        BOOST_HANA_CHECK(
                            hana::not_(hana::any_of(xs, hana::always(false_)))
                        );

                        BOOST_HANA_CHECK(hana::equal(
                            hana::find_if(xs, hana::always(true_)),
                            hana::just(hana::head(xs))
                        ));
                    })(xs)
                );

            });
        }
    };

    template <typename S>
    struct TestIterable<S, when<_models<Sequence, S>{}()>>
        : TestIterable<S, laws>
    {
        template <int i>
        using x = ct_eq<i>;

        template <int i = 0>
        struct invalid { };

        template <typename Xs>
        TestIterable(Xs xs) : TestIterable<S, laws>{xs} {
            constexpr auto list = make<S>;

            //////////////////////////////////////////////////////////////////
            // head
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                head(list(x<0>{})),
                x<0>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                head(list(x<0>{}, invalid<>{})),
                x<0>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                head(list(x<0>{}, invalid<1>{}, invalid<2>{})),
                x<0>{}
            ));

            BOOST_HANA_CONSTEXPR_CHECK(equal(
                head(list(1)), 1
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                head(list(1, '2')), 1
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                head(list(1, '2', 3.3)), 1
            ));

            //////////////////////////////////////////////////////////////////
            // is_empty
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(is_empty(list()));
            BOOST_HANA_CONSTANT_CHECK(not_(is_empty(list(invalid<>{}))));
            BOOST_HANA_CONSTANT_CHECK(not_(is_empty(list(invalid<0>{}, invalid<1>{}))));

            int i = 0; // non-constexpr
            BOOST_HANA_CONSTANT_CHECK(not_(is_empty(list(i))));
            BOOST_HANA_CONSTANT_CHECK(not_(is_empty(list(i, i))));

            //////////////////////////////////////////////////////////////////
            // tail
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                tail(list(invalid<>{}, x<0>{})), list(x<0>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                tail(list(invalid<>{}, x<0>{}, x<1>{})), list(x<0>{}, x<1>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                tail(list(invalid<>{}, x<0>{}, x<1>{}, x<2>{})), list(x<0>{}, x<1>{}, x<2>{})
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                tail(list(1)), list()
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                tail(list(1, '2')), list('2')
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                tail(list(1, '2', 3.3)), list('2', 3.3)
            ));


            //////////////////////////////////////////////////////////////////
            // at
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(x<0>{}), size_t<0>),
                x<0>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(x<0>{}, invalid<>{}), size_t<0>),
                x<0>{}
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(invalid<>{}, x<1>{}), size_t<1>),
                x<1>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(invalid<0>{}, x<1>{}, invalid<2>{}), size_t<1>),
                x<1>{}
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(invalid<0>{}, invalid<1>{}, x<2>{}), size_t<2>),
                x<2>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                at(list(invalid<0>{}, invalid<1>{}, x<2>{}, invalid<3>{}), size_t<2>),
                x<2>{}
            ));

            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1), size_t<0>),
                1
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1, '2'), size_t<0>),
                1
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1, '2', 3.3), size_t<0>),
                1
            ));

            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1, '2'), size_t<1>),
                '2'
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1, '2', 3.3), size_t<1>),
                '2'
            ));

            BOOST_HANA_CONSTEXPR_CHECK(equal(
                at(list(1, '2', 3.3), size_t<2>),
                3.3
            ));

            // make sure we can use non-pods on both sides
            at(list(Tracked{0}, x<1>{}, Tracked{1}), size_t<1>);


            //////////////////////////////////////////////////////////////////
            // last
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                last(list(x<0>{})),
                x<0>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                last(list(invalid<0>{}, x<1>{})),
                x<1>{}
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                last(list(invalid<0>{}, invalid<1>{}, x<2>{})),
                x<2>{}
            ));

            BOOST_HANA_CONSTEXPR_CHECK(equal(
                last(list(1)), 1
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                last(list(1, '2')), '2'
            ));
            BOOST_HANA_CONSTEXPR_CHECK(equal(
                last(list(1, '2', 3.3)), 3.3
            ));


            //////////////////////////////////////////////////////////////////
            // drop.at_most
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(), size_t<0>),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(), size_t<1>),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(), size_t<2>),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}), size_t<0>),
                list(x<0>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}), size_t<1>),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}), size_t<2>),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}, x<1>{}), size_t<0>),
                list(x<0>{}, x<1>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}, x<1>{}), size_t<1>),
                list(x<1>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.at_most(list(x<0>{}, x<1>{}), size_t<2>),
                list()
            ));

            // drop == drop.at_most
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop(list(x<0>{}, x<1>{}), size_t<1>),
                drop.at_most(list(x<0>{}, x<1>{}), size_t<1>)
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop(list(x<0>{}, x<1>{}), size_t<3>),
                drop.at_most(list(x<0>{}, x<1>{}), size_t<3>)
            ));

            //////////////////////////////////////////////////////////////////
            // drop.exactly
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(), size_t<0>),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}), size_t<0>),
                list(x<0>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}), size_t<1>),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}, x<1>{}), size_t<0>),
                list(x<0>{}, x<1>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}, x<1>{}), size_t<1>),
                list(x<1>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}, x<1>{}), size_t<2>),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop.exactly(list(x<0>{}, x<1>{}, x<2>{}, x<3>{},
                                  x<4>{}, x<5>{}, x<6>{}, x<7>{}), size_t<4>),
                list(x<4>{}, x<5>{}, x<6>{}, x<7>{})
            ));

            //////////////////////////////////////////////////////////////////
            // drop_until
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(), id),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(true_), id),
                list(true_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(false_), id),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(true_, true_), id),
                list(true_, true_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(true_, false_), id),
                list(true_, false_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(false_, true_), id),
                list(true_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_until(list(false_, false_), id),
                list()
            ));

            //////////////////////////////////////////////////////////////////
            // drop_while
            //////////////////////////////////////////////////////////////////
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(), id),
                list()
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(true_), id),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(false_), id),
                list(false_)
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(true_, true_), id),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(true_, false_), id),
                list(false_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(false_, true_), id),
                list(false_, true_)
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(false_, false_), id),
                list(false_, false_)
            ));

            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(x<0>{}, x<1>{}), not_equal.to(x<99>{})),
                list()
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(x<0>{}, x<1>{}, x<2>{}), not_equal.to(x<1>{})),
                list(x<1>{}, x<2>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(x<0>{}, x<1>{}, x<2>{}, x<3>{}), not_equal.to(x<3>{})),
                list(x<3>{})
            ));
            BOOST_HANA_CONSTANT_CHECK(equal(
                drop_while(list(x<0>{}, x<1>{}, x<2>{}, x<3>{}), not_equal.to(x<0>{})),
                list(x<0>{}, x<1>{}, x<2>{}, x<3>{})
            ));
        }
    };
}}} // end namespace boost::hana::test

#endif // !BOOST_HANA_TEST_LAWS_ITERABLE_HPP
