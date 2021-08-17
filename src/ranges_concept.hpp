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

#pragma once

#include <concepts>
#include <ranges>
#include <limits>
#include <bits/iterator_concepts.h>

namespace myranges {
    //using namespace std;
    using std::same_as;
    using std::integral;
    using std::signed_integral;
    using std::make_unsigned_t;
    using std::conditional_t;
    using std::is_lvalue_reference_v;
    using std::remove_cvref_t;
    using std::remove_reference_t;
    using std::is_array_v;
    using std::remove_all_extents_t;
    using std::sentinel_for;
    using std::is_bounded_array_v;
    using std::extent_v;
    using std::input_or_output_iterator;
    using std::bidirectional_iterator;
    using std::is_nothrow_copy_constructible_v;
    using std::sized_sentinel_for;
    using std::forward_iterator;
    using std::iter_difference_t;
    using std::is_pointer_v;
    using std::remove_pointer_t;
    using std::is_object_v;
    using std::contiguous_iterator;


    // std::begin
    // std::end
    // cbegin cend rcbegin rcend size empty
    // data sszie
    // [[nodiscard]] ?? 
  template<typename>
    inline constexpr bool disable_sized_range = false; //TODO 不知道有什么用

  template<typename _Tp>
    inline constexpr bool enable_borrowed_range = false; //TODO 不知道有什么用


  namespace __detail
  {

    using __max_diff_type = __int128;
    using __max_size_type = unsigned __int128;

    template<typename _Tp> 
      concept __is_integer_like = integral<_Tp> // 整数
	|| same_as<_Tp, __max_diff_type> || same_as<_Tp, __max_size_type>;

    template<typename _Tp>
      concept __is_signed_integer_like = signed_integral<_Tp> // 有符号整数
	|| same_as<_Tp, __max_diff_type>;

    template<integral _Tp>
      constexpr make_unsigned_t<_Tp>
      __to_unsigned_like(_Tp __t) noexcept
      { return __t; }

    //两个指针之间的距离的值类型
    template<typename _Tp, bool _MaxDiff = same_as<_Tp, __int128> >
      using __make_unsigned_like_t
	= conditional_t<_MaxDiff,unsigned __int128, make_unsigned_t<_Tp>>;

    // Part of the constraints of ranges::borrowed_range
    template<typename _Tp>
      concept __maybe_borrowed_range //可能从ranges 引用
	= is_lvalue_reference_v<_Tp>
	  || enable_borrowed_range<remove_cvref_t<_Tp>>;

  }  // namespace __detail

  namespace __cust_access
  {
      //using std::ranges::__detail::__maybe_borrowed_range;
      using ::myranges::__detail::__maybe_borrowed_range;
      using std::__detail::__class_or_enum;
      using std::__detail::__decay_copy;
      using std::__detail::__member_begin;
      using std::__detail::__adl_begin;

      struct _Begin
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (is_array_v<remove_reference_t<_Tp>>)
                          return true;
                      else if constexpr (__member_begin<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp&>().begin()));
                      else
                          return noexcept(__decay_copy(begin(std::declval<_Tp&>())));
                  }

          public:
              template<__maybe_borrowed_range _Tp>
                  requires is_array_v<remove_reference_t<_Tp>> || __member_begin<_Tp>
                  || __adl_begin<_Tp>
                  constexpr auto
                  operator()(_Tp&& __t) const noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (is_array_v<remove_reference_t<_Tp>>)
                      {
                          static_assert(is_lvalue_reference_v<_Tp>);
                          using _Up = remove_all_extents_t<remove_reference_t<_Tp>>;
                          static_assert(sizeof(_Up) != 0, "not array of incomplete type");
                          return __t + 0;
                      }
                      else if constexpr (__member_begin<_Tp>)
                          return __t.begin();
                      else
                          return begin(__t);
                  }
      };

      template<typename _Tp>
          concept __member_end = requires(_Tp& __t)
          {
              { __decay_copy(__t.end()) }
              -> sentinel_for<decltype(_Begin{}(std::forward<_Tp>(__t)))>;
          };

      void end(auto&) = delete;
      void end(const auto&) = delete;

      template<typename _Tp>
          concept __adl_end = __class_or_enum<remove_reference_t<_Tp>>
          && requires(_Tp& __t)
          {
              { __decay_copy(end(__t)) }
              -> sentinel_for<decltype(_Begin{}(std::forward<_Tp>(__t)))>;
          };

      struct _End
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (is_bounded_array_v<remove_reference_t<_Tp>>)
                          return true;
                      else if constexpr (__member_end<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp&>().end()));
                      else
                          return noexcept(__decay_copy(end(std::declval<_Tp&>())));
                  }

          public:
              template<__maybe_borrowed_range _Tp>
                  requires is_bounded_array_v<remove_reference_t<_Tp>> || __member_end<_Tp>
                  || __adl_end<_Tp>
                  constexpr auto
                  operator()(_Tp&& __t) const noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (is_bounded_array_v<remove_reference_t<_Tp>>)
                      {
                          static_assert(is_lvalue_reference_v<_Tp>);
                          return __t + extent_v<remove_reference_t<_Tp>>;
                      }
                      else if constexpr (__member_end<_Tp>)
                          return __t.end();
                      else
                          return end(__t);
                  }
      };

      template<typename _Tp>
          constexpr decltype(auto)
          __as_const(_Tp&& __t) noexcept
          {
              if constexpr (is_lvalue_reference_v<_Tp>)
                  return static_cast<const remove_reference_t<_Tp>&>(__t);
              else
                  return static_cast<const _Tp&&>(__t);
          }

      struct _CBegin
      {
          template<typename _Tp>
              constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_Begin{}(__cust_access::__as_const((_Tp&&)__e))))
              requires requires { _Begin{}(__cust_access::__as_const((_Tp&&)__e)); }
          {
              return _Begin{}(__cust_access::__as_const(std::forward<_Tp>(__e)));
          }
      };

      struct _CEnd
      {
          template<typename _Tp>
              constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_End{}(__cust_access::__as_const((_Tp&&)__e))))
              requires requires { _End{}(__cust_access::__as_const((_Tp&&)__e)); }
          {
              return _End{}(__cust_access::__as_const(std::forward<_Tp>(__e)));
          }
      };

      template<typename _Tp>
          concept __member_rbegin = requires(_Tp& __t)
          {
              { __decay_copy(__t.rbegin()) } -> input_or_output_iterator;
          };

      void rbegin(auto&) = delete;
      void rbegin(const auto&) = delete;

      template<typename _Tp>
          concept __adl_rbegin = __class_or_enum<remove_reference_t<_Tp>>
          && requires(_Tp& __t)
          {
              { __decay_copy(rbegin(__t)) } -> input_or_output_iterator;
          };

      template<typename _Tp>
          concept __reversable = requires(_Tp& __t)
          {
              { _Begin{}(__t) } -> bidirectional_iterator;
              { _End{}(__t) } -> same_as<decltype(_Begin{}(__t))>;
          };

      struct _RBegin
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (__member_rbegin<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp&>().rbegin()));
                      else if constexpr (__adl_rbegin<_Tp>)
                          return noexcept(__decay_copy(rbegin(std::declval<_Tp&>())));
                      else
                      {
                          if constexpr (noexcept(_End{}(std::declval<_Tp&>())))
                          {
                              using _It = decltype(_End{}(std::declval<_Tp&>()));
                              // std::reverse_iterator copy-initializes its member.
                              return is_nothrow_copy_constructible_v<_It>;
                          }
                          else
                              return false;
                      }
                  }

          public:
              template<__maybe_borrowed_range _Tp>
                  requires __member_rbegin<_Tp> || __adl_rbegin<_Tp> || __reversable<_Tp>
                  constexpr auto
                  operator()(_Tp&& __t) const
                  noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (__member_rbegin<_Tp>)
                          return __t.rbegin();
                      else if constexpr (__adl_rbegin<_Tp>)
                          return rbegin(__t);
                      else
                          return std::make_reverse_iterator(_End{}(__t));
                  }
      };

      template<typename _Tp>
          concept __member_rend = requires(_Tp& __t)
          {
              { __decay_copy(__t.rend()) }
              -> sentinel_for<decltype(_RBegin{}(__t))>;
          };

      void rend(auto&) = delete;
      void rend(const auto&) = delete;

      template<typename _Tp>
          concept __adl_rend = __class_or_enum<remove_reference_t<_Tp>>
          && requires(_Tp& __t)
          {
              { __decay_copy(rend(__t)) }
              -> sentinel_for<decltype(_RBegin{}(std::forward<_Tp>(__t)))>;
          };

      struct _REnd
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (__member_rend<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp&>().rend()));
                      else if constexpr (__adl_rend<_Tp>)
                          return noexcept(__decay_copy(rend(std::declval<_Tp&>())));
                      else
                      {
                          if constexpr (noexcept(_Begin{}(std::declval<_Tp&>())))
                          {
                              using _It = decltype(_Begin{}(std::declval<_Tp&>()));
                              // std::reverse_iterator copy-initializes its member.
                              return is_nothrow_copy_constructible_v<_It>;
                          }
                          else
                              return false;
                      }
                  }

          public:
              template<__maybe_borrowed_range _Tp>
                  requires __member_rend<_Tp> || __adl_rend<_Tp> || __reversable<_Tp>
                  constexpr auto
                  operator()(_Tp&& __t) const
                  noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (__member_rend<_Tp>)
                          return __t.rend();
                      else if constexpr (__adl_rend<_Tp>)
                          return rend(__t);
                      else
                          return std::make_reverse_iterator(_Begin{}(__t));
                  }
      };

      struct _CRBegin
      {
          template<typename _Tp>
              constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_RBegin{}(__cust_access::__as_const((_Tp&&)__e))))
              requires requires { _RBegin{}(__cust_access::__as_const((_Tp&&)__e)); }
          {
              return _RBegin{}(__cust_access::__as_const(std::forward<_Tp>(__e)));
          }
      };

      struct _CREnd
      {
          template<typename _Tp>
              constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_REnd{}(__cust_access::__as_const((_Tp&&)__e))))
              requires requires { _REnd{}(__cust_access::__as_const((_Tp&&)__e)); }
          {
              return _REnd{}(__cust_access::__as_const(std::forward<_Tp>(__e)));
          }
      };

      template<typename _Tp>
          concept __member_size = !disable_sized_range<remove_cvref_t<_Tp>>
          && requires(_Tp&& __t)
          {
              { __decay_copy(std::forward<_Tp>(__t).size()) }
              -> __detail::__is_integer_like;
          };

      void size(auto&) = delete;
      void size(const auto&) = delete;

      template<typename _Tp>
          concept __adl_size = __class_or_enum<remove_reference_t<_Tp>>
          && !disable_sized_range<remove_cvref_t<_Tp>>
          && requires(_Tp&& __t)
          {
              { __decay_copy(size(std::forward<_Tp>(__t))) }
              -> __detail::__is_integer_like;
          };

      template<typename _Tp>
          concept __sentinel_size = requires(_Tp&& __t)
          {
              { _Begin{}(std::forward<_Tp>(__t)) } -> forward_iterator;

              { _End{}(std::forward<_Tp>(__t)) }
              -> sized_sentinel_for<decltype(_Begin{}(std::forward<_Tp>(__t)))>;
          };

      struct _Size
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (is_bounded_array_v<remove_reference_t<_Tp>>)
                          return true;
                      else if constexpr (__member_size<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp>().size()));
                      else if constexpr (__adl_size<_Tp>)
                          return noexcept(__decay_copy(size(std::declval<_Tp>())));
                      else if constexpr (__sentinel_size<_Tp>)
                          return noexcept(_End{}(std::declval<_Tp>())
                                  - _Begin{}(std::declval<_Tp>()));
                  }

          public:
              template<typename _Tp>
                  requires is_bounded_array_v<remove_reference_t<_Tp>>
                  || __member_size<_Tp> || __adl_size<_Tp> || __sentinel_size<_Tp>
                  constexpr auto
                  operator()(_Tp&& __e) const noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (is_bounded_array_v<remove_reference_t<_Tp>>)
                      {
                          return extent_v<remove_reference_t<_Tp>>;
                      }
                      else if constexpr (__member_size<_Tp>)
                          return std::forward<_Tp>(__e).size();
                      else if constexpr (__adl_size<_Tp>)
                          return size(std::forward<_Tp>(__e));
                      else if constexpr (__sentinel_size<_Tp>)
                          return __detail::__to_unsigned_like(
                                  _End{}(std::forward<_Tp>(__e))
                                  - _Begin{}(std::forward<_Tp>(__e)));
                  }
      };

      struct _SSize
      {
          template<typename _Tp>
              requires requires (_Tp&& __e)
              {
                  _Begin{}(std::forward<_Tp>(__e));
                  _Size{}(std::forward<_Tp>(__e));
              }
          constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_Size{}(std::forward<_Tp>(__e))))
              {
                  using __iter_type = decltype(_Begin{}(std::forward<_Tp>(__e)));
                  using __diff_type = iter_difference_t<__iter_type>;
                  //using std::__detail::__int_limits;
                  using std::numeric_limits;


                  auto __size = _Size{}(std::forward<_Tp>(__e));
                  if constexpr (integral<__diff_type>)
                  {
                      if constexpr (numeric_limits<__diff_type>::digits
                              < numeric_limits<ptrdiff_t>::digits)
                          return static_cast<ptrdiff_t>(__size);
                  }
                  return static_cast<__diff_type>(__size);
              }
      };

      template<typename _Tp>
          concept __member_empty = requires(_Tp&& __t)
          { bool(std::forward<_Tp>(__t).empty()); };

      template<typename _Tp>
          concept __size0_empty = requires(_Tp&& __t)
          { _Size{}(std::forward<_Tp>(__t)) == 0; };

      template<typename _Tp>
          concept __eq_iter_empty = requires(_Tp&& __t)
          {
              { _Begin{}(std::forward<_Tp>(__t)) } -> forward_iterator;
              bool(_Begin{}(std::forward<_Tp>(__t))
                      == _End{}(std::forward<_Tp>(__t)));
          };

      struct _Empty
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (__member_empty<_Tp>)
                          return noexcept(std::declval<_Tp>().empty());
                      else if constexpr (__size0_empty<_Tp>)
                          return noexcept(_Size{}(std::declval<_Tp>()) == 0);
                      else
                          return noexcept(bool(_Begin{}(std::declval<_Tp>())
                                      == _End{}(std::declval<_Tp>())));
                  }

          public:
              template<typename _Tp>
                  requires __member_empty<_Tp> || __size0_empty<_Tp>
                  || __eq_iter_empty<_Tp>
                  constexpr bool
                  operator()(_Tp&& __e) const noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (__member_empty<_Tp>)
                          return bool(std::forward<_Tp>(__e).empty());
                      else if constexpr (__size0_empty<_Tp>)
                          return _Size{}(std::forward<_Tp>(__e)) == 0;
                      else
                          return bool(_Begin{}(std::forward<_Tp>(__e))
                                  == _End{}(std::forward<_Tp>(__e)));
                  }
      };

      template<typename _Tp>
          concept __pointer_to_object = is_pointer_v<_Tp>
          && is_object_v<remove_pointer_t<_Tp>>;

      template<typename _Tp>
          concept __member_data = is_lvalue_reference_v<_Tp>
          && requires(_Tp __t) { { __t.data() } -> __pointer_to_object; };

      template<typename _Tp>
          concept __begin_data = requires(_Tp&& __t)
          { { _Begin{}(std::forward<_Tp>(__t)) } -> contiguous_iterator; };

      struct _Data
      {
          private:
              template<typename _Tp>
                  static constexpr bool
                  _S_noexcept()
                  {
                      if constexpr (__member_data<_Tp>)
                          return noexcept(__decay_copy(std::declval<_Tp>().data()));
                      else
                          return noexcept(_Begin{}(std::declval<_Tp>()));
                  }

          public:
              template<__maybe_borrowed_range _Tp>
                  requires __member_data<_Tp> || __begin_data<_Tp>
                  constexpr auto
                  operator()(_Tp&& __e) const noexcept(_S_noexcept<_Tp>())
                  {
                      if constexpr (__member_data<_Tp>)
                          return __e.data();
                      else
                          return std::to_address(_Begin{}(std::forward<_Tp>(__e)));
                  }
      };

      struct _CData
      {
          template<typename _Tp>
              constexpr auto
              operator()(_Tp&& __e) const
              noexcept(noexcept(_Data{}(__cust_access::__as_const((_Tp&&)__e))))
              requires requires { _Data{}(__cust_access::__as_const((_Tp&&)__e)); }
          {
              return _Data{}(__cust_access::__as_const(std::forward<_Tp>(__e)));
          }
      };

  } // namespace __cust_access

  inline namespace __cust
  {
    inline constexpr __cust_access::_Begin begin{};
    inline constexpr __cust_access::_End end{};
    inline constexpr __cust_access::_CBegin cbegin{};
    inline constexpr __cust_access::_CEnd cend{};
    inline constexpr __cust_access::_RBegin rbegin{};
    inline constexpr __cust_access::_REnd rend{};
    inline constexpr __cust_access::_CRBegin crbegin{};
    inline constexpr __cust_access::_CREnd crend{};
    inline constexpr __cust_access::_Size size{};
    inline constexpr __cust_access::_SSize ssize{};
    inline constexpr __cust_access::_Empty empty{};
    inline constexpr __cust_access::_Data data{};
    inline constexpr __cust_access::_CData cdata{};
  }
}

