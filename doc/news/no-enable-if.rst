**Changed:**

* Changed declaration of some methods in `Element` from `template<bool, enable_if_t<bool, bool>>` to `template<bool, void>` since the former uses different C++ symbol mangling in different versions of Clang. This is a **breaking ABI change**. The API is unchanged. Existing code can just be recompiled without any changes.
