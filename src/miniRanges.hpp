#pragma once
#include <iostream>
#include <type_traits>
#include <functional>
#include <iterator>

namespace miniRanges {
    /* =============== type_traits ===============*/
    template<typename T>
    using itertor_t = decltype(std::begin(std::declval<T&>()));

    /* =============== type_traits_end ===============*/
    struct view_base {};

    template<typename _Derived>
    class view_interface : public view_base
    {
        private:
            //返回 _Derived 类型的引用
            _Derived& _M_derived(){
                return static_cast<_Derived&>(*this);
            };
        public:
            bool empty(){
                return std::begin(_M_derived()) == std::end(_M_derived());
            }
    };

    template<typename T>
    class subrange : public view_interface<T> {
    };

    // ============== Adaptor 适配器类 BEG ==============
    template<typename _Callable>
        struct _RangeAdaptorClosure;

    template<typename _Callable>
    struct _RangeAdaptor {
        protected:
            _Callable _M_callable;
        public:
            //_RangeAdaptor(_Callable const & = {} ){}

            _RangeAdaptor(_Callable __callable)
                : _M_callable(std::move(__callable))
            {}

            //   (1) adaptor(range, args...)
            //   (2) adaptor(args...)(range)
            //   (3) range | adaptor(args...)
            template<typename... _Args>
                auto operator()(_Args&&... __args){
                    //if constexpr ( std::is_invocable_v<_Callable, _Args... >) {
                        return _Callable{}(std::forward<_Args>(__args)...);
                    //}
                }

    };
    template<typename _Callable>
        struct _RangeAdaptorClosure : public _RangeAdaptor<_Callable>
    {
        //using _RangeAdaptor<_Callable>::_RangeAdaptor;
        _RangeAdaptorClosure(_Callable __callable): _RangeAdaptor<_Callable>(__callable)
        {}

        template<typename _Range>
        auto operator()(_Range&& __r){
            return this->_M_callable(std::forward<_Range>(__r));
        };
    };

    template<typename _Callable>
        struct __Make_RangeAdaptorClosure : public _RangeAdaptor<_Callable>
    {

        __Make_RangeAdaptorClosure(_Callable __callable) : _RangeAdaptor<_Callable>(__callable)
        {}

        //using _RangeAdaptor<_Callable>
        //   (3) range | adaptor(args...)
        template<typename... _Args>
        auto operator()(_Args&&... __args){
            //std::cout << sizeof...(__args) << std::endl;
            auto __closure = [__args...]<typename _Range>(_Range && __r){
                return _Callable{}(std::forward<_Range>(__r),__args...);
            };
            return _RangeAdaptorClosure<decltype(__closure)>(std::move(__closure));
        }
    };
    // ============== Adaptor 适配器类 END ==============

    template<typename _Range> //requires is_object_v<_Range>
        class ref_view : public view_interface<ref_view<_Range>>
    {
        private:
            _Range * _M_r = nullptr;
        public:
            ref_view() = default;

            //构造函数
            template<typename U>
            ref_view(U&& __t)
                :_M_r(std::addressof(static_cast<_Range&>(std::forward<U>(__t))) )
            {
            }

            //返回 原数据的引用
            _Range& base() const {
                return *_M_r;
            }

            itertor_t<_Range> begin() const {
                return std::begin(*_M_r);
            }

            itertor_t<_Range> end() const {
                return std::end(*_M_r);
            }

            auto size() const {
                return std::size(*_M_r);
            }
    };

    //_RangeAdaptorClosure all
        //= [] <typename _Range> (_Range&& __r)
        //{
        //};

    template<typename _Vp,typename  _Pred> //requires is_object_v<_Vp>
        class filter_view: public view_interface<filter_view<_Vp,_Pred>> {
            private:
                struct _Sentinel;

                struct _Iterator{
                    using _Vp_iter = itertor_t<_Vp>;

                    _Iterator(filter_view& __parent ,_Vp_iter __current)
                        :   _M_current(__current)
                            ,_M_parent(std::addressof(__parent))
                    {}

                    _Iterator& operator++(){
                        _M_current = std::find_if(
                                    std::move(++ _M_current),
                                    std::end(_M_parent->_M_base),
                                    //std::ref()
                                    _M_parent->_M_pred
                                );
                        return *this;
                    }

                    //std::reference_wrapper<typename _Tp>
                    typename std::iterator_traits<_Vp_iter>::reference operator*(){
                        return *_M_current;
                    }

                    void operator++(int)
                    { ++*this; }

                    friend bool operator==(const _Iterator& __x,const _Iterator& __y){
                        return __x._M_current == __y._M_current;
                    }
                    filter_view * _M_parent;
                    _Vp_iter _M_current = _Vp_iter();
                };
                struct _Sentinel{};

                _Vp _M_base = _Vp();
                _Pred _M_pred;

            public:
                filter_view() = default;
                //filter_view(_Vp __r,_Pred __pred) : _M_base(std::move(__r)),_Pred(std::move(__pred))
                filter_view(_Vp __r,_Pred __pred) : _M_base(__r),_M_pred(__pred)
                {}

                _Iterator begin(){
                    return {*this,std::begin(_M_base) };
                }

                _Iterator end(){
                    return {*this,std::end(_M_base) };
                }

        };

    namespace views {
        inline __Make_RangeAdaptorClosure filter = 
            []<typename _Range,typename _Pred>(_Range&& __r,_Pred&& __p){
                //std::cout << typeid(_Pred).name() << std::endl;
                return filter_view(std::forward<_Range>(__r),std::forward<_Pred>(__p));
            };
    }
}
