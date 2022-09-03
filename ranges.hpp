#include <ranges>
#include <type_traits>
#include <cassert>
#include <functional>
#include <iterator>

namespace myranges {

    struct view_base {};

    //CRTP
    // 通用工具函数
    template<typename _Derived>
    class view_interface : public view_base {
        private:
            _Derived & _M_Derived() {
                return static_cast<_Derived&>(*this);
            }

        public:
            bool empty();
            explicit operator bool();
            auto data();
            auto size();
            auto front();
            auto back();
            decltype(auto) operator[](std::size_t);

    };

    namespace __adaptor {

        template<typename T>
        inline constexpr auto
        __maybe_refwrap(T & __args)
        { return std::reference_wrapper<T>{__args}; }

        template<typename T>
        inline constexpr auto
        __maybe_refwrap(const T & __args)
        { return std::reference_wrapper<const T>{__args}; }

        template<typename T>
        inline constexpr auto
        __maybe_refwrap(T && __args)
        { return std::forward<T>(__args); }

        template<typename _Callable>
        struct _RangeAdaptorClosure;

        template<typename _Callable>
        struct _RangeAdaptor {
            protected:
                _Callable _M_callable;
            public:

            //构造函数
            _RangeAdaptor(_Callable __callable)
                : _M_callable(std::move(__callable))
            {}


            //核心,返回一个_RangeAdaptorClosure对象
            template<typename... _Args>
                requires (sizeof...(_Args) >= 1)
            auto operator()(_Args&&... __args) {
                //分类
                // 1 只需要参数就可以运行,不需要Ranges
                if constexpr (std::is_invocable_v<_Callable,decltype(__args)...>)
                {
                    return _M_callable(std::forward<_Args>(__args)...);
                }
                else {
                    auto __closure = 
                    [...__args = __maybe_refwrap(std::forward<_Args>(__args))]
                <typename _Range>(_Range&& __r)
                {
                    //为什么要 default construct ?
                    return _Callable{}(
                            std::forward<_Range>(__r),
                            (static_cast<std::unwrap_reference_t<
                                std::remove_const_t<decltype(__args)>>>(__args))...
                            );
                };

                    using _ClosureType = decltype(__closure);
                    return _RangeAdaptorClosure<_ClosureType>(std::move(__closure));

                }

            }
            // 0 what is --> = [...__args(__maybe_refwrap(std::forward<_Args>(__args)))]
            // 1 How to understand template lambda ?
            // template lambda 本质是一个template class;
            // 这个class的 operator() 是template 但自己不是template,
            // 所以type一直没有变
            // 2 ?(static_cast<unwrap_reference_t
                                                         // <remove_const_t<decltype(__args)>>>
                                                         // (__args))
            // 3. why decltype a template lambda type ?
            // using _ClosureType = decltype(__closure);
            // 最后创建了一个 AdaptorClosure

        }; // end struct _RangeAdaptor
           //
           //

        template<typename _Callable>
        struct _RangeAdaptorClosure : public _RangeAdaptor<_Callable> {
            //核心思想是闭包
            //构造函数
            //重载 operator |
            using _RangeAdaptor<_Callable>::_RangeAdaptor;

            //重载 operator () 对给定了 range 进行操作
            template<std::ranges::viewable_range _Range>
            requires std::is_invocable_v<_Callable,_Range>
            auto operator()(_Range&& __r) 
            {
                if constexpr (std::is_default_constructible_v<_Callable>)
                    return _Callable{}(std::forward<_Range>(__r));
                else
                    return this->_M_callable(std::forward<_Range>(__r));

            }

        };
    } // end namespace __adaptor
      //




    template<typename ViewRange>
    class drop_view : public view_interface<drop_view<ViewRange>>
    {
        private:
            ViewRange m_r;
            std::ranges::range_difference_t<ViewRange> m_count;
        public:
            drop_view(ViewRange _r,std::ranges::range_difference_t<ViewRange> __count)
                : m_r(std::move(_r)),m_count(__count)
            {
                assert(__count >= 0);
            }

            auto begin(){
                return std::ranges::next(std::ranges::begin(m_r),m_count,std::ranges::end(m_r));
            }
    };

} // end namespace myranges