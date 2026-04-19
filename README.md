# lextest

Lightweight C++ test framework. Header + single translation unit, no dependencies.

---

## Integration

Copy the `lextest/` folder into your project, then in your `CMakeLists.txt`:

```cmake
add_subdirectory(lextest)
include(lextest/cmake/lextest.cmake)

lx_add_tests(
    test1.cpp
    test2.cpp
)
```

That's it. Each file gets its own executable with an entrypoint provided automatically.

---

## Writing Tests

```cpp
#include "lextest.h"

LXTEST(Math, addition, "basic arithmetic") {
    int a = 2, b = 3;
    LXEQ(a + b, 5);
    LXGT(b, a);
    return lx::test_result::pass;
}
```

`LXTEST(category, name, description)` registers the test at startup. The return value is used as the overall result — the framework also derives it from the events, so returning `pass` at the end is conventional.

---

## Assertions

| Macro | Check |
|---|---|
| `LXEQ(a, b)` | a == b |
| `LXNEQ(a, b)` | a != b |
| `LXGT(a, b)` | a > b |
| `LXLT(a, b)` | a < b |
| `LXGE(a, b)` | a >= b |
| `LXLE(a, b)` | a <= b |
| `LXTRUE(x)` | x == true |
| `LXFALSE(x)` | x == false |
| `LXNULL(x)` | x == nullptr |
| `LXSTREQ(a, b)` | std::string(a) == std::string(b) |

All assertions are non-fatal — the test continues after a failure and all events are reported.

---

## Skipping

```cpp
LXTEST(Feature, not_ready, "wip") {
    LXSKIPTEST(pass);  // skip, assume pass
}

LXTEST(Feature, known_bug, "wip") {
    LXSKIPTEST(fail);  // skip, assume fail
}
```

Skipped tests appear in the summary but don't run.

---

## Output

```
----LEXTEST----

---- TEST : Math # addition
DESCRIPTION : "basic arithmetic"
  $    a + b == 5
  PASS    5 == 5
  test1.cpp:8

---- TEST : Math # addition  [p:1 f:0] PASS

OVERALL

FAILURES:0 PASSES:1
```

---

## Requirements

- C++17
- CMake 3.20+
