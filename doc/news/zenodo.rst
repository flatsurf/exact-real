**Changed:**

* Renamed `exact-real.hpp` to `local.hpp`. The old header file `exact-real.hpp`
  still exists. However, it imports the entire exact-real API so in particular
  `local.hpp`.

**Fixed:**

* Fixed compilation error on macOS: `'value' is unavailable: introduced in macOS 10.13`
