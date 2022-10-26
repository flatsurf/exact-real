**Fixed:**

* Fixed serialization with cereal 1.3.1+ in the same way we did for e-antic in https://github.com/flatsurf/e-antic/pull/242. The old ABI is still present but calling it cannot be supported in a sane way. The cereal::JSONOutputArchive has a breaking ABI change in 1.3.1 so code that builds against libexactreal cereal support from earlier versions must be recompiled. Otherwise, there will be an error at runtime.
