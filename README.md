# lextest

Lightweight C++ test framework. Header + single translation unit, no dependencies.

---

## Integration

Copy the `lextest/` folder into your project, then in your `CMakeLists.txt`:

```cmake
add_subdirectory(lextest)
include(lextest/cmake/lextest.cmake)

lx_add_test(example-1 test1.cpp)
```

That's it. Each test target gets its own executable with an entrypoint provided automatically via `entrypoint.cpp`.

> **Note:** The CMake function is `lx_add_test` (singular), not `lx_add_tests` , this means it generates 1 target with multiple files.

---

## Writing Tests

```cpp
#include <lextest.h>

LXTEST(Math, Addition, "basic arithmetic") {
    int a = 2, b = 3;
    LXEQ(a + b, 5);
    LXGT(b, a);
    LXRETURN;
}
```

`LXTEST(category, name, description)` registers the test at startup. Use `LXRETURN` to exit early — it expands to `return lx::test_result::pass`.

---

## Assertions

All assertions are **non-fatal** — the test continues after a failure and all events are reported.

| Macro | Check |
|---|---|
| `LXEQ(a, b)` | `a == b` |
| `LXNEQ(a, b)` | `a != b` |
| `LXGT(a, b)` | `a > b` |
| `LXLT(a, b)` | `a < b` |
| `LXGE(a, b)` | `a >= b` |
| `LXLE(a, b)` | `a <= b` |
| `LXTRUE(x)` | `x == true` |
| `LXFALSE(x)` | `x == false` |
| `LXNULL(x)` | `x == nullptr` |
| `LXSTREQ(a, b)` | `std::string(a) == std::string(b)` |
| `LXSTRNEQ(a, b)` | `std::string(a) != std::string(b)` |

---

## Event Modifiers

Event modifiers are placed **before** an assertion macro on the same line.

### `LXSKIP` — Skip an event

The assertion is not evaluated or counted. A skip message appears in the log.

```cpp
LXTEST(Feature, example, "skip demo") {
    LXSKIP LXGE(5, 12);  // not evaluated, not counted
    LXRETURN;
}
```

```
  SKIP: skipping this event... (5 >= 12)
```

`event.control.skipped` will be `true`.

---

### `LXDEPRECATED` — Deprecate an event

The assertion **is** evaluated and counted, but a warning is printed. Useful for tracking assertions you intend to remove.

```cpp
LXTEST(Feature, example, "deprecated demo") {
    LXDEPRECATED LXGE(5, 12);  // evaluated, warning shown
    LXRETURN;
}
```

```
  WARN: this event is deprecated. (5 >= 12)
  $    5 >= 12
  FAIL    5 >= 12
```

`event.control.deprecated` will be `true`.

---

## Skipping Tests

To skip an entire test, use `LXSKIPTEST(result)` as the first statement:

```cpp
LXTEST(Feature, not_ready, "wip") {
    LXSKIPTEST(pass);  // skip, assume pass
}

LXTEST(Feature, known_bug, "wip") {
    LXSKIPTEST(fail);  // skip, assume fail
}
```

Skipped tests appear in the summary but do not run.

---

## CLI Options

```
--no-color        Disable ANSI color output
--color           Enable ANSI color output (default)

--list            List tests without running them
--verbose         Show descriptions, file locations, expression details and verbose summary
--filter=CATEGORY Run only tests in the given category

--fail-fast       Stop a test after the first failing assertion

--json            Converts all test data to json
--json-pretty     Makes json data indented and human readable (flag --json must be present)
```

Example:

```sh
./my-tests --filter=Math --verbose
./my-tests --no-color --fail-fast
```

---

## Output

```
----LEXTEST----

---- TEST : Math # Addition
  PASS    5 == 5
  PASS    3 > 2

---- TEST : Math # Addition  [p:2 f:0] PASS

OVERALL

FAILURES:0 PASSES:1
```

With `--verbose`:

```
---- TEST : Math # Addition
DESCRIPTION : "basic arithmetic"
  $    a + b == 5
  PASS    5 == 5
  test1.cpp:8
```

---

## Custom Type Stringification

The `lx::detail::to_string_any` template function is used to format assertion values. You can specialize it for your own types:

```cpp
namespace lx::detail {
    template <>
    std::string to_string_any(const MyType& value) {
        return value.to_string();
    }
}
```

---

## JSON Output


---

## Requirements

- C++17
- CMake 3.20+