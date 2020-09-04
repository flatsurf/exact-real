**Fixed:**

* deserialization of modules over number fields; shared pointers to renf_class was not compared correctly which lead to a module not being identical to a deserialization of a serialization of itself; part of #106.
