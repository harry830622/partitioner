# fiduccia-mattheyses-partitioning

## How to build

```sh
make
```

> Please make sure your `clang` is the latest version, or the program will be
> extremely slow due to the O(n) time complexity of old `list<>::size()`
> implementation.
> Or you can just use the statically compiled version `fmp_static`.
