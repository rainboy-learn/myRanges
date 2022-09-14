
基于gcc 10 手动实现 ranges,以便理解ranges的原理

Base on gcc 10, implement of ranges library to understand principle.



std::ranges::view<T>
- 是range

```plaintext

template<typename _Tp>
    inline constexpr bool enable_view = derived_from<_Tp, view_base>;

template<typename _Tp>
    concept view
    = range<_Tp> && movable<_Tp> && default_initializable<_Tp>
    && enable_view<_Tp>;
```

## 理解

gcc10 ranges 分为三个方面

1. view 视图,是另一个range的视图,表示不能更改另一个range
2. adaptor,适配器,插入不同的函数完成轮廓上一样的不同功能
3. algorithm 这个不在`<ranges>`头文件内,所以这里不讨论

当然在`<ranges>`头文件里还有一些细碎的`concept`概念

- `enable_borrowed_range`

## 适配器 adaptor

有下面两个适配器

```plaintext
template<typename _Callable>
__adaptor::_RangeAdaptor
功能:
1.构造: 存入一个callable 对象
2.重载`operator()`,返回一个`_RangeAdaptorClosure`
  - 如果传进来的参数可以直接被callable运行,返回运行后的值
  - 否则,返回适配器闭包_RangeAdaptorClosure

template<typename _Callable>
struct _RangeAdaptorClosure : public _RangeAdaptor<_Callable> {

1. 重载`operator(ranges && r)`, 对range进行执行
1. 重载`operator|`,调用`operator(ranges&& r)`
```

