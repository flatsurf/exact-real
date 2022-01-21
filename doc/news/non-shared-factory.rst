**Changed:**

* Changed `RealNumber::deglex()` slightly. Indeterminates are not ordered by
  their value but by their internal unique id. This is a breaking change.

**Performance:**

* Improved creation of products of real numbers and modules by replacing shared
  pointer cache keys with integers.
