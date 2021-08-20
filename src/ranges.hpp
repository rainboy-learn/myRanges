// Copyright (C) 2010-2020 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#include "ranges_concept.hpp"

            // [range.range] The range concept.
            // [range.sized] The sized_range concept.
            // Defined in <bits/range_access.h>

            // [range.refinements]
            // Defined in <bits/range_access.h>
namespace myranges {
    using std::copy_constructible;
    using std::is_nothrow_default_constructible_v;
    using std::is_nothrow_move_constructible_v;
    using std::in_place_t;
    using std::in_place;
    using std::incrementable;
    using std::totally_ordered;
    using std::weakly_incrementable;
    using std::semiregular;
    using std::unreachable_sentinel_t;
    using std::equality_comparable;
    using std::three_way_comparable;
    using std::indirect_unary_predicate;
    using std::random_access_iterator_tag;
    using std::bidirectional_iterator_tag;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::basic_istream;
    using std::default_sentinel_t;
    using std::type_identity_t;
    using std::totally_ordered_with;
    using std::unreachable_sentinel;
    using std::default_sentinel;
    using std::reference_wrapper;
    using std::is_default_constructible_v;
    using std::is_invocable_v;
    using std::remove_const_t;
    using std::unwrap_reference_t;
    using std::declval;
    using std::identity;
    using std::projected;
    using std::indirect_strict_weak_order;
    using std::ranges::less;
    using std::indirectly_comparable;
    using std::pair;
    using std::iterator_traits;
    using std::indirectly_swappable;
    using std::regular_invocable;
    using std::invoke_result_t;
    using std::contiguous_iterator_tag;
    using std::counted_iterator;
    using std::output_iterator_tag;
    using std::common_type_t;
    using std::reverse_iterator;
    using std::tuple_size_v;


    struct view_base {}; // 基础的 视图类， 是一处概念

    template<typename T> //T 是否从view_base 直接 public 继承
    inline constexpr bool enable_view = derived_from<T,view_base>;

    template<typename _Tp>
        concept view
        = range<_Tp> && movable<_Tp> && default_initializable<_Tp>
        && enable_view<_Tp>;

    /// A range which can be safely converted to a view.
    template<typename _Tp>
        concept viewable_range = range<_Tp>
        && (borrowed_range<_Tp> || view<remove_cvref_t<_Tp>>);

    namespace __detail
    {
        template<typename _Range>
            concept __simple_view = view<_Range> && range<const _Range>
            && same_as<iterator_t<_Range>, iterator_t<const _Range>>
            && same_as<sentinel_t<_Range>, sentinel_t<const _Range>>;

        template<typename _It>
            concept __has_arrow = input_iterator<_It>
            && (is_pointer_v<_It> || requires(_It __it) { __it.operator->(); });

        template<typename _Tp, typename _Up>
            concept __not_same_as
            = !same_as<remove_cvref_t<_Tp>, remove_cvref_t<_Up>>;
    } // namespace __detail

    template<typename _Derived>
        requires is_class_v<_Derived> && same_as<_Derived, remove_cv_t<_Derived>>
        class view_interface : public view_base
        {
            private:
                constexpr _Derived& _M_derived() noexcept
                {
                    static_assert(derived_from<_Derived, view_interface<_Derived>>);
                    static_assert(view<_Derived>);
                    return static_cast<_Derived&>(*this);
                }

                constexpr const _Derived& _M_derived() const noexcept
                {
                    static_assert(derived_from<_Derived, view_interface<_Derived>>);
                    static_assert(view<_Derived>);
                    return static_cast<const _Derived&>(*this);
                }

            public:
                constexpr bool
                    empty() requires forward_range<_Derived>
                    { return myranges::begin(_M_derived()) == myranges::end(_M_derived()); }

                constexpr bool
                    empty() const requires forward_range<const _Derived>
                    { return myranges::begin(_M_derived()) == myranges::end(_M_derived()); }

                constexpr explicit
                    operator bool() requires requires { myranges::empty(_M_derived()); }
                { return !myranges::empty(_M_derived()); }

                constexpr explicit
                    operator bool() const requires requires { myranges::empty(_M_derived()); }
                { return !myranges::empty(_M_derived()); }

                constexpr auto
                    data() requires contiguous_iterator<iterator_t<_Derived>>
                    { return to_address(myranges::begin(_M_derived())); }

                constexpr auto
                    data() const
                    requires range<const _Derived>
                    && contiguous_iterator<iterator_t<const _Derived>>
                    { return to_address(myranges::begin(_M_derived())); }

                constexpr auto
                    size()
                    requires forward_range<_Derived>
                    && sized_sentinel_for<sentinel_t<_Derived>, iterator_t<_Derived>>
                    { return myranges::end(_M_derived()) - myranges::begin(_M_derived()); }

                constexpr auto
                    size() const
                    requires forward_range<const _Derived>
                    && sized_sentinel_for<sentinel_t<const _Derived>,
                    iterator_t<const _Derived>>
                    { return myranges::end(_M_derived()) - myranges::begin(_M_derived()); }

                constexpr decltype(auto)
                    front() requires forward_range<_Derived>
                    {
                        __glibcxx_assert(!empty());
                        return *myranges::begin(_M_derived());
                    }

                constexpr decltype(auto)
                    front() const requires forward_range<const _Derived>
                    {
                        __glibcxx_assert(!empty());
                        return *myranges::begin(_M_derived());
                    }

                constexpr decltype(auto)
                    back()
                    requires bidirectional_range<_Derived> && common_range<_Derived>
                    {
                        __glibcxx_assert(!empty());
                        return *myranges::prev(myranges::end(_M_derived()));
                    }

                constexpr decltype(auto)
                    back() const
                    requires bidirectional_range<const _Derived>
                    && common_range<const _Derived>
                    {
                        __glibcxx_assert(!empty());
                        return *myranges::prev(myranges::end(_M_derived()));
                    }

                template<random_access_range _Range = _Derived>
                    constexpr decltype(auto)
                    operator[](range_difference_t<_Range> __n)
                    { return myranges::begin(_M_derived())[__n]; }

                template<random_access_range _Range = const _Derived>
                    constexpr decltype(auto)
                    operator[](range_difference_t<_Range> __n) const
                    { return myranges::begin(_M_derived())[__n]; }
        };

    namespace __detail
    {
        template<class _From, class _To>
            concept __convertible_to_non_slicing = convertible_to<_From, _To>
            && !(is_pointer_v<decay_t<_From>> && is_pointer_v<decay_t<_To>>
                    && __not_same_as<remove_pointer_t<decay_t<_From>>,
                    remove_pointer_t<decay_t<_To>>>);

        template<typename _Tp>
            concept __pair_like
            = !is_reference_v<_Tp> && requires(_Tp __t)
            {
                typename tuple_size<_Tp>::type;
                requires derived_from<tuple_size<_Tp>, integral_constant<size_t, 2>>;
                typename tuple_element_t<0, remove_const_t<_Tp>>;
                typename tuple_element_t<1, remove_const_t<_Tp>>;
                { get<0>(__t) } -> convertible_to<const tuple_element_t<0, _Tp>&>;
                { get<1>(__t) } -> convertible_to<const tuple_element_t<1, _Tp>&>;
            };

        template<typename _Tp, typename _Up, typename _Vp>
            concept __pair_like_convertible_from
            = !range<_Tp> && __pair_like<_Tp>
            && constructible_from<_Tp, _Up, _Vp>
            && __convertible_to_non_slicing<_Up, tuple_element_t<0, _Tp>>
            && convertible_to<_Vp, tuple_element_t<1, _Tp>>;

        template<typename _Tp>
            concept __iterator_sentinel_pair
            = !range<_Tp> && __pair_like<_Tp>
            && sentinel_for<tuple_element_t<1, _Tp>, tuple_element_t<0, _Tp>>;

    } // namespace __detail

    enum class subrange_kind : bool { unsized, sized };

    template<input_or_output_iterator _It, sentinel_for<_It> _Sent = _It,
        subrange_kind _Kind = sized_sentinel_for<_Sent, _It>
            ? subrange_kind::sized : subrange_kind::unsized>
            requires (_Kind == subrange_kind::sized || !sized_sentinel_for<_Sent, _It>)
            class subrange : public view_interface<subrange<_It, _Sent, _Kind>>
            {
                private:
                    // XXX: gcc complains when using constexpr here
                    static const bool _S_store_size
                        = _Kind == subrange_kind::sized && !sized_sentinel_for<_Sent, _It>;

                    _It _M_begin = _It();
                    _Sent _M_end = _Sent();

                    template<typename, bool = _S_store_size>
                        struct _Size
                        { };

                    template<typename _Tp>
                        struct _Size<_Tp, true>
                        { __detail::__make_unsigned_like_t<_Tp> _M_size; };

                    [[no_unique_address]] _Size<iter_difference_t<_It>> _M_size = {};

                public:
                    subrange() = default;

                    constexpr
                        subrange(__detail::__convertible_to_non_slicing<_It> auto __i, _Sent __s)
                        requires (!_S_store_size)
                        : _M_begin(std::move(__i)), _M_end(__s)
                        { }

                    constexpr
                        subrange(__detail::__convertible_to_non_slicing<_It> auto __i, _Sent __s,
                                __detail::__make_unsigned_like_t<iter_difference_t<_It>> __n)
                        requires (_Kind == subrange_kind::sized)
                        : _M_begin(std::move(__i)), _M_end(__s)
                        {
                            using __detail::__to_unsigned_like;
                            __glibcxx_assert(__n == __to_unsigned_like(myranges::distance(__i, __s)));
                            if constexpr (_S_store_size)
                                _M_size._M_size = __n;
                        }

                    template<__detail::__not_same_as<subrange> _Rng>
                        requires borrowed_range<_Rng>
                        && __detail::__convertible_to_non_slicing<iterator_t<_Rng>, _It>
                        && convertible_to<sentinel_t<_Rng>, _Sent>
                        constexpr
                        subrange(_Rng&& __r) requires _S_store_size && sized_range<_Rng>
                        : subrange{__r, myranges::size(__r)}
                    { }

                    template<__detail::__not_same_as<subrange> _Rng>
                        requires borrowed_range<_Rng>
                        && __detail::__convertible_to_non_slicing<iterator_t<_Rng>, _It>
                        && convertible_to<sentinel_t<_Rng>, _Sent>
                        constexpr
                        subrange(_Rng&& __r) requires (!_S_store_size)
                        : subrange{myranges::begin(__r), myranges::end(__r)}
                    { }

                    template<borrowed_range _Rng>
                        requires __detail::__convertible_to_non_slicing<iterator_t<_Rng>, _It>
                        && convertible_to<sentinel_t<_Rng>, _Sent>
                        constexpr
                        subrange(_Rng&& __r,
                                __detail::__make_unsigned_like_t<iter_difference_t<_It>> __n)
                        requires (_Kind == subrange_kind::sized)
                        : subrange{myranges::begin(__r), myranges::end(__r), __n}
                    { }

                    template<__detail::__not_same_as<subrange> _PairLike>
                        requires __detail::__pair_like_convertible_from<_PairLike, const _It&,
                                 const _Sent&>
                                     constexpr
                                     operator _PairLike() const
                                     { return _PairLike(_M_begin, _M_end); }

                    constexpr _It
                        begin() const requires copyable<_It>
                        { return _M_begin; }

                    [[nodiscard]] constexpr _It
                        begin() requires (!copyable<_It>)
                        { return std::move(_M_begin); }

                    constexpr _Sent end() const { return _M_end; }

                    constexpr bool empty() const { return _M_begin == _M_end; }

                    constexpr __detail::__make_unsigned_like_t<iter_difference_t<_It>>
                        size() const requires (_Kind == subrange_kind::sized)
                        {
                            if constexpr (_S_store_size)
                                return _M_size._M_size;
                            else
                                return __detail::__to_unsigned_like(_M_end - _M_begin);
                        }

                    [[nodiscard]] constexpr subrange
                        next(iter_difference_t<_It> __n = 1) const &
                            requires forward_iterator<_It>
                            {
                                auto __tmp = *this;
                                __tmp.advance(__n);
                                return __tmp;
                            }

                    [[nodiscard]] constexpr subrange
                        next(iter_difference_t<_It> __n = 1) &&
                        {
                            advance(__n);
                            return std::move(*this);
                        }

                    [[nodiscard]] constexpr subrange
                        prev(iter_difference_t<_It> __n = 1) const
                            requires bidirectional_iterator<_It>
                            {
                                auto __tmp = *this;
                                __tmp.advance(-__n);
                                return __tmp;
                            }

                    constexpr subrange&
                        advance(iter_difference_t<_It> __n)
                        {
                            // _GLIBCXX_RESOLVE_LIB_DEFECTS
                            // 3433. subrange::advance(n) has UB when n < 0
                            if constexpr (bidirectional_iterator<_It>)
                                if (__n < 0)
                                {
                                    myranges::advance(_M_begin, __n);
                                    if constexpr (_S_store_size)
                                        _M_size._M_size += __detail::__to_unsigned_like(-__n);
                                    return *this;
                                }

                            __glibcxx_assert(__n >= 0);
                            auto __d = __n - myranges::advance(_M_begin, __n, _M_end);
                            if constexpr (_S_store_size)
                                _M_size._M_size -= __detail::__to_unsigned_like(__d);
                            return *this;
                        }
            };

    template<input_or_output_iterator _It, sentinel_for<_It> _Sent>
        subrange(_It, _Sent) -> subrange<_It, _Sent>;

    template<input_or_output_iterator _It, sentinel_for<_It> _Sent>
        subrange(_It, _Sent,
                __detail::__make_unsigned_like_t<iter_difference_t<_It>>)
        -> subrange<_It, _Sent, subrange_kind::sized>;

    template<__detail::__iterator_sentinel_pair _Pr>
        subrange(_Pr)
        -> subrange<tuple_element_t<0, _Pr>, tuple_element_t<1, _Pr>>;

    template<__detail::__iterator_sentinel_pair _Pr>
        subrange(_Pr, __detail::__make_unsigned_like_t<iter_difference_t<
                tuple_element_t<0, _Pr>>>)
        -> subrange<tuple_element_t<0, _Pr>, tuple_element_t<1, _Pr>,
        subrange_kind::sized>;

    template<borrowed_range _Rng>
        subrange(_Rng&&)
        -> subrange<iterator_t<_Rng>, sentinel_t<_Rng>,
        (sized_range<_Rng>
         || sized_sentinel_for<sentinel_t<_Rng>, iterator_t<_Rng>>)
            ? subrange_kind::sized : subrange_kind::unsized>;

    template<borrowed_range _Rng>
        subrange(_Rng&&,
                __detail::__make_unsigned_like_t<range_difference_t<_Rng>>)
        -> subrange<iterator_t<_Rng>, sentinel_t<_Rng>, subrange_kind::sized>;

    template<size_t _Num, class _It, class _Sent, subrange_kind _Kind>
        requires (_Num < 2)
        constexpr auto
        get(const subrange<_It, _Sent, _Kind>& __r)
        {
            if constexpr (_Num == 0)
                return __r.begin();
            else
                return __r.end();
        }

    template<size_t _Num, class _It, class _Sent, subrange_kind _Kind>
        requires (_Num < 2)
        constexpr auto
        get(subrange<_It, _Sent, _Kind>&& __r)
        {
            if constexpr (_Num == 0)
                return __r.begin();
            else
                return __r.end();
        }

    template<input_or_output_iterator _It, sentinel_for<_It> _Sent,
        subrange_kind _Kind>
            inline constexpr bool
            enable_borrowed_range<subrange<_It, _Sent, _Kind>> = true;


}
namespace myranges
{
    /// Type returned by algorithms instead of a dangling iterator or subrange.
    struct dangling
    {
        constexpr dangling() noexcept = default;
        template<typename... _Args>
            constexpr dangling(_Args&&...) noexcept { }
    };

    template<range _Range>
        using borrowed_iterator_t = conditional_t<borrowed_range<_Range>,
              iterator_t<_Range>,
              dangling>;

    template<range _Range>
        using borrowed_subrange_t = conditional_t<borrowed_range<_Range>,
              subrange<iterator_t<_Range>>,
              dangling>;

    template<typename _Tp> requires is_object_v<_Tp>
        class empty_view
        : public view_interface<empty_view<_Tp>>
        {
            public:
                static constexpr _Tp* begin() noexcept { return nullptr; }
                static constexpr _Tp* end() noexcept { return nullptr; }
                static constexpr _Tp* data() noexcept { return nullptr; }
                static constexpr size_t size() noexcept { return 0; }
                static constexpr bool empty() noexcept { return true; }
        };

    template<typename _Tp>
        inline constexpr bool enable_borrowed_range<empty_view<_Tp>> = true;

    namespace __detail
    {
        template<copy_constructible _Tp> requires is_object_v<_Tp>
            struct __box : std::optional<_Tp>
        {
            using std::optional<_Tp>::optional;

            constexpr
                __box()
                noexcept(is_nothrow_default_constructible_v<_Tp>)
                requires default_initializable<_Tp>
                : std::optional<_Tp>{std::in_place}
            { }

            __box(const __box&) = default;
            __box(__box&&) = default;

            using std::optional<_Tp>::operator=;

            __box&
                operator=(const __box& __that)
                noexcept(is_nothrow_copy_constructible_v<_Tp>)
                requires (!assignable_from<_Tp&, const _Tp&>)
                {
                    if ((bool)__that)
                        this->emplace(*__that);
                    else
                        this->reset();
                    return *this;
                }

            __box&
                operator=(__box&& __that)
                noexcept(is_nothrow_move_constructible_v<_Tp>)
                requires (!assignable_from<_Tp&, _Tp>)
                {
                    if ((bool)__that)
                        this->emplace(std::move(*__that));
                    else
                        this->reset();
                    return *this;
                }
        };

    } // namespace __detail

    /// A view that contains exactly one element.
    template<copy_constructible _Tp> requires is_object_v<_Tp>
        class single_view : public view_interface<single_view<_Tp>>
    {
        public:
            single_view() = default;

            constexpr explicit
                single_view(const _Tp& __t)
                : _M_value(__t)
                { }

            constexpr explicit
                single_view(_Tp&& __t)
                : _M_value(std::move(__t))
                { }

            template<typename... _Args>
                requires constructible_from<_Tp, _Args...>
                constexpr
                single_view(in_place_t, _Args&&... __args)
                : _M_value{in_place, std::forward<_Args>(__args)...}
            { }

            constexpr _Tp*
                begin() noexcept
                { return data(); }

            constexpr const _Tp*
                begin() const noexcept
                { return data(); }

            constexpr _Tp*
                end() noexcept
                { return data() + 1; }

            constexpr const _Tp*
                end() const noexcept
                { return data() + 1; }

            static constexpr size_t
                size() noexcept
                { return 1; }

            constexpr _Tp*
                data() noexcept
                { return _M_value.operator->(); }

            constexpr const _Tp*
                data() const noexcept
                { return _M_value.operator->(); }

        private:
            __detail::__box<_Tp> _M_value;
    };

    namespace __detail
    {
        template<typename _Wp>
            constexpr auto __to_signed_like(_Wp __w) noexcept
            {
                if constexpr (!integral<_Wp>)
                    return iter_difference_t<_Wp>();
                else if constexpr (sizeof(iter_difference_t<_Wp>) > sizeof(_Wp))
                    return iter_difference_t<_Wp>(__w);
                else if constexpr (sizeof(ptrdiff_t) > sizeof(_Wp))
                    return ptrdiff_t(__w);
                else if constexpr (sizeof(long long) > sizeof(_Wp))
                    return (long long)(__w);
#ifdef __SIZEOF_INT128__
                else if constexpr (__SIZEOF_INT128__ > sizeof(_Wp))
                    return __int128(__w);
#endif
                else
                    return __max_diff_type(__w);
            }

        template<typename _Wp>
            using __iota_diff_t = decltype(__to_signed_like(std::declval<_Wp>()));

        template<typename _It>
            concept __decrementable = incrementable<_It>
            && requires(_It __i)
            {
                { --__i } -> same_as<_It&>;
                { __i-- } -> same_as<_It>;
            };

        template<typename _It>
            concept __advanceable = __decrementable<_It> && totally_ordered<_It>
            && requires( _It __i, const _It __j, const __iota_diff_t<_It> __n)
            {
                { __i += __n } -> same_as<_It&>;
                { __i -= __n } -> same_as<_It&>;
                _It(__j + __n);
                _It(__n + __j);
                _It(__j - __n);
                { __j - __j } -> convertible_to<__iota_diff_t<_It>>;
            };

    } // namespace __detail

    template<weakly_incrementable _Winc,
        semiregular _Bound = unreachable_sentinel_t>
            requires std::__detail::__weakly_eq_cmp_with<_Winc, _Bound>
            && semiregular<_Winc>
            class iota_view : public view_interface<iota_view<_Winc, _Bound>>
            {
                private:
                    struct _Sentinel;

                    struct _Iterator
                    {
                        private:
                            static auto
                                _S_iter_cat()
                                {
                                    using namespace __detail;
                                    if constexpr (__advanceable<_Winc>)
                                        return random_access_iterator_tag{};
                                    else if constexpr (__decrementable<_Winc>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (incrementable<_Winc>)
                                        return forward_iterator_tag{};
                                    else
                                        return input_iterator_tag{};
                                }

                        public:
                            using iterator_category = decltype(_S_iter_cat());
                            using value_type = _Winc;
                            using difference_type = __detail::__iota_diff_t<_Winc>;

                            _Iterator() = default;

                            constexpr explicit
                                _Iterator(_Winc __value)
                                : _M_value(__value) { }

                            constexpr _Winc
                                operator*() const noexcept(is_nothrow_copy_constructible_v<_Winc>)
                                { return _M_value; }

                            constexpr _Iterator&
                                operator++()
                                {
                                    ++_M_value;
                                    return *this;
                                }

                            constexpr void
                                operator++(int)
                                { ++*this; }

                            constexpr _Iterator
                                operator++(int) requires incrementable<_Winc>
                                {
                                    auto __tmp = *this;
                                    ++*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator--() requires __detail::__decrementable<_Winc>
                                {
                                    --_M_value;
                                    return *this;
                                }

                            constexpr _Iterator
                                operator--(int) requires __detail::__decrementable<_Winc>
                                {
                                    auto __tmp = *this;
                                    --*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator+=(difference_type __n) requires __detail::__advanceable<_Winc>
                                {
                                    using __detail::__is_integer_like;
                                    using __detail::__is_signed_integer_like;
                                    if constexpr (__is_integer_like<_Winc>
                                            && !__is_signed_integer_like<_Winc>)
                                    {
                                        if (__n >= difference_type(0))
                                            _M_value += static_cast<_Winc>(__n);
                                        else
                                            _M_value -= static_cast<_Winc>(-__n);
                                    }
                                    else
                                        _M_value += __n;
                                    return *this;
                                }

                            constexpr _Iterator&
                                operator-=(difference_type __n) requires __detail::__advanceable<_Winc>
                                {
                                    using __detail::__is_integer_like;
                                    using __detail::__is_signed_integer_like;
                                    if constexpr (__is_integer_like<_Winc>
                                            && !__is_signed_integer_like<_Winc>)
                                    {
                                        if (__n >= difference_type(0))
                                            _M_value -= static_cast<_Winc>(__n);
                                        else
                                            _M_value += static_cast<_Winc>(-__n);
                                    }
                                    else
                                        _M_value -= __n;
                                    return *this;
                                }

                            constexpr _Winc
                                operator[](difference_type __n) const
                                requires __detail::__advanceable<_Winc>
                                { return _Winc(_M_value + __n); }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Iterator& __y)
                                requires equality_comparable<_Winc>
                                { return __x._M_value == __y._M_value; }

                            friend constexpr bool
                                operator<(const _Iterator& __x, const _Iterator& __y)
                                requires totally_ordered<_Winc>
                                { return __x._M_value < __y._M_value; }

                            friend constexpr bool
                                operator>(const _Iterator& __x, const _Iterator& __y)
                                requires totally_ordered<_Winc>
                                { return __y < __x; }

                            friend constexpr bool
                                operator<=(const _Iterator& __x, const _Iterator& __y)
                                requires totally_ordered<_Winc>
                                { return !(__y < __x); }

                            friend constexpr bool
                                operator>=(const _Iterator& __x, const _Iterator& __y)
                                requires totally_ordered<_Winc>
                                { return !(__x < __y); }

#ifdef __cpp_lib_three_way_comparison
                            friend constexpr auto
                                operator<=>(const _Iterator& __x, const _Iterator& __y)
                                requires totally_ordered<_Winc> && three_way_comparable<_Winc>
                                { return __x._M_value <=> __y._M_value; }
#endif

                            friend constexpr _Iterator
                                operator+(_Iterator __i, difference_type __n)
                                requires __detail::__advanceable<_Winc>
                                { return __i += __n; }

                            friend constexpr _Iterator
                                operator+(difference_type __n, _Iterator __i)
                                requires __detail::__advanceable<_Winc>
                                { return __i += __n; }

                            friend constexpr _Iterator
                                operator-(_Iterator __i, difference_type __n)
                                requires __detail::__advanceable<_Winc>
                                { return __i -= __n; }

                            friend constexpr difference_type
                                operator-(const _Iterator& __x, const _Iterator& __y)
                                requires __detail::__advanceable<_Winc>
                                {
                                    using __detail::__is_integer_like;
                                    using __detail::__is_signed_integer_like;
                                    using _Dt = difference_type;
                                    if constexpr (__is_integer_like<_Winc>)
                                    {
                                        if constexpr (__is_signed_integer_like<_Winc>)
                                            return _Dt(_Dt(__x._M_value) - _Dt(__y._M_value));
                                        else
                                            return (__y._M_value > __x._M_value)
                                                ? _Dt(-_Dt(__y._M_value - __x._M_value))
                                                : _Dt(__x._M_value - __y._M_value);
                                    }
                                    else
                                        return __x._M_value - __y._M_value;
                                }

                        private:
                            _Winc _M_value = _Winc();

                            friend _Sentinel;
                    };

                    struct _Sentinel
                    {
                        private:
                            constexpr bool
                                _M_equal(const _Iterator& __x) const
                                { return __x._M_value == _M_bound; }

                            _Bound _M_bound = _Bound();

                        public:
                            _Sentinel() = default;

                            constexpr explicit
                                _Sentinel(_Bound __bound)
                                : _M_bound(__bound) { }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Sentinel& __y)
                                { return __y._M_equal(__x); }

                            friend constexpr iter_difference_t<_Winc>
                                operator-(const _Iterator& __x, const _Sentinel& __y)
                                requires sized_sentinel_for<_Bound, _Winc>
                                { return __x._M_value - __y._M_bound; }

                            friend constexpr iter_difference_t<_Winc>
                                operator-(const _Sentinel& __x, const _Iterator& __y)
                                requires sized_sentinel_for<_Bound, _Winc>
                                { return -(__y - __x); }
                    };

                    _Winc _M_value = _Winc();
                    _Bound _M_bound = _Bound();

                public:
                    iota_view() = default;

                    constexpr explicit
                        iota_view(_Winc __value)
                        : _M_value(__value)
                        { }

                    constexpr
                        iota_view(type_identity_t<_Winc> __value,
                                type_identity_t<_Bound> __bound)
                        : _M_value(__value), _M_bound(__bound)
                        {
                            if constexpr (totally_ordered_with<_Winc, _Bound>)
                            {
                                __glibcxx_assert( bool(__value <= __bound) );
                            }
                        }

                    constexpr _Iterator
                        begin() const { return _Iterator{_M_value}; }

                    constexpr auto
                        end() const
                        {
                            if constexpr (same_as<_Bound, unreachable_sentinel_t>)
                                return unreachable_sentinel;
                            else
                                return _Sentinel{_M_bound};
                        }

                    constexpr _Iterator
                        end() const requires same_as<_Winc, _Bound>
                        { return _Iterator{_M_bound}; }

                    constexpr auto
                        size() const
                        requires (same_as<_Winc, _Bound> && __detail::__advanceable<_Winc>)
                        || (integral<_Winc> && integral<_Bound>)
                        || sized_sentinel_for<_Bound, _Winc>
                        {
                            using __detail::__is_integer_like;
                            using __detail::__to_unsigned_like;
                            if constexpr (__is_integer_like<_Winc> && __is_integer_like<_Bound>)
                                return (_M_value < 0)
                                    ? ((_M_bound < 0)
                                            ? __to_unsigned_like(-_M_value) - __to_unsigned_like(-_M_bound)
                                            : __to_unsigned_like(_M_bound) + __to_unsigned_like(-_M_value))
                                    : __to_unsigned_like(_M_bound) - __to_unsigned_like(_M_value);
                            else
                                return __to_unsigned_like(_M_bound - _M_value);
                        }
            };

    template<typename _Winc, typename _Bound>
        requires (!__detail::__is_integer_like<_Winc>
                || !__detail::__is_integer_like<_Bound>
                || (__detail::__is_signed_integer_like<_Winc>
                    == __detail::__is_signed_integer_like<_Bound>))
        iota_view(_Winc, _Bound) -> iota_view<_Winc, _Bound>;

    template<weakly_incrementable _Winc, semiregular _Bound>
        inline constexpr bool
        enable_borrowed_range<iota_view<_Winc, _Bound>> = true;

    namespace views
    {
        template<typename _Tp>
            inline constexpr empty_view<_Tp> empty{};

        struct _Single
        {
            template<typename _Tp>
                constexpr auto
                operator()(_Tp&& __e) const
                { return single_view{std::forward<_Tp>(__e)}; }
        };

        inline constexpr _Single single{};

        struct _Iota
        {
            template<typename _Tp>
                constexpr auto
                operator()(_Tp&& __e) const
                { return iota_view{std::forward<_Tp>(__e)}; }

            template<typename _Tp, typename _Up>
                constexpr auto
                operator()(_Tp&& __e, _Up&& __f) const
                { return iota_view{std::forward<_Tp>(__e), std::forward<_Up>(__f)}; }
        };

        inline constexpr _Iota iota{};
    } // namespace views

    namespace __detail
    {
        template<typename _Val, typename _CharT, typename _Traits>
            concept __stream_extractable
            = requires(basic_istream<_CharT, _Traits>& is, _Val& t) { is >> t; };
    } // namespace __detail

    template<movable _Val, typename _CharT, typename _Traits>
        requires default_initializable<_Val>
        && __detail::__stream_extractable<_Val, _CharT, _Traits>
        class basic_istream_view
        : public view_interface<basic_istream_view<_Val, _CharT, _Traits>>
        {
            public:
                basic_istream_view() = default;

                constexpr explicit
                    basic_istream_view(basic_istream<_CharT, _Traits>& __stream)
                    : _M_stream(std::__addressof(__stream))
                    { }

                constexpr auto
                    begin()
                    {
                        if (_M_stream != nullptr)
                            *_M_stream >> _M_object;
                        return _Iterator{*this};
                    }

                constexpr default_sentinel_t
                    end() const noexcept
                    { return default_sentinel; }

            private:
                basic_istream<_CharT, _Traits>* _M_stream = nullptr;
                _Val _M_object = _Val();

                struct _Iterator
                {
                    public:
                        using iterator_concept = input_iterator_tag;
                        using difference_type = ptrdiff_t;
                        using value_type = _Val;

                        _Iterator() = default;

                        constexpr explicit
                            _Iterator(basic_istream_view& __parent) noexcept
                            : _M_parent(std::__addressof(__parent))
                            { }

                        _Iterator(const _Iterator&) = delete;
                        _Iterator(_Iterator&&) = default;
                        _Iterator& operator=(const _Iterator&) = delete;
                        _Iterator& operator=(_Iterator&&) = default;

                        _Iterator&
                            operator++()
                            {
                                __glibcxx_assert(_M_parent->_M_stream != nullptr);
                                *_M_parent->_M_stream >> _M_parent->_M_object;
                                return *this;
                            }

                        void
                            operator++(int)
                            { ++*this; }

                        _Val&
                            operator*() const
                            {
                                __glibcxx_assert(_M_parent->_M_stream != nullptr);
                                return _M_parent->_M_object;
                            }

                        friend bool
                            operator==(const _Iterator& __x, default_sentinel_t)
                            { return __x._M_at_end(); }

                    private:
                        basic_istream_view* _M_parent = nullptr;

                        bool
                            _M_at_end() const
                            { return _M_parent == nullptr || !*_M_parent->_M_stream; }
                };

                friend _Iterator;
        };

    template<typename _Val, typename _CharT, typename _Traits>
        basic_istream_view<_Val, _CharT, _Traits>
        istream_view(basic_istream<_CharT, _Traits>& __s)
        { return basic_istream_view<_Val, _CharT, _Traits>{__s}; }

    namespace __detail
    {
        struct _Empty { };

        // Alias for a type that is conditionally present
        // (and is an empty type otherwise).
        // Data members using this alias should use [[no_unique_address]] so that
        // they take no space when not needed.
        template<bool _Present, typename _Tp>
            using __maybe_present_t = conditional_t<_Present, _Tp, _Empty>;

        // Alias for a type that is conditionally const.
        template<bool _Const, typename _Tp>
            using __maybe_const_t = conditional_t<_Const, const _Tp, _Tp>;

    } // namespace __detail

    namespace views
    {
        namespace __adaptor
        {
            template<typename _Tp>
                inline constexpr auto
                __maybe_refwrap(_Tp& __arg)
                { return reference_wrapper<_Tp>{__arg}; }

            template<typename _Tp>
                inline constexpr auto
                __maybe_refwrap(const _Tp& __arg)
                { return reference_wrapper<const _Tp>{__arg}; }

            template<typename _Tp>
                inline constexpr decltype(auto)
                __maybe_refwrap(_Tp&& __arg)
                { return std::forward<_Tp>(__arg); }

            template<typename _Callable>
                struct _RangeAdaptorClosure;

            template<typename _Callable>
                struct _RangeAdaptor
                {
                    protected:
                        [[no_unique_address]]
                            __detail::__maybe_present_t<!is_default_constructible_v<_Callable>,
                            _Callable> _M_callable;

                    public:
                        constexpr
                            _RangeAdaptor(const _Callable& = {})
                            requires is_default_constructible_v<_Callable>
                            { }

                        constexpr
                            _RangeAdaptor(_Callable __callable)
                            requires (!is_default_constructible_v<_Callable>)
                            : _M_callable(std::move(__callable))
                            { }

                        template<typename... _Args>
                            requires (sizeof...(_Args) >= 1)
                            constexpr auto
                            operator()(_Args&&... __args) const
                            {
                                // [range.adaptor.object]: If a range adaptor object accepts more
                                // than one argument, then the following expressions are equivalent:
                                //
                                //   (1) adaptor(range, args...)
                                //   (2) adaptor(args...)(range)
                                //   (3) range | adaptor(args...)
                                //
                                // In this case, adaptor(args...) is a range adaptor closure object.
                                //
                                // We handle (1) and (2) here, and (3) is just a special case of a
                                // more general case already handled by _RangeAdaptorClosure.
                                if constexpr (is_invocable_v<_Callable, _Args...>)
                                {
                                    static_assert(sizeof...(_Args) != 1,
                                            "a _RangeAdaptor that accepts only one argument "
                                            "should be defined as a _RangeAdaptorClosure");
                                    // Here we handle adaptor(range, args...) -- just forward all
                                    // arguments to the underlying adaptor routine.
                                    return _Callable{}(std::forward<_Args>(__args)...);
                                }
                                else
                                {
                                    // Here we handle adaptor(args...)(range).
                                    // Given args..., we return a _RangeAdaptorClosure that takes a
                                    // range argument, such that (2) is equivalent to (1).
                                    //
                                    // We need to be careful about how we capture args... in this
                                    // closure.  By using __maybe_refwrap, we capture lvalue
                                    // references by reference (through a reference_wrapper) and
                                    // otherwise capture by value.
                                    auto __closure
                                        = [...__args(__maybe_refwrap(std::forward<_Args>(__args)))]
                                        <typename _Range> (_Range&& __r) {
                                            // This static_cast has two purposes: it forwards a
                                            // reference_wrapper<T> capture as a T&, and otherwise
                                            // forwards the captured argument as an rvalue.
                                            return _Callable{}(std::forward<_Range>(__r),
                                                    (static_cast<unwrap_reference_t
                                                     <remove_const_t<decltype(__args)>>>
                                                     (__args))...);
                                        };
                                    using _ClosureType = decltype(__closure);
                                    return _RangeAdaptorClosure<_ClosureType>(std::move(__closure));
                                }
                            }
                };

            template<typename _Callable>
                _RangeAdaptor(_Callable) -> _RangeAdaptor<_Callable>;

            template<typename _Callable>
                struct _RangeAdaptorClosure : public _RangeAdaptor<_Callable>
            {
                using _RangeAdaptor<_Callable>::_RangeAdaptor;

                template<viewable_range _Range>
                    requires requires { declval<_Callable>()(declval<_Range>()); }
                constexpr auto
                    operator()(_Range&& __r) const
                    {
                        if constexpr (is_default_constructible_v<_Callable>)
                            return _Callable{}(std::forward<_Range>(__r));
                        else
                            return this->_M_callable(std::forward<_Range>(__r));
                    }

                template<viewable_range _Range>
                    requires requires { declval<_Callable>()(declval<_Range>()); }
                friend constexpr auto
                    operator|(_Range&& __r, const _RangeAdaptorClosure& __o)
                    { return __o(std::forward<_Range>(__r)); }

                template<typename _Tp>
                    friend constexpr auto
                    operator|(const _RangeAdaptorClosure<_Tp>& __x,
                            const _RangeAdaptorClosure& __y)
                    {
                        if constexpr (is_default_constructible_v<_Tp>
                                && is_default_constructible_v<_Callable>)
                        {
                            auto __closure = [] <typename _Up> (_Up&& __e) {
                                return std::forward<_Up>(__e) | decltype(__x){} | decltype(__y){};
                            };
                            return _RangeAdaptorClosure<decltype(__closure)>(__closure);
                        }
                        else if constexpr (is_default_constructible_v<_Tp>
                                && !is_default_constructible_v<_Callable>)
                        {
                            auto __closure = [__y] <typename _Up> (_Up&& __e) {
                                return std::forward<_Up>(__e) | decltype(__x){} | __y;
                            };
                            return _RangeAdaptorClosure<decltype(__closure)>(__closure);
                        }
                        else if constexpr (!is_default_constructible_v<_Tp>
                                && is_default_constructible_v<_Callable>)
                        {
                            auto __closure = [__x] <typename _Up> (_Up&& __e) {
                                return std::forward<_Up>(__e) | __x | decltype(__y){};
                            };
                            return _RangeAdaptorClosure<decltype(__closure)>(__closure);
                        }
                        else
                        {
                            auto __closure = [__x, __y] <typename _Up> (_Up&& __e) {
                                return std::forward<_Up>(__e) | __x | __y;
                            };
                            return _RangeAdaptorClosure<decltype(__closure)>(__closure);
                        }
                    }
            };

            template<typename _Callable>
                _RangeAdaptorClosure(_Callable) -> _RangeAdaptorClosure<_Callable>;
        } // namespace __adaptor
    } // namespace views

    template<range _Range> requires is_object_v<_Range>
        class ref_view : public view_interface<ref_view<_Range>>
    {
        private:
            _Range* _M_r = nullptr;

            static void _S_fun(_Range&); // not defined
            static void _S_fun(_Range&&) = delete;

        public:
            constexpr
                ref_view() noexcept = default;

            template<__detail::__not_same_as<ref_view> _Tp>
                requires convertible_to<_Tp, _Range&>
                && requires { _S_fun(declval<_Tp>()); }
            constexpr
                ref_view(_Tp&& __t)
                : _M_r(std::__addressof(static_cast<_Range&>(std::forward<_Tp>(__t))))
                { }

            constexpr _Range&
                base() const
                { return *_M_r; }

            constexpr iterator_t<_Range>
                begin() const
                { return myranges::begin(*_M_r); }

            constexpr sentinel_t<_Range>
                end() const
                { return myranges::end(*_M_r); }

            constexpr bool
                empty() const requires requires { myranges::empty(*_M_r); }
            { return myranges::empty(*_M_r); }

            constexpr auto
                size() const requires sized_range<_Range>
                { return myranges::size(*_M_r); }

            constexpr auto
                data() const requires contiguous_range<_Range>
                { return myranges::data(*_M_r); }
    };

    template<typename _Range>
        ref_view(_Range&) -> ref_view<_Range>;

    template<typename _Tp>
        inline constexpr bool enable_borrowed_range<ref_view<_Tp>> = true;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptorClosure all
            = [] <viewable_range _Range> (_Range&& __r)
            {
                if constexpr (view<decay_t<_Range>>)
                    return std::forward<_Range>(__r);
                else if constexpr (requires { ref_view{std::forward<_Range>(__r)}; })
                    return ref_view{std::forward<_Range>(__r)};
                else
                    return subrange{std::forward<_Range>(__r)};
            };

        template<viewable_range _Range>
            using all_t = decltype(all(std::declval<_Range>()));

    } // namespace views

    // XXX: the following algos are copied from ranges_algo.h to avoid a circular
    // dependency with that header.
    namespace __detail
    {
        template<input_iterator _Iter, sentinel_for<_Iter> _Sent,
            typename _Proj = identity,
            indirect_unary_predicate<projected<_Iter, _Proj>> _Pred>
                constexpr _Iter
                find_if(_Iter __first, _Sent __last, _Pred __pred, _Proj __proj = {})
                {
                    while (__first != __last
                            && !(bool)std::__invoke(__pred, std::__invoke(__proj, *__first)))
                        ++__first;
                    return __first;
                }

        template<input_iterator _Iter, sentinel_for<_Iter> _Sent,
            typename _Proj = identity,
            indirect_unary_predicate<projected<_Iter, _Proj>> _Pred>
                constexpr _Iter
                find_if_not(_Iter __first, _Sent __last, _Pred __pred, _Proj __proj = {})
                {
                    while (__first != __last
                            && (bool)std::__invoke(__pred, std::__invoke(__proj, *__first)))
                        ++__first;
                    return __first;
                }

        template<typename _Tp, typename _Proj = identity,
            indirect_strict_weak_order<projected<const _Tp*, _Proj>>
                _Comp = myranges::less>
                constexpr const _Tp&
                min(const _Tp& __a, const _Tp& __b, _Comp __comp = {}, _Proj __proj = {})
                {
                    if (std::__invoke(std::move(__comp),
                                std::__invoke(__proj, __b),
                                std::__invoke(__proj, __a)))
                        return __b;
                    else
                        return __a;
                }

        template<input_iterator _Iter1, sentinel_for<_Iter1> _Sent1,
            input_iterator _Iter2, sentinel_for<_Iter2> _Sent2,
            typename _Pred = std::ranges::equal_to,
            typename _Proj1 = identity, typename _Proj2 = identity>
                requires indirectly_comparable<_Iter1, _Iter2, _Pred, _Proj1, _Proj2>
                constexpr pair<_Iter1, _Iter2>
                mismatch(_Iter1 __first1, _Sent1 __last1, _Iter2 __first2, _Sent2 __last2,
                        _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {})
                {
                    while (__first1 != __last1 && __first2 != __last2
                            && (bool)std::__invoke(__pred,
                                std::__invoke(__proj1, *__first1),
                                std::__invoke(__proj2, *__first2)))
                    {
                        ++__first1;
                        ++__first2;
                    }
                    return { std::move(__first1), std::move(__first2) };
                }
    } // namespace __detail

    namespace __detail
    {
        template<range _Range>
            struct _CachedPosition
            {
                constexpr bool
                    _M_has_value() const
                    { return false; }

                constexpr iterator_t<_Range>
                    _M_get(const _Range&) const
                    {
                        __glibcxx_assert(false);
                        return {};
                    }

                constexpr void
                    _M_set(const _Range&, const iterator_t<_Range>&) const
                    { }
            };

        template<forward_range _Range>
            struct _CachedPosition<_Range>
            {
                private:
                    iterator_t<_Range> _M_iter{};

                public:
                    constexpr bool
                        _M_has_value() const
                        { return _M_iter != iterator_t<_Range>{}; }

                    constexpr iterator_t<_Range>
                        _M_get(const _Range&) const
                        {
                            __glibcxx_assert(_M_has_value());
                            return _M_iter;
                        }

                    constexpr void
                        _M_set(const _Range&, const iterator_t<_Range>& __it)
                        {
                            __glibcxx_assert(!_M_has_value());
                            _M_iter = __it;
                        }
            };

        template<random_access_range _Range>
            requires (sizeof(range_difference_t<_Range>)
                    <= sizeof(iterator_t<_Range>))
            struct _CachedPosition<_Range>
            {
                private:
                    range_difference_t<_Range> _M_offset = -1;

                public:
                    constexpr bool
                        _M_has_value() const
                        { return _M_offset >= 0; }

                    constexpr iterator_t<_Range>
                        _M_get(_Range& __r) const
                        {
                            __glibcxx_assert(_M_has_value());
                            return myranges::begin(__r) + _M_offset;
                        }

                    constexpr void
                        _M_set(_Range& __r, const iterator_t<_Range>& __it)
                        {
                            __glibcxx_assert(!_M_has_value());
                            _M_offset = __it - myranges::begin(__r);
                        }
            };

    } // namespace __detail

    template<input_range _Vp,
        indirect_unary_predicate<iterator_t<_Vp>> _Pred>
            requires view<_Vp> && is_object_v<_Pred>
            class filter_view : public view_interface<filter_view<_Vp, _Pred>>
            {
                private:
                    struct _Sentinel;

                    struct _Iterator
                    {
                        private:
                            static constexpr auto
                                _S_iter_concept()
                                {
                                    if constexpr (bidirectional_range<_Vp>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (forward_range<_Vp>)
                                        return forward_iterator_tag{};
                                    else
                                        return input_iterator_tag{};
                                }

                            static constexpr auto
                                _S_iter_cat()
                                {
                                    using _Cat = typename iterator_traits<_Vp_iter>::iterator_category;
                                    if constexpr (derived_from<_Cat, bidirectional_iterator_tag>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (derived_from<_Cat, forward_iterator_tag>)
                                        return forward_iterator_tag{};
                                    else
                                        return _Cat{};
                                }

                            friend filter_view;

                            using _Vp_iter = iterator_t<_Vp>;

                            _Vp_iter _M_current = _Vp_iter();
                            filter_view* _M_parent = nullptr;

                        public:
                            using iterator_concept = decltype(_S_iter_concept());
                            using iterator_category = decltype(_S_iter_cat());
                            using value_type = range_value_t<_Vp>;
                            using difference_type = range_difference_t<_Vp>;

                            _Iterator() = default;

                            constexpr
                                _Iterator(filter_view& __parent, _Vp_iter __current)
                                : _M_current(std::move(__current)),
                                _M_parent(std::__addressof(__parent))
                        { }

                            constexpr _Vp_iter
                                base() const &
                                requires copyable<_Vp_iter>
                                { return _M_current; }

                            constexpr _Vp_iter
                                base() &&
                                { return std::move(_M_current); }

                            constexpr range_reference_t<_Vp>
                                operator*() const
                                { return *_M_current; }

                            constexpr _Vp_iter
                                operator->() const
                                requires __detail::__has_arrow<_Vp_iter>
                                && copyable<_Vp_iter>
                                { return _M_current; }

                            constexpr _Iterator&
                                operator++()
                                {
                                    _M_current = __detail::find_if(std::move(++_M_current),
                                            myranges::end(_M_parent->_M_base),
                                            std::ref(*_M_parent->_M_pred));
                                    return *this;
                                }

                            constexpr void
                                operator++(int)
                                { ++*this; }

                            constexpr _Iterator
                                operator++(int) requires forward_range<_Vp>
                                {
                                    auto __tmp = *this;
                                    ++*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator--() requires bidirectional_range<_Vp>
                                {
                                    do
                                        --_M_current;
                                    while (!std::__invoke(*_M_parent->_M_pred, *_M_current));
                                    return *this;
                                }

                            constexpr _Iterator
                                operator--(int) requires bidirectional_range<_Vp>
                                {
                                    auto __tmp = *this;
                                    --*this;
                                    return __tmp;
                                }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Iterator& __y)
                                requires equality_comparable<_Vp_iter>
                                { return __x._M_current == __y._M_current; }

                            friend constexpr range_rvalue_reference_t<_Vp>
                                iter_move(const _Iterator& __i)
                                noexcept(noexcept(std::ranges::iter_move(__i._M_current)))
                                { return std::ranges::iter_move(__i._M_current); }

                            friend constexpr void
                                iter_swap(const _Iterator& __x, const _Iterator& __y)
                                noexcept(noexcept(std::ranges::iter_swap(__x._M_current, __y._M_current)))
                                requires indirectly_swappable<_Vp_iter>
                                { std::ranges::iter_swap(__x._M_current, __y._M_current); }
                    };

                    struct _Sentinel
                    {
                        private:
                            sentinel_t<_Vp> _M_end = sentinel_t<_Vp>();

                            constexpr bool
                                __equal(const _Iterator& __i) const
                                { return __i._M_current == _M_end; }

                        public:
                            _Sentinel() = default;

                            constexpr explicit
                                _Sentinel(filter_view& __parent)
                                : _M_end(myranges::end(__parent._M_base))
                                { }

                            constexpr sentinel_t<_Vp>
                                base() const
                                { return _M_end; }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Sentinel& __y)
                                { return __y.__equal(__x); }
                    };

                    _Vp _M_base = _Vp();
                    __detail::__box<_Pred> _M_pred;
                    [[no_unique_address]] __detail::_CachedPosition<_Vp> _M_cached_begin;

                public:
                    filter_view() = default;

                    constexpr
                        filter_view(_Vp __base, _Pred __pred)
                        : _M_base(std::move(__base)), _M_pred(std::move(__pred))
                        { }

                    constexpr _Vp
                        base() const& requires copy_constructible<_Vp>
                        { return _M_base; }

                    constexpr _Vp
                        base() &&
                        { return std::move(_M_base); }

                    constexpr const _Pred&
                        pred() const
                        { return *_M_pred; }

                    constexpr _Iterator
                        begin()
                        {
                            if (_M_cached_begin._M_has_value())
                                return {*this, _M_cached_begin._M_get(_M_base)};

                            __glibcxx_assert(_M_pred.has_value());
                            auto __it = __detail::find_if(myranges::begin(_M_base),
                                    myranges::end(_M_base),
                                    std::ref(*_M_pred));
                            _M_cached_begin._M_set(_M_base, __it);
                            return {*this, std::move(__it)};
                        }

                    constexpr auto
                        end()
                        {
                            if constexpr (common_range<_Vp>)
                                return _Iterator{*this, myranges::end(_M_base)};
                            else
                                return _Sentinel{*this};
                        }
            };

    template<typename _Range, typename _Pred>
        filter_view(_Range&&, _Pred) -> filter_view<views::all_t<_Range>, _Pred>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor filter
            = [] <viewable_range _Range, typename _Pred> (_Range&& __r, _Pred&& __p)
            {
                return filter_view{std::forward<_Range>(__r), std::forward<_Pred>(__p)};
            };
    } // namespace views

    template<input_range _Vp, copy_constructible _Fp>
        requires view<_Vp> && is_object_v<_Fp>
        && regular_invocable<_Fp&, range_reference_t<_Vp>>
        && std::__detail::__can_reference<invoke_result_t<_Fp&,
        range_reference_t<_Vp>>>
            class transform_view : public view_interface<transform_view<_Vp, _Fp>>
        {
            private:
                template<bool _Const>
                    struct _Sentinel;

                template<bool _Const>
                    struct _Iterator
                    {
                        private:
                            using _Parent = __detail::__maybe_const_t<_Const, transform_view>;
                            using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                            static constexpr auto
                                _S_iter_concept()
                                {
                                    if constexpr (random_access_range<_Vp>)
                                        return random_access_iterator_tag{};
                                    else if constexpr (bidirectional_range<_Vp>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (forward_range<_Vp>)
                                        return forward_iterator_tag{};
                                    else
                                        return input_iterator_tag{};
                                }

                            static constexpr auto
                                _S_iter_cat()
                                {
                                    using _Res = invoke_result_t<_Fp&, range_reference_t<_Base>>;
                                    if constexpr (is_lvalue_reference_v<_Res>)
                                    {
                                        using _Cat
                                            = typename iterator_traits<_Base_iter>::iterator_category;
                                        if constexpr (derived_from<_Cat, contiguous_iterator_tag>)
                                            return random_access_iterator_tag{};
                                        else
                                            return _Cat{};
                                    }
                                    else
                                        return input_iterator_tag{};
                                }

                            using _Base_iter = iterator_t<_Base>;

                            _Base_iter _M_current = _Base_iter();
                            _Parent* _M_parent = nullptr;

                        public:
                            using iterator_concept = decltype(_S_iter_concept());
                            using iterator_category = decltype(_S_iter_cat());
                            using value_type
                                = remove_cvref_t<invoke_result_t<_Fp&, range_reference_t<_Base>>>;
                            using difference_type = range_difference_t<_Base>;

                            _Iterator() = default;

                            constexpr
                                _Iterator(_Parent& __parent, _Base_iter __current)
                                : _M_current(std::move(__current)),
                                _M_parent(std::__addressof(__parent))
                        { }

                            constexpr
                                _Iterator(_Iterator<!_Const> __i)
                                requires _Const
                                && convertible_to<iterator_t<_Vp>, _Base_iter>
                                : _M_current(std::move(__i._M_current)), _M_parent(__i._M_parent)
                                { }

                            constexpr _Base_iter
                                base() const &
                                requires copyable<_Base_iter>
                                { return _M_current; }

                            constexpr _Base_iter
                                base() &&
                                { return std::move(_M_current); }

                            constexpr decltype(auto)
                                operator*() const
                                noexcept(noexcept(std::__invoke(*_M_parent->_M_fun, *_M_current)))
                                { return std::__invoke(*_M_parent->_M_fun, *_M_current); }

                            constexpr _Iterator&
                                operator++()
                                {
                                    ++_M_current;
                                    return *this;
                                }

                            constexpr void
                                operator++(int)
                                { ++_M_current; }

                            constexpr _Iterator
                                operator++(int) requires forward_range<_Base>
                                {
                                    auto __tmp = *this;
                                    ++*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator--() requires bidirectional_range<_Base>
                                {
                                    --_M_current;
                                    return *this;
                                }

                            constexpr _Iterator
                                operator--(int) requires bidirectional_range<_Base>
                                {
                                    auto __tmp = *this;
                                    --*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator+=(difference_type __n) requires random_access_range<_Base>
                                {
                                    _M_current += __n;
                                    return *this;
                                }

                            constexpr _Iterator&
                                operator-=(difference_type __n) requires random_access_range<_Base>
                                {
                                    _M_current -= __n;
                                    return *this;
                                }

                            constexpr decltype(auto)
                                operator[](difference_type __n) const
                                requires random_access_range<_Base>
                                { return std::__invoke(*_M_parent->_M_fun, _M_current[__n]); }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Iterator& __y)
                                requires equality_comparable<_Base_iter>
                                { return __x._M_current == __y._M_current; }

                            friend constexpr bool
                                operator<(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                { return __x._M_current < __y._M_current; }

                            friend constexpr bool
                                operator>(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                { return __y < __x; }

                            friend constexpr bool
                                operator<=(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                { return !(__y < __x); }

                            friend constexpr bool
                                operator>=(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                { return !(__x < __y); }

#ifdef __cpp_lib_three_way_comparison
                            friend constexpr auto
                                operator<=>(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                && three_way_comparable<_Base_iter>
                                { return __x._M_current <=> __y._M_current; }
#endif

                            friend constexpr _Iterator
                                operator+(_Iterator __i, difference_type __n)
                                requires random_access_range<_Base>
                                { return {*__i._M_parent, __i._M_current + __n}; }

                            friend constexpr _Iterator
                                operator+(difference_type __n, _Iterator __i)
                                requires random_access_range<_Base>
                                { return {*__i._M_parent, __i._M_current + __n}; }

                            friend constexpr _Iterator
                                operator-(_Iterator __i, difference_type __n)
                                requires random_access_range<_Base>
                                { return {*__i._M_parent, __i._M_current - __n}; }

                            friend constexpr difference_type
                                operator-(const _Iterator& __x, const _Iterator& __y)
                                requires random_access_range<_Base>
                                { return __x._M_current - __y._M_current; }

                            friend constexpr decltype(auto)
                                iter_move(const _Iterator& __i) noexcept(noexcept(*__i))
                                {
                                    if constexpr (is_lvalue_reference_v<decltype(*__i)>)
                                        return std::move(*__i);
                                    else
                                        return *__i;
                                }

                            friend constexpr void
                                iter_swap(const _Iterator& __x, const _Iterator& __y)
                                noexcept(noexcept(std::ranges::iter_swap(__x._M_current, __y._M_current)))
                                requires indirectly_swappable<_Base_iter>
                                { return std::ranges::iter_swap(__x._M_current, __y._M_current); }

                            friend _Iterator<!_Const>;
                            template<bool> friend struct _Sentinel;
                    };

                template<bool _Const>
                    struct _Sentinel
                    {
                        private:
                            using _Parent = __detail::__maybe_const_t<_Const, transform_view>;
                            using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                            template<bool _Const2>
                                constexpr range_difference_t<_Base>
                                __distance_from(const _Iterator<_Const2>& __i) const
                                { return _M_end - __i._M_current; }

                            template<bool _Const2>
                                constexpr bool
                                __equal(const _Iterator<_Const2>& __i) const
                                { return __i._M_current == _M_end; }

                            sentinel_t<_Base> _M_end = sentinel_t<_Base>();

                        public:
                            _Sentinel() = default;

                            constexpr explicit
                                _Sentinel(sentinel_t<_Base> __end)
                                : _M_end(__end)
                                { }

                            constexpr
                                _Sentinel(_Sentinel<!_Const> __i)
                                requires _Const
                                && convertible_to<sentinel_t<_Vp>, sentinel_t<_Base>>
                                : _M_end(std::move(__i._M_end))
                                { }

                            constexpr sentinel_t<_Base>
                                base() const
                                { return _M_end; }

                            template<bool _Const2>
                                requires sentinel_for<sentinel_t<_Base>,
                                         iterator_t<__detail::__maybe_const_t<_Const2, _Vp>>>
                                             friend constexpr bool
                                             operator==(const _Iterator<_Const2>& __x, const _Sentinel& __y)
                                             { return __y.__equal(__x); }

                            template<bool _Const2>
                                requires sized_sentinel_for<sentinel_t<_Base>,
                                         iterator_t<__detail::__maybe_const_t<_Const2, _Vp>>>
                                             friend constexpr range_difference_t<_Base>
                                             operator-(const _Iterator<_Const2>& __x, const _Sentinel& __y)
                                             { return -__y.__distance_from(__x); }

                            template<bool _Const2>
                                requires sized_sentinel_for<sentinel_t<_Base>,
                                         iterator_t<__detail::__maybe_const_t<_Const2, _Vp>>>
                                             friend constexpr range_difference_t<_Base>
                                             operator-(const _Sentinel& __y, const _Iterator<_Const2>& __x)
                                             { return __y.__distance_from(__x); }

                            friend _Sentinel<!_Const>;
                    };

                _Vp _M_base = _Vp();
                __detail::__box<_Fp> _M_fun;

            public:
                transform_view() = default;

                constexpr
                    transform_view(_Vp __base, _Fp __fun)
                    : _M_base(std::move(__base)), _M_fun(std::move(__fun))
                    { }

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base ; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr _Iterator<false>
                    begin()
                    { return _Iterator<false>{*this, myranges::begin(_M_base)}; }

                constexpr _Iterator<true>
                    begin() const
                    requires range<const _Vp>
                    && regular_invocable<const _Fp&, range_reference_t<const _Vp>>
                    { return _Iterator<true>{*this, myranges::begin(_M_base)}; }

                constexpr _Sentinel<false>
                    end()
                    { return _Sentinel<false>{myranges::end(_M_base)}; }

                constexpr _Iterator<false>
                    end() requires common_range<_Vp>
                    { return _Iterator<false>{*this, myranges::end(_M_base)}; }

                constexpr _Sentinel<true>
                    end() const
                    requires range<const _Vp>
                    && regular_invocable<const _Fp&, range_reference_t<const _Vp>>
                    { return _Sentinel<true>{myranges::end(_M_base)}; }

                constexpr _Iterator<true>
                    end() const
                    requires common_range<const _Vp>
                    && regular_invocable<const _Fp&, range_reference_t<const _Vp>>
                    { return _Iterator<true>{*this, myranges::end(_M_base)}; }

                constexpr auto
                    size() requires sized_range<_Vp>
                    { return myranges::size(_M_base); }

                constexpr auto
                    size() const requires sized_range<const _Vp>
                    { return myranges::size(_M_base); }
        };

    template<typename _Range, typename _Fp>
        transform_view(_Range&&, _Fp) -> transform_view<views::all_t<_Range>, _Fp>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor transform
            = [] <viewable_range _Range, typename _Fp> (_Range&& __r, _Fp&& __f)
            {
                return transform_view{std::forward<_Range>(__r), std::forward<_Fp>(__f)};
            };
    } // namespace views

    template<view _Vp>
        class take_view : public view_interface<take_view<_Vp>>
    {
        private:
            template<bool _Const>
                struct _Sentinel
                {
                    private:
                        using _Base = __detail::__maybe_const_t<_Const, _Vp>;
                        using _CI = counted_iterator<iterator_t<_Base>>;

                        sentinel_t<_Base> _M_end = sentinel_t<_Base>();

                    public:
                        _Sentinel() = default;

                        constexpr explicit
                            _Sentinel(sentinel_t<_Base> __end)
                            : _M_end(__end)
                            { }

                        constexpr
                            _Sentinel(_Sentinel<!_Const> __s)
                            requires _Const && convertible_to<sentinel_t<_Vp>, sentinel_t<_Base>>
                            : _M_end(std::move(__s._M_end))
                            { }

                        constexpr sentinel_t<_Base>
                            base() const
                            { return _M_end; }

                        friend constexpr bool operator==(const _CI& __y, const _Sentinel& __x)
                        { return __y.count() == 0 || __y.base() == __x._M_end; }

                        friend _Sentinel<!_Const>;
                };

            _Vp _M_base = _Vp();
            range_difference_t<_Vp> _M_count = 0;

        public:
            take_view() = default;

            constexpr
                take_view(_Vp base, range_difference_t<_Vp> __count)
                : _M_base(std::move(base)), _M_count(std::move(__count))
                { }

            constexpr _Vp
                base() const& requires copy_constructible<_Vp>
                { return _M_base; }

            constexpr _Vp
                base() &&
                { return std::move(_M_base); }

            constexpr auto
                begin() requires (!__detail::__simple_view<_Vp>)
                {
                    if constexpr (sized_range<_Vp>)
                    {
                        if constexpr (random_access_range<_Vp>)
                            return myranges::begin(_M_base);
                        else
                        {
                            auto __sz = size();
                            return counted_iterator{myranges::begin(_M_base), __sz};
                        }
                    }
                    else
                        return counted_iterator{myranges::begin(_M_base), _M_count};
                }

            constexpr auto
                begin() const requires range<const _Vp>
                {
                    if constexpr (sized_range<const _Vp>)
                    {
                        if constexpr (random_access_range<const _Vp>)
                            return myranges::begin(_M_base);
                        else
                        {
                            auto __sz = size();
                            return counted_iterator{myranges::begin(_M_base), __sz};
                        }
                    }
                    else
                        return counted_iterator{myranges::begin(_M_base), _M_count};
                }

            constexpr auto
                end() requires (!__detail::__simple_view<_Vp>)
                {
                    if constexpr (sized_range<_Vp>)
                    {
                        if constexpr (random_access_range<_Vp>)
                            return myranges::begin(_M_base) + size();
                        else
                            return default_sentinel;
                    }
                    else
                        return _Sentinel<false>{myranges::end(_M_base)};
                }

            constexpr auto
                end() const requires range<const _Vp>
                {
                    if constexpr (sized_range<const _Vp>)
                    {
                        if constexpr (random_access_range<const _Vp>)
                            return myranges::begin(_M_base) + size();
                        else
                            return default_sentinel;
                    }
                    else
                        return _Sentinel<true>{myranges::end(_M_base)};
                }

            constexpr auto
                size() requires sized_range<_Vp>
                {
                    auto __n = myranges::size(_M_base);
                    return __detail::min(__n, static_cast<decltype(__n)>(_M_count));
                }

            constexpr auto
                size() const requires sized_range<const _Vp>
                {
                    auto __n = myranges::size(_M_base);
                    return __detail::min(__n, static_cast<decltype(__n)>(_M_count));
                }
    };

    template<range _Range>
        take_view(_Range&&, range_difference_t<_Range>)
        -> take_view<views::all_t<_Range>>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor take
            = [] <viewable_range _Range, typename _Tp> (_Range&& __r, _Tp&& __n)
            {
                return take_view{std::forward<_Range>(__r), std::forward<_Tp>(__n)};
            };
    } // namespace views

    template<view _Vp, typename _Pred>
        requires input_range<_Vp> && is_object_v<_Pred>
        && indirect_unary_predicate<const _Pred, iterator_t<_Vp>>
        class take_while_view : public view_interface<take_while_view<_Vp, _Pred>>
        {
            template<bool _Const>
                struct _Sentinel
                {
                    private:
                        using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                        sentinel_t<_Base> _M_end = sentinel_t<_Base>();
                        const _Pred* _M_pred = nullptr;

                    public:
                        _Sentinel() = default;

                        constexpr explicit
                            _Sentinel(sentinel_t<_Base> __end, const _Pred* __pred)
                            : _M_end(__end), _M_pred(__pred)
                            { }

                        constexpr
                            _Sentinel(_Sentinel<!_Const> __s)
                            requires _Const && convertible_to<sentinel_t<_Vp>, sentinel_t<_Base>>
                            : _M_end(__s._M_end), _M_pred(__s._M_pred)
                            { }

                        constexpr sentinel_t<_Base>
                            base() const { return _M_end; }

                        friend constexpr bool
                            operator==(const iterator_t<_Base>& __x, const _Sentinel& __y)
                            { return __y._M_end == __x || !std::__invoke(*__y._M_pred, *__x); }

                        friend _Sentinel<!_Const>;
                };

            _Vp _M_base = _Vp();
            __detail::__box<_Pred> _M_pred;

            public:
            take_while_view() = default;

            constexpr
                take_while_view(_Vp base, _Pred __pred)
                : _M_base(std::move(base)), _M_pred(std::move(__pred))
                {
                }

            constexpr _Vp
                base() const& requires copy_constructible<_Vp>
                { return _M_base; }

            constexpr _Vp
                base() &&
                { return std::move(_M_base); }

            constexpr const _Pred&
                pred() const
                { return *_M_pred; }

            constexpr auto
                begin() requires (!__detail::__simple_view<_Vp>)
                { return myranges::begin(_M_base); }

            constexpr auto
                begin() const requires range<const _Vp>
                { return myranges::begin(_M_base); }

            constexpr auto
                end() requires (!__detail::__simple_view<_Vp>)
                { return _Sentinel<false>(myranges::end(_M_base),
                        std::__addressof(*_M_pred)); }

            constexpr auto
                end() const requires range<const _Vp>
                { return _Sentinel<true>(myranges::end(_M_base),
                        std::__addressof(*_M_pred)); }
        };

    template<typename _Range, typename _Pred>
        take_while_view(_Range&&, _Pred)
        -> take_while_view<views::all_t<_Range>, _Pred>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor take_while
            = [] <viewable_range _Range, typename _Pred> (_Range&& __r, _Pred&& __p)
            {
                return take_while_view{std::forward<_Range>(__r), std::forward<_Pred>(__p)};
            };
    } // namespace views

    template<view _Vp>
        class drop_view : public view_interface<drop_view<_Vp>>
    {
        private:
            _Vp _M_base = _Vp();
            range_difference_t<_Vp> _M_count = 0;

            static constexpr bool _S_needs_cached_begin = !random_access_range<_Vp>;
            [[no_unique_address]]
                __detail::__maybe_present_t<_S_needs_cached_begin,
                __detail::_CachedPosition<_Vp>>
                    _M_cached_begin;

        public:
            drop_view() = default;

            constexpr
                drop_view(_Vp __base, range_difference_t<_Vp> __count)
                : _M_base(std::move(__base)), _M_count(__count)
                { __glibcxx_assert(__count >= 0); }

            constexpr _Vp
                base() const& requires copy_constructible<_Vp>
                { return _M_base; }

            constexpr _Vp
                base() &&
                { return std::move(_M_base); }

            constexpr auto
                begin() requires (!(__detail::__simple_view<_Vp>
                            && random_access_range<_Vp>))
                {
                    if constexpr (_S_needs_cached_begin)
                        if (_M_cached_begin._M_has_value())
                            return _M_cached_begin._M_get(_M_base);

                    auto __it = myranges::next(myranges::begin(_M_base),
                            _M_count, myranges::end(_M_base));
                    if constexpr (_S_needs_cached_begin)
                        _M_cached_begin._M_set(_M_base, __it);
                    return __it;
                }

            constexpr auto
                begin() const requires random_access_range<const _Vp>
                {
                    return myranges::next(myranges::begin(_M_base), _M_count,
                            myranges::end(_M_base));
                }

            constexpr auto
                end() requires (!__detail::__simple_view<_Vp>)
                { return myranges::end(_M_base); }

            constexpr auto
                end() const requires range<const _Vp>
                { return myranges::end(_M_base); }

            constexpr auto
                size() requires sized_range<_Vp>
                {
                    const auto __s = myranges::size(_M_base);
                    const auto __c = static_cast<decltype(__s)>(_M_count);
                    return __s < __c ? 0 : __s - __c;
                }

            constexpr auto
                size() const requires sized_range<const _Vp>
                {
                    const auto __s = myranges::size(_M_base);
                    const auto __c = static_cast<decltype(__s)>(_M_count);
                    return __s < __c ? 0 : __s - __c;
                }
    };

    template<typename _Range>
        drop_view(_Range&&, range_difference_t<_Range>)
        -> drop_view<views::all_t<_Range>>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor drop
            = [] <viewable_range _Range, typename _Tp> (_Range&& __r, _Tp&& __n)
            {
                return drop_view{std::forward<_Range>(__r), std::forward<_Tp>(__n)};
            };
    } // namespace views

    template<view _Vp, typename _Pred>
        requires input_range<_Vp> && is_object_v<_Pred>
        && indirect_unary_predicate<const _Pred, iterator_t<_Vp>>
        class drop_while_view : public view_interface<drop_while_view<_Vp, _Pred>>
        {
            private:
                _Vp _M_base = _Vp();
                __detail::__box<_Pred> _M_pred;
                [[no_unique_address]] __detail::_CachedPosition<_Vp> _M_cached_begin;

            public:
                drop_while_view() = default;

                constexpr
                    drop_while_view(_Vp __base, _Pred __pred)
                    : _M_base(std::move(__base)), _M_pred(std::move(__pred))
                    { }

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr const _Pred&
                    pred() const
                    { return *_M_pred; }

                constexpr auto
                    begin()
                    {
                        if (_M_cached_begin._M_has_value())
                            return _M_cached_begin._M_get(_M_base);

                        auto __it = __detail::find_if_not(myranges::begin(_M_base),
                                myranges::end(_M_base),
                                std::cref(*_M_pred));
                        _M_cached_begin._M_set(_M_base, __it);
                        return __it;
                    }

                constexpr auto
                    end()
                    { return myranges::end(_M_base); }
        };

    template<typename _Range, typename _Pred>
        drop_while_view(_Range&&, _Pred)
        -> drop_while_view<views::all_t<_Range>, _Pred>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor drop_while
            = [] <viewable_range _Range, typename _Pred> (_Range&& __r, _Pred&& __p)
            {
                return drop_while_view{std::forward<_Range>(__r),
                    std::forward<_Pred>(__p)};
            };
    } // namespace views

    template<input_range _Vp>
        requires view<_Vp> && input_range<range_reference_t<_Vp>>
        && (is_reference_v<range_reference_t<_Vp>>
                || view<range_value_t<_Vp>>)
        class join_view : public view_interface<join_view<_Vp>>
        {
            private:
                using _InnerRange = range_reference_t<_Vp>;

                template<bool _Const>
                    struct _Sentinel;

                template<bool _Const>
                    struct _Iterator
                    {
                        private:
                            using _Parent = __detail::__maybe_const_t<_Const, join_view>;
                            using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                            static constexpr bool _S_ref_is_glvalue
                                = is_reference_v<range_reference_t<_Base>>;

                            constexpr void
                                _M_satisfy()
                                {
                                    auto __update_inner = [this] (range_reference_t<_Base> __x) -> auto&
                                    {
                                        if constexpr (_S_ref_is_glvalue)
                                            return __x;
                                        else
                                            return (_M_parent->_M_inner = views::all(std::move(__x)));
                                    };

                                    for (; _M_outer != myranges::end(_M_parent->_M_base); ++_M_outer)
                                    {
                                        auto& inner = __update_inner(*_M_outer);
                                        _M_inner = myranges::begin(inner);
                                        if (_M_inner != myranges::end(inner))
                                            return;
                                    }

                                    if constexpr (_S_ref_is_glvalue)
                                        _M_inner = _Inner_iter();
                                }

                            static constexpr auto
                                _S_iter_concept()
                                {
                                    if constexpr (_S_ref_is_glvalue
                                            && bidirectional_range<_Base>
                                            && bidirectional_range<range_reference_t<_Base>>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (_S_ref_is_glvalue
                                            && forward_range<_Base>
                                            && forward_range<range_reference_t<_Base>>)
                                        return forward_iterator_tag{};
                                    else
                                        return input_iterator_tag{};
                                }

                            static constexpr auto
                                _S_iter_cat()
                                {
                                    using _OuterCat
                                        = typename iterator_traits<_Outer_iter>::iterator_category;
                                    using _InnerCat
                                        = typename iterator_traits<_Inner_iter>::iterator_category;
                                    if constexpr (_S_ref_is_glvalue
                                            && derived_from<_OuterCat, bidirectional_iterator_tag>
                                            && derived_from<_InnerCat, bidirectional_iterator_tag>)
                                        return bidirectional_iterator_tag{};
                                    else if constexpr (_S_ref_is_glvalue
                                            && derived_from<_OuterCat, forward_iterator_tag>
                                            && derived_from<_InnerCat, forward_iterator_tag>)
                                        return forward_iterator_tag{};
                                    else if constexpr (derived_from<_OuterCat, input_iterator_tag>
                                            && derived_from<_InnerCat, input_iterator_tag>)
                                        return input_iterator_tag{};
                                    else
                                        return output_iterator_tag{};
                                }

                            using _Outer_iter = iterator_t<_Base>;
                            using _Inner_iter = iterator_t<range_reference_t<_Base>>;

                            _Outer_iter _M_outer = _Outer_iter();
                            _Inner_iter _M_inner = _Inner_iter();
                            _Parent* _M_parent = nullptr;

                        public:
                            using iterator_concept = decltype(_S_iter_concept());
                            using iterator_category = decltype(_S_iter_cat());
                            using value_type = range_value_t<range_reference_t<_Base>>;
                            using difference_type
                                = common_type_t<range_difference_t<_Base>,
                                range_difference_t<range_reference_t<_Base>>>;

                            _Iterator() = default;

                            constexpr
                                _Iterator(_Parent& __parent, _Outer_iter __outer)
                                : _M_outer(std::move(__outer)),
                                _M_parent(std::__addressof(__parent))
                        { _M_satisfy(); }

                            constexpr
                                _Iterator(_Iterator<!_Const> __i)
                                requires _Const
                                && convertible_to<iterator_t<_Vp>, _Outer_iter>
                                && convertible_to<iterator_t<_InnerRange>, _Inner_iter>
                                : _M_outer(std::move(__i._M_outer)), _M_inner(__i._M_inner),
                                _M_parent(__i._M_parent)
                                { }

                            constexpr decltype(auto)
                                operator*() const
                                { return *_M_inner; }

                            constexpr _Outer_iter
                                operator->() const
                                requires __detail::__has_arrow<_Outer_iter>
                                && copyable<_Outer_iter>
                                { return _M_inner; }

                            constexpr _Iterator&
                                operator++()
                                {
                                    auto&& __inner_range = [this] () -> decltype(auto) {
                                        if constexpr (_S_ref_is_glvalue)
                                            return *_M_outer;
                                        else
                                            return _M_parent->_M_inner;
                                    }();
                                    if (++_M_inner == myranges::end(__inner_range))
                                    {
                                        ++_M_outer;
                                        _M_satisfy();
                                    }
                                    return *this;
                                }

                            constexpr void
                                operator++(int)
                                { ++*this; }

                            constexpr _Iterator
                                operator++(int)
                                requires _S_ref_is_glvalue && forward_range<_Base>
                                && forward_range<range_reference_t<_Base>>
                                {
                                    auto __tmp = *this;
                                    ++*this;
                                    return __tmp;
                                }

                            constexpr _Iterator&
                                operator--()
                                requires _S_ref_is_glvalue && bidirectional_range<_Base>
                                && bidirectional_range<range_reference_t<_Base>>
                                && common_range<range_reference_t<_Base>>
                                {
                                    if (_M_outer == myranges::end(_M_parent->_M_base))
                                        _M_inner = myranges::end(*--_M_outer);
                                    while (_M_inner == myranges::begin(*_M_outer))
                                        _M_inner = myranges::end(*--_M_outer);
                                    --_M_inner;
                                    return *this;
                                }

                            constexpr _Iterator
                                operator--(int)
                                requires _S_ref_is_glvalue && bidirectional_range<_Base>
                                && bidirectional_range<range_reference_t<_Base>>
                                && common_range<range_reference_t<_Base>>
                                {
                                    auto __tmp = *this;
                                    --*this;
                                    return __tmp;
                                }

                            friend constexpr bool
                                operator==(const _Iterator& __x, const _Iterator& __y)
                                requires _S_ref_is_glvalue
                                && equality_comparable<_Outer_iter>
                                && equality_comparable<_Inner_iter>
                                {
                                    return (__x._M_outer == __y._M_outer
                                            && __x._M_inner == __y._M_inner);
                                }

                            friend constexpr decltype(auto)
                                iter_move(const _Iterator& __i)
                                noexcept(noexcept(std::ranges::iter_move(__i._M_inner)))
                                { return std::ranges::iter_move(__i._M_inner); }

                            friend constexpr void
                                iter_swap(const _Iterator& __x, const _Iterator& __y)
                                noexcept(noexcept(std::ranges::iter_swap(__x._M_inner, __y._M_inner)))
                                { return std::ranges::iter_swap(__x._M_inner, __y._M_inner); }

                            friend _Iterator<!_Const>;
                            template<bool> friend struct _Sentinel;
                    };

                template<bool _Const>
                    struct _Sentinel
                    {
                        private:
                            using _Parent = __detail::__maybe_const_t<_Const, join_view>;
                            using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                            template<bool _Const2>
                                constexpr bool
                                __equal(const _Iterator<_Const2>& __i) const
                                { return __i._M_outer == _M_end; }

                            sentinel_t<_Base> _M_end = sentinel_t<_Base>();

                        public:
                            _Sentinel() = default;

                            constexpr explicit
                                _Sentinel(_Parent& __parent)
                                : _M_end(myranges::end(__parent._M_base))
                                { }

                            constexpr
                                _Sentinel(_Sentinel<!_Const> __s)
                                requires _Const && convertible_to<sentinel_t<_Vp>, sentinel_t<_Base>>
                                : _M_end(std::move(__s._M_end))
                                { }

                            template<bool _Const2>
                                requires sentinel_for<sentinel_t<_Base>,
                                         iterator_t<__detail::__maybe_const_t<_Const2, _Vp>>>
                                             friend constexpr bool
                                             operator==(const _Iterator<_Const2>& __x, const _Sentinel& __y)
                                             { return __y.__equal(__x); }

                            friend _Sentinel<!_Const>;
                    };

                _Vp _M_base = _Vp();

                // XXX: _M_inner is "present only when !is_reference_v<_InnerRange>"
                [[no_unique_address]]
                    __detail::__maybe_present_t<!is_reference_v<_InnerRange>,
                    views::all_t<_InnerRange>> _M_inner;

            public:
                join_view() = default;

                constexpr explicit
                    join_view(_Vp __base)
                    : _M_base(std::move(__base))
                    { }

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr auto
                    begin()
                    {
                        constexpr bool __use_const
                            = (__detail::__simple_view<_Vp>
                                    && is_reference_v<range_reference_t<_Vp>>);
                        return _Iterator<__use_const>{*this, myranges::begin(_M_base)};
                    }

                constexpr auto
                    begin() const
                    requires input_range<const _Vp>
                    && is_reference_v<range_reference_t<const _Vp>>
                    {
                        return _Iterator<true>{*this, myranges::begin(_M_base)};
                    }

                constexpr auto
                    end()
                    {
                        if constexpr (forward_range<_Vp> && is_reference_v<_InnerRange>
                                && forward_range<_InnerRange>
                                && common_range<_Vp> && common_range<_InnerRange>)
                            return _Iterator<__detail::__simple_view<_Vp>>{*this,
                                myranges::end(_M_base)};
                        else
                            return _Sentinel<__detail::__simple_view<_Vp>>{*this};
                    }

                constexpr auto
                    end() const
                    requires input_range<const _Vp>
                    && is_reference_v<range_reference_t<const _Vp>>
                    {
                        if constexpr (forward_range<const _Vp>
                                && is_reference_v<range_reference_t<const _Vp>>
                                && forward_range<range_reference_t<const _Vp>>
                                && common_range<const _Vp>
                                && common_range<range_reference_t<const _Vp>>)
                            return _Iterator<true>{*this, myranges::end(_M_base)};
                        else
                            return _Sentinel<true>{*this};
                    }
        };

    template<typename _Range>
        explicit join_view(_Range&&) -> join_view<views::all_t<_Range>>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptorClosure join
            = [] <viewable_range _Range> (_Range&& __r)
            {
                return join_view{std::forward<_Range>(__r)};
            };
    } // namespace views

    namespace __detail
    {
        template<auto>
            struct __require_constant;

        template<typename _Range>
            concept __tiny_range = sized_range<_Range>
            && requires
            { typename __require_constant<remove_reference_t<_Range>::size()>; }
        && (remove_reference_t<_Range>::size() <= 1);
    }

    template<input_range _Vp, forward_range _Pattern>
        requires view<_Vp> && view<_Pattern>
        && indirectly_comparable<iterator_t<_Vp>, iterator_t<_Pattern>,
        std::ranges::equal_to>
            && (forward_range<_Vp> || __detail::__tiny_range<_Pattern>)
            class split_view : public view_interface<split_view<_Vp, _Pattern>>
            {
                private:
                    template<bool _Const>
                        struct _InnerIter;

                    template<bool _Const>
                        struct _OuterIter
                        {
                            private:
                                using _Parent = __detail::__maybe_const_t<_Const, split_view>;
                                using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                                constexpr bool
                                    __at_end() const
                                    { return __current() == myranges::end(_M_parent->_M_base); }

                                // [range.split.outer] p1
                                //  Many of the following specifications refer to the notional member
                                //  current of outer-iterator.  current is equivalent to current_ if
                                //  V models forward_range, and parent_->current_ otherwise.
                                constexpr auto&
                                    __current() noexcept
                                    {
                                        if constexpr (forward_range<_Vp>)
                                            return _M_current;
                                        else
                                            return _M_parent->_M_current;
                                    }

                                constexpr auto&
                                    __current() const noexcept
                                    {
                                        if constexpr (forward_range<_Vp>)
                                            return _M_current;
                                        else
                                            return _M_parent->_M_current;
                                    }

                                _Parent* _M_parent = nullptr;

                                // XXX: _M_current is present only if "V models forward_range"
                                [[no_unique_address]]
                                    __detail::__maybe_present_t<forward_range<_Vp>,
                                    iterator_t<_Base>> _M_current;

                            public:
                                using iterator_concept = conditional_t<forward_range<_Base>,
                                      forward_iterator_tag,
                                      input_iterator_tag>;
                                using iterator_category = input_iterator_tag;
                                using difference_type = range_difference_t<_Base>;

                                struct value_type : view_interface<value_type>
                            {
                                private:
                                    _OuterIter _M_i = _OuterIter();

                                public:
                                    value_type() = default;

                                    constexpr explicit
                                        value_type(_OuterIter __i)
                                        : _M_i(std::move(__i))
                                        { }

                                    constexpr _InnerIter<_Const>
                                        begin() const
                                        requires copyable<_OuterIter>
                                        { return _InnerIter<_Const>{_M_i}; }

                                    constexpr _InnerIter<_Const>
                                        begin()
                                        requires (!copyable<_OuterIter>)
                                        { return _InnerIter<_Const>{std::move(_M_i)}; }

                                    constexpr default_sentinel_t
                                        end() const
                                        { return default_sentinel; }
                            };

                                _OuterIter() = default;

                                constexpr explicit
                                    _OuterIter(_Parent& __parent) requires (!forward_range<_Base>)
                                    : _M_parent(std::__addressof(__parent))
                                    { }

                                constexpr
                                    _OuterIter(_Parent& __parent, iterator_t<_Base> __current)
                                    requires forward_range<_Base>
                                    : _M_parent(std::__addressof(__parent)),
                                    _M_current(std::move(__current))
                                    { }

                                constexpr
                                    _OuterIter(_OuterIter<!_Const> __i)
                                    requires _Const
                                    && convertible_to<iterator_t<_Vp>, iterator_t<_Base>>
                                    : _M_parent(__i._M_parent), _M_current(std::move(__i._M_current))
                                    { }

                                constexpr value_type
                                    operator*() const
                                    { return value_type{*this}; }

                                constexpr _OuterIter&
                                    operator++()
                                    {
                                        const auto __end = myranges::end(_M_parent->_M_base);
                                        if (__current() == __end)
                                            return *this;
                                        const auto [__pbegin, __pend] = subrange{_M_parent->_M_pattern};
                                        if (__pbegin == __pend)
                                            ++__current();
                                        else
                                            do
                                            {
                                                auto [__b, __p]
                                                    = __detail::mismatch(std::move(__current()), __end,
                                                            __pbegin, __pend);
                                                __current() = std::move(__b);
                                                if (__p == __pend)
                                                    break;
                                            } while (++__current() != __end);
                                        return *this;
                                    }

                                constexpr decltype(auto)
                                    operator++(int)
                                    {
                                        if constexpr (forward_range<_Base>)
                                        {
                                            auto __tmp = *this;
                                            ++*this;
                                            return __tmp;
                                        }
                                        else
                                            ++*this;
                                    }

                                friend constexpr bool
                                    operator==(const _OuterIter& __x, const _OuterIter& __y)
                                    requires forward_range<_Base>
                                    { return __x._M_current == __y._M_current; }

                                friend constexpr bool
                                    operator==(const _OuterIter& __x, default_sentinel_t)
                                    { return __x.__at_end(); };

                                friend _OuterIter<!_Const>;
                                friend _InnerIter<_Const>;
                        };

                    template<bool _Const>
                        struct _InnerIter
                        {
                            private:
                                using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                                constexpr bool
                                    __at_end() const
                                    {
                                        auto [__pcur, __pend] = subrange{_M_i._M_parent->_M_pattern};
                                        auto __end = myranges::end(_M_i._M_parent->_M_base);
                                        if constexpr (__detail::__tiny_range<_Pattern>)
                                        {
                                            const auto& __cur = _M_i_current();
                                            if (__cur == __end)
                                                return true;
                                            if (__pcur == __pend)
                                                return _M_incremented;
                                            return *__cur == *__pcur;
                                        }
                                        else
                                        {
                                            auto __cur = _M_i_current();
                                            if (__cur == __end)
                                                return true;
                                            if (__pcur == __pend)
                                                return _M_incremented;
                                            do
                                            {
                                                if (*__cur != *__pcur)
                                                    return false;
                                                if (++__pcur == __pend)
                                                    return true;
                                            } while (++__cur != __end);
                                            return false;
                                        }
                                    }

                                static constexpr auto
                                    _S_iter_cat()
                                    {
                                        using _Cat
                                            = typename iterator_traits<iterator_t<_Base>>::iterator_category;
                                        if constexpr (derived_from<_Cat, forward_iterator_tag>)
                                            return forward_iterator_tag{};
                                        else
                                            return _Cat{};
                                    }

                                constexpr auto&
                                    _M_i_current() noexcept
                                    { return _M_i.__current(); }

                                constexpr auto&
                                    _M_i_current() const noexcept
                                    { return _M_i.__current(); }

                                _OuterIter<_Const> _M_i = _OuterIter<_Const>();
                                bool _M_incremented = false;

                            public:
                                using iterator_concept
                                    = typename _OuterIter<_Const>::iterator_concept;
                                using iterator_category = decltype(_S_iter_cat());
                                using value_type = range_value_t<_Base>;
                                using difference_type = range_difference_t<_Base>;

                                _InnerIter() = default;

                                constexpr explicit
                                    _InnerIter(_OuterIter<_Const> __i)
                                    : _M_i(std::move(__i))
                                    { }

                                constexpr decltype(auto)
                                    operator*() const
                                    { return *_M_i_current(); }

                                constexpr _InnerIter&
                                    operator++()
                                    {
                                        _M_incremented = true;
                                        if constexpr (!forward_range<_Base>)
                                            if constexpr (_Pattern::size() == 0)
                                                return *this;
                                        ++_M_i_current();
                                        return *this;
                                    }

                                constexpr decltype(auto)
                                    operator++(int)
                                    {
                                        if constexpr (forward_range<_Vp>)
                                        {
                                            auto __tmp = *this;
                                            ++*this;
                                            return __tmp;
                                        }
                                        else
                                            ++*this;
                                    }

                                friend constexpr bool
                                    operator==(const _InnerIter& __x, const _InnerIter& __y)
                                    requires forward_range<_Base>
                                    { return __x._M_i == __y._M_i; }

                                friend constexpr bool
                                    operator==(const _InnerIter& __x, default_sentinel_t)
                                    { return __x.__at_end(); }

                                friend constexpr decltype(auto)
                                    iter_move(const _InnerIter& __i)
                                    noexcept(noexcept(std::ranges::iter_move(__i._M_i_current())))
                                    { return std::ranges::iter_move(__i._M_i_current()); }

                                friend constexpr void
                                    iter_swap(const _InnerIter& __x, const _InnerIter& __y)
                                    noexcept(noexcept(std::ranges::iter_swap(__x._M_i_current(),
                                                    __y._M_i_current())))
                                    requires indirectly_swappable<iterator_t<_Base>>
                                    { std::ranges::iter_swap(__x._M_i_current(), __y._M_i_current()); }
                        };

                    _Vp _M_base = _Vp();
                    _Pattern _M_pattern = _Pattern();

                    // XXX: _M_current is "present only if !forward_range<V>"
                    [[no_unique_address]]
                        __detail::__maybe_present_t<!forward_range<_Vp>, iterator_t<_Vp>>
                            _M_current;


                public:
                    split_view() = default;

                    constexpr
                        split_view(_Vp __base, _Pattern __pattern)
                        : _M_base(std::move(__base)), _M_pattern(std::move(__pattern))
                        { }

                    template<input_range _Range>
                        requires constructible_from<_Vp, views::all_t<_Range>>
                        && constructible_from<_Pattern, single_view<range_value_t<_Range>>>
                        constexpr
                        split_view(_Range&& __r, range_value_t<_Range> __e)
                        : _M_base(views::all(std::forward<_Range>(__r))),
                        _M_pattern(std::move(__e))
                { }

                    constexpr _Vp
                        base() const& requires copy_constructible<_Vp>
                        { return _M_base; }

                    constexpr _Vp
                        base() &&
                        { return std::move(_M_base); }

                    constexpr auto
                        begin()
                        {
                            if constexpr (forward_range<_Vp>)
                                return _OuterIter<__detail::__simple_view<_Vp>>{
                                    *this, myranges::begin(_M_base)};
                            else
                            {
                                _M_current = myranges::begin(_M_base);
                                return _OuterIter<false>{*this};
                            }
                        }

                    constexpr auto
                        begin() const requires forward_range<_Vp> && forward_range<const _Vp>
                        {
                            return _OuterIter<true>{*this, myranges::begin(_M_base)};
                        }

                    constexpr auto
                        end() requires forward_range<_Vp> && common_range<_Vp>
                        {
                            return _OuterIter<__detail::__simple_view<_Vp>>{
                                *this, myranges::end(_M_base)};
                        }

                    constexpr auto
                        end() const
                        {
                            if constexpr (forward_range<_Vp>
                                    && forward_range<const _Vp>
                                    && common_range<const _Vp>)
                                return _OuterIter<true>{*this, myranges::end(_M_base)};
                            else
                                return default_sentinel;
                        }
            };

    template<typename _Range, typename _Pred>
        split_view(_Range&&, _Pred&&)
        -> split_view<views::all_t<_Range>, views::all_t<_Pred>>;

    template<input_range _Range>
        split_view(_Range&&, range_value_t<_Range>)
        -> split_view<views::all_t<_Range>, single_view<range_value_t<_Range>>>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptor split
            = [] <viewable_range _Range, typename _Fp> (_Range&& __r, _Fp&& __f)
            {
                return split_view{std::forward<_Range>(__r), std::forward<_Fp>(__f)};
            };
    } // namespace views

    namespace views
    {
        struct _Counted
        {
            template<input_or_output_iterator _Iter>
                constexpr auto
                operator()(_Iter __i, iter_difference_t<_Iter> __n) const
                {
                    if constexpr (random_access_iterator<_Iter>)
                        return subrange{__i, __i + __n};
                    else
                        return subrange{counted_iterator{std::move(__i), __n},
                            default_sentinel};
                }
        };

        inline constexpr _Counted counted{};
    } // namespace views

    template<view _Vp>
        requires (!common_range<_Vp>) && copyable<iterator_t<_Vp>>
        class common_view : public view_interface<common_view<_Vp>>
        {
            private:
                _Vp _M_base = _Vp();

            public:
                common_view() = default;

                constexpr explicit
                    common_view(_Vp __r)
                    : _M_base(std::move(__r))
                    { }

                /* XXX: LWG 3280 didn't remove this constructor, but I think it should?
                   template<viewable_range _Range>
                   requires (!common_range<_Range>)
                   && constructible_from<_Vp, views::all_t<_Range>>
                   constexpr explicit
                   common_view(_Range&& __r)
                   : _M_base(views::all(std::forward<_Range>(__r)))
                   { }
                   */

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr auto
                    begin()
                    {
                        if constexpr (random_access_range<_Vp> && sized_range<_Vp>)
                            return myranges::begin(_M_base);
                        else
                            return common_iterator<iterator_t<_Vp>, sentinel_t<_Vp>>
                                (myranges::begin(_M_base));
                    }

                constexpr auto
                    begin() const requires range<const _Vp>
                    {
                        if constexpr (random_access_range<const _Vp> && sized_range<const _Vp>)
                            return myranges::begin(_M_base);
                        else
                            return common_iterator<iterator_t<const _Vp>, sentinel_t<const _Vp>>
                                (myranges::begin(_M_base));
                    }

                constexpr auto
                    end()
                    {
                        if constexpr (random_access_range<_Vp> && sized_range<_Vp>)
                            return myranges::begin(_M_base) + myranges::size(_M_base);
                        else
                            return common_iterator<iterator_t<_Vp>, sentinel_t<_Vp>>
                                (myranges::end(_M_base));
                    }

                constexpr auto
                    end() const requires range<const _Vp>
                    {
                        if constexpr (random_access_range<const _Vp> && sized_range<const _Vp>)
                            return myranges::begin(_M_base) + myranges::size(_M_base);
                        else
                            return common_iterator<iterator_t<const _Vp>, sentinel_t<const _Vp>>
                                (myranges::end(_M_base));
                    }

                constexpr auto
                    size() requires sized_range<_Vp>
                    { return myranges::size(_M_base); }

                constexpr auto
                    size() const requires sized_range<const _Vp>
                    { return myranges::size(_M_base); }
        };

    template<typename _Range>
        common_view(_Range&&) -> common_view<views::all_t<_Range>>;

    namespace views
    {
        inline constexpr __adaptor::_RangeAdaptorClosure common
            = [] <viewable_range _Range> (_Range&& __r)
            {
                if constexpr (common_range<_Range>
                        && requires { views::all(std::forward<_Range>(__r)); })
                    return views::all(std::forward<_Range>(__r));
                else
                    return common_view{std::forward<_Range>(__r)};
            };

    } // namespace views

    template<view _Vp>
        requires bidirectional_range<_Vp>
        class reverse_view : public view_interface<reverse_view<_Vp>>
        {
            private:
                _Vp _M_base = _Vp();

                static constexpr bool _S_needs_cached_begin
                    = !common_range<_Vp> && !random_access_range<_Vp>;
                [[no_unique_address]]
                    __detail::__maybe_present_t<_S_needs_cached_begin,
                    __detail::_CachedPosition<_Vp>>
                        _M_cached_begin;

            public:
                reverse_view() = default;

                constexpr explicit
                    reverse_view(_Vp __r)
                    : _M_base(std::move(__r))
                    { }

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr reverse_iterator<iterator_t<_Vp>>
                    begin()
                    {
                        if constexpr (_S_needs_cached_begin)
                            if (_M_cached_begin._M_has_value())
                                return make_reverse_iterator(_M_cached_begin._M_get(_M_base));

                        auto __it = myranges::next(myranges::begin(_M_base), myranges::end(_M_base));
                        if constexpr (_S_needs_cached_begin)
                            _M_cached_begin._M_set(_M_base, __it);
                        return make_reverse_iterator(std::move(__it));
                    }

                constexpr auto
                    begin() requires common_range<_Vp>
                    { return make_reverse_iterator(myranges::end(_M_base)); }

                constexpr auto
                    begin() const requires common_range<const _Vp>
                    { return make_reverse_iterator(myranges::end(_M_base)); }

                constexpr reverse_iterator<iterator_t<_Vp>>
                    end()
                    { return make_reverse_iterator(myranges::begin(_M_base)); }

                constexpr auto
                    end() const requires common_range<const _Vp>
                    { return make_reverse_iterator(myranges::begin(_M_base)); }

                constexpr auto
                    size() requires sized_range<_Vp>
                    { return myranges::size(_M_base); }

                constexpr auto
                    size() const requires sized_range<const _Vp>
                    { return myranges::size(_M_base); }
        };

    template<typename _Range>
        reverse_view(_Range&&) -> reverse_view<views::all_t<_Range>>;

    namespace views
    {
        namespace __detail
        {
            template<typename>
                inline constexpr bool __is_reversible_subrange = false;

            template<typename _Iter, subrange_kind _Kind>
                inline constexpr bool
                __is_reversible_subrange<subrange<reverse_iterator<_Iter>,
                reverse_iterator<_Iter>,
                _Kind>> = true;

            template<typename>
                inline constexpr bool __is_reverse_view = false;

            template<typename _Vp>
                inline constexpr bool __is_reverse_view<reverse_view<_Vp>> = true;
        }

        inline constexpr __adaptor::_RangeAdaptorClosure reverse
            = [] <viewable_range _Range> (_Range&& __r)
            {
                using _Tp = remove_cvref_t<_Range>;
                if constexpr (__detail::__is_reverse_view<_Tp>)
                    return std::forward<_Range>(__r).base();
                else if constexpr (__detail::__is_reversible_subrange<_Tp>)
                {
                    using _Iter = decltype(myranges::begin(__r).base());
                    if constexpr (sized_range<_Tp>)
                        return subrange<_Iter, _Iter, subrange_kind::sized>
                            (__r.end().base(), __r.begin().base(), __r.size());
                    else
                        return subrange<_Iter, _Iter, subrange_kind::unsized>
                            (__r.end().base(), __r.begin().base());
                }
                else
                    return reverse_view{std::forward<_Range>(__r)};
            };
    } // namespace views

    namespace __detail
    {
        template<typename _Tp, size_t _Nm>
            concept __has_tuple_element = requires(_Tp __t)
            {
                typename tuple_size<_Tp>::type;
                requires _Nm < tuple_size_v<_Tp>;
                typename tuple_element_t<_Nm, _Tp>;
                { std::get<_Nm>(__t) }
                -> convertible_to<const tuple_element_t<_Nm, _Tp>&>;
            };
    }

    template<input_range _Vp, size_t _Nm>
        requires view<_Vp>
        && __detail::__has_tuple_element<range_value_t<_Vp>, _Nm>
        && __detail::__has_tuple_element<remove_reference_t<range_reference_t<_Vp>>,
        _Nm>
            class elements_view : public view_interface<elements_view<_Vp, _Nm>>
        {
            public:
                elements_view() = default;

                constexpr explicit
                    elements_view(_Vp base)
                    : _M_base(std::move(base))
                    { }

                constexpr _Vp
                    base() const& requires copy_constructible<_Vp>
                    { return _M_base; }

                constexpr _Vp
                    base() &&
                    { return std::move(_M_base); }

                constexpr auto
                    begin() requires (!__detail::__simple_view<_Vp>)
                    { return _Iterator<false>(myranges::begin(_M_base)); }

                constexpr auto
                    begin() const requires __detail::__simple_view<_Vp>
                    { return _Iterator<true>(myranges::begin(_M_base)); }

                constexpr auto
                    end() requires (!__detail::__simple_view<_Vp>)
                    { return myranges::end(_M_base); }

                constexpr auto
                    end() const requires __detail::__simple_view<_Vp>
                    { return myranges::end(_M_base); }

                constexpr auto
                    size() requires sized_range<_Vp>
                    { return myranges::size(_M_base); }

                constexpr auto
                    size() const requires sized_range<const _Vp>
                    { return myranges::size(_M_base); }

            private:
                template<bool _Const>
                    struct _Iterator
                    {
                        using _Base = __detail::__maybe_const_t<_Const, _Vp>;

                        iterator_t<_Base> _M_current = iterator_t<_Base>();

                        friend _Iterator<!_Const>;

                        public:
                        using iterator_category
                            = typename iterator_traits<iterator_t<_Base>>::iterator_category;
                        using value_type
                            = remove_cvref_t<tuple_element_t<_Nm, range_value_t<_Base>>>;
                        using difference_type = range_difference_t<_Base>;

                        _Iterator() = default;

                        constexpr explicit
                            _Iterator(iterator_t<_Base> current)
                            : _M_current(std::move(current))
                            { }

                        constexpr
                            _Iterator(_Iterator<!_Const> i)
                            requires _Const && convertible_to<iterator_t<_Vp>, iterator_t<_Base>>
                            : _M_current(std::move(i._M_current))
                            { }

                        constexpr iterator_t<_Base>
                            base() const&
                            requires copyable<iterator_t<_Base>>
                            { return _M_current; }

                        constexpr iterator_t<_Base>
                            base() &&
                            { return std::move(_M_current); }

                        constexpr decltype(auto)
                            operator*() const
                            { return std::get<_Nm>(*_M_current); }

                        constexpr _Iterator&
                            operator++()
                            {
                                ++_M_current;
                                return *this;
                            }

                        constexpr void
                            operator++(int) requires (!forward_range<_Base>)
                            { ++_M_current; }

                        constexpr _Iterator
                            operator++(int) requires forward_range<_Base>
                            {
                                auto __tmp = *this;
                                ++_M_current;
                                return __tmp;
                            }

                        constexpr _Iterator&
                            operator--() requires bidirectional_range<_Base>
                            {
                                --_M_current;
                                return *this;
                            }

                        constexpr _Iterator
                            operator--(int) requires bidirectional_range<_Base>
                            {
                                auto __tmp = *this;
                                --_M_current;
                                return __tmp;
                            }

                        constexpr _Iterator&
                            operator+=(difference_type __n)
                            requires random_access_range<_Base>
                            {
                                _M_current += __n;
                                return *this;
                            }

                        constexpr _Iterator&
                            operator-=(difference_type __n)
                            requires random_access_range<_Base>
                            {
                                _M_current -= __n;
                                return *this;
                            }

                        constexpr decltype(auto)
                            operator[](difference_type __n) const
                            requires random_access_range<_Base>
                            { return std::get<_Nm>(*(_M_current + __n)); }

                        friend constexpr bool
                            operator==(const _Iterator& __x, const _Iterator& __y)
                            requires equality_comparable<iterator_t<_Base>>
                            { return __x._M_current == __y._M_current; }

                        friend constexpr bool
                            operator==(const _Iterator& __x, const sentinel_t<_Base>& __y)
                            { return __x._M_current == __y; }

                        friend constexpr bool
                            operator<(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return __x._M_current < __y._M_current; }

                        friend constexpr bool
                            operator>(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return __y._M_current < __x._M_current; }

                        friend constexpr bool
                            operator<=(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return !(__y._M_current > __x._M_current); }

                        friend constexpr bool
                            operator>=(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return !(__x._M_current > __y._M_current); }

#ifdef __cpp_lib_three_way_comparison
                        friend constexpr auto
                            operator<=>(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            && three_way_comparable<iterator_t<_Base>>
                            { return __x._M_current <=> __y._M_current; }
#endif

                        friend constexpr _Iterator
                            operator+(const _Iterator& __x, difference_type __y)
                            requires random_access_range<_Base>
                            { return _Iterator{__x} += __y; }

                        friend constexpr _Iterator
                            operator+(difference_type __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return __y + __x; }

                        friend constexpr _Iterator
                            operator-(const _Iterator& __x, difference_type __y)
                            requires random_access_range<_Base>
                            { return _Iterator{__x} -= __y; }

                        friend constexpr difference_type
                            operator-(const _Iterator& __x, const _Iterator& __y)
                            requires random_access_range<_Base>
                            { return __x._M_current - __y._M_current; }

                        friend constexpr difference_type
                            operator-(const _Iterator<_Const>& __x, const sentinel_t<_Base>& __y)
                            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
                            { return __x._M_current - __y; }

                        friend constexpr difference_type
                            operator-(const sentinel_t<_Base>& __x, const _Iterator<_Const>& __y)
                            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
                            { return -(__y - __x); }
                    };

                _Vp _M_base = _Vp();
        };

    template<typename _Range>
        using keys_view = elements_view<views::all_t<_Range>, 0>;

    template<typename _Range>
        using values_view = elements_view<views::all_t<_Range>, 1>;

    namespace views
    {
        template<size_t _Nm>
            inline constexpr __adaptor::_RangeAdaptorClosure elements
            = [] <viewable_range _Range> (_Range&& __r)
            {
                using _El = elements_view<views::all_t<_Range>, _Nm>;
                return _El{std::forward<_Range>(__r)};
            };

        inline constexpr __adaptor::_RangeAdaptorClosure keys = elements<0>;
        inline constexpr __adaptor::_RangeAdaptorClosure values = elements<1>;
    } // namespace views

}  //namespace ranges
