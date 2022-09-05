现在问题是:

我们的drop_view 只能由 rvalue 初始化,那应该如何解决这个问题呢?

所以引用了`ref_view`和`all_t`

- `ref_view`: 相当于一个其它的`Range`的reference 包装类
- `all_t`是一个adaptor,作用:返回不同的类型(当然发生在 compile time)(c++ so hard 😢)
