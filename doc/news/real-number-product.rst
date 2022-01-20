**Performance:**

* Improved performance of multiplication of real numbers and elements by
  keeping more modules and products alive in caches. (Before, when doing `x*x`
  and there had been no reference to a parent of the result anymore, the parent
  module had to be recreated for every multiplication. Now we always keep the
  last 1024 such modules alive. This might require a bit of RAM but speeds up
  some computations substantially.)
