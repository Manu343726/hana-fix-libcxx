/*!
@file
Defines `boost::hana::detail::has_[nontrivial_]common_embedding`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_DETAIL_HAS_COMMON_EMBEDDING_HPP
#define BOOST_HANA_DETAIL_HAS_COMMON_EMBEDDING_HPP

#include <boost/hana/core/common.hpp>
#include <boost/hana/core/convert.hpp>
#include <boost/hana/core/models.hpp>

#include <type_traits>


namespace boost { namespace hana { namespace detail {
    template <typename Concept, typename T, typename U, typename = void>
    struct has_common_embedding_impl : std::false_type { };

    template <typename Concept, typename T, typename U>
    struct has_common_embedding_impl<Concept, T, U, detail::void_t<
        typename common<T, U>::type
    >> {
        using Common = typename common<T, U>::type;
        using type = std::integral_constant<bool,
            _models<Concept, T>{}() &&
            _models<Concept, U>{}() &&
            _models<Concept, Common>{}() &&
            is_embedded<T, Common>{}() &&
            is_embedded<U, Common>{}()
        >;
    };

    //! @ingroup group-details
    //! Returns whether `T` and `U` both have an embedding into a
    //! common type.
    //!
    //! If `T` and `U` do not have a common-type, this metafunction returns
    //! false.
    template <typename Concept, typename T, typename U>
    using has_common_embedding =
                    typename has_common_embedding_impl<Concept, T, U>::type;

    template <typename Concept, typename T, typename U>
    struct has_nontrivial_common_embedding_impl
        : has_common_embedding_impl<Concept, T, U>
    { };

    template <typename Concept, typename T>
    struct has_nontrivial_common_embedding_impl<Concept, T, T>
        : std::false_type
    { };

    //! @ingroup group-details
    //! Returns whether `T` and `U` are distinct and both have an embedding
    //! into a common type.
    //!
    //! If `T` and `U` do not have a common-type, this metafunction returns
    //! false.
    template <typename Concept, typename T, typename U>
    using has_nontrivial_common_embedding =
        typename has_nontrivial_common_embedding_impl<Concept, T, U>::type;
}}} // end namespace boost::hana::detail

#endif // !BOOST_HANA_DETAIL_HAS_COMMON_EMBEDDING_HPP
