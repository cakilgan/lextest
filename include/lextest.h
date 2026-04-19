#ifndef LEXTEST_H
#define LEXTEST_H
#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
namespace lx {
//
enum struct test_event_result { pass, fail };
enum struct test_result { pass, fail };

struct control_data {
    bool skipped = false;
    bool deprecated = false;
};
enum struct log_level { INFO, WARN, ERROR, FATAL, NO_LOG_JUST_DETAIL };
struct log_data {
    const char *message;
    log_level level;
};
struct assertion_data {
    const char *expression;
    test_event_result result;
    std::string evaluated;
};
struct test_event {
    assertion_data assertion;
    log_data log;
    control_data control;
};

struct test_description {
    const char *category;
    const char *name;
    const char *description;
};
struct test_controller {
    enum class test_state {
        //
        finished,
        skipped,
    } state;
};
struct test_case {
    using test_function = std::function<test_result(test_case *)>;
    test_function function;
    test_description description;
    test_controller controller;
    std::vector<test_event> events{};
};
struct registry {
    std::vector<test_case> test_cases;
};
struct test_id {
    size_t id;
};
namespace detail {

// you can override this for your own types
template <typename T> std::string to_string_any(const T &value) {
    if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>,
                                 std::nullptr_t>) {
        return "<null>";
    } else if constexpr (std::is_pointer_v<T>) {
        if (value == nullptr)
            return "<null>"; // runtime null pointer
        if constexpr (std::is_same_v<std::remove_cv_t<std::remove_pointer_t<T>>,
                                     char>) {
            return std::string(value);
        } else {
            std::ostringstream oss;
            oss << static_cast<const void *>(value);
            return oss.str();
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else {
        return std::to_string(value);
    }
}

struct __lx_test_event_skipper__ {
    __lx_test_event_skipper__(test_event *ev) {
        //
        ev->control.skipped = true;
    }
};
struct __lx_test_event_deprecater__ {
    __lx_test_event_deprecater__(test_event *ev) {
        //
        ev->control.deprecated = true;
    }
};
} // namespace detail
// free functions

// singleton because static init order error.
inline registry &get_registry() {
    static registry instance;
    return instance;
}
test_id add_case(registry &registry_, test_case::test_function func,
                 test_description description);
int run_all(registry &registry_);
test_case &get_case(registry &registry_, test_id &id);

}; // namespace lx

#define _LX_STR2(x) #x
#define _LX_STR1(x) _LX_STR2(x)

#define __LX_CONCAT_INNER(a, b) a##b
#define __LX_CONCAT(a, b) __LX_CONCAT_INNER(a, b)

#define __LX_TEST_EVENT_IMPL(a, b, OP, FILE_, LINE_)                           \
    [&]() {                                                                    \
        decltype(auto) __lx_a = (a);                                           \
        decltype(auto) __lx_b = (b);                                           \
        auto &ev = self->events.emplace_back();                                \
                                                                               \
        ev.assertion.expression = #a " " #OP " " #b;                           \
                                                                               \
        ev.assertion.result = (__lx_a OP __lx_b)                               \
                                  ? ::lx::test_event_result::pass              \
                                  : ::lx::test_event_result::fail;             \
                                                                               \
        ev.assertion.evaluated = ::lx::detail::to_string_any(__lx_a) +         \
                                 " " #OP " " +                                 \
                                 ::lx::detail::to_string_any(__lx_b);          \
                                                                               \
        ev.log.level = lx::log_level::NO_LOG_JUST_DETAIL;                      \
        ev.log.message = FILE_ ":" _LX_STR1(LINE_);                            \
                                                                               \
        return &ev;                                                            \
    }()

#define __LXSTR_IMPL(a, b, op, FILE_, LINE_)                                   \
    [&]() {                                                                    \
        auto &&__lx_a = (a);                                                   \
        auto &&__lx_b = (b);                                                   \
        auto &ev = self->events.emplace_back();                                \
                                                                               \
        ev.assertion.expression = #a #op #b;                                   \
                                                                               \
        ev.assertion.result = (std::string(__lx_a) op std::string(__lx_b))     \
                                  ? ::lx::test_event_result::pass              \
                                  : ::lx::test_event_result::fail;             \
                                                                               \
        ev.assertion.evaluated =                                               \
            std::string("\"") + std::string(__lx_a) + "\" " #op "\"" +         \
            std::string(__lx_b) + "\" = " +                                    \
            ((ev.assertion.result == ::lx::test_event_result::pass)            \
                 ? "true"                                                      \
                 : "false");                                                   \
                                                                               \
        ev.log.level = lx::log_level::NO_LOG_JUST_DETAIL;                      \
        ev.log.message = FILE_ ":" _LX_STR1(LINE_);                            \
    }()

#define LXGT(a, b) __LX_TEST_EVENT_IMPL(a, b, >, __FILE__, __LINE__)
#define LXLT(a, b) __LX_TEST_EVENT_IMPL(a, b, <, __FILE__, __LINE__)
#define LXGE(a, b) __LX_TEST_EVENT_IMPL(a, b, >=, __FILE__, __LINE__)
#define LXLE(a, b) __LX_TEST_EVENT_IMPL(a, b, <=, __FILE__, __LINE__)
#define LXEQ(a, b) __LX_TEST_EVENT_IMPL(a, b, ==, __FILE__, __LINE__)
#define LXNEQ(a, b) __LX_TEST_EVENT_IMPL(a, b, !=, __FILE__, __LINE__)
#define LXTRUE(x) __LX_TEST_EVENT_IMPL(x, true, ==, __FILE__, __LINE__)
#define LXFALSE(x) __LX_TEST_EVENT_IMPL(x, true, ==, __FILE__, __LINE__)
#define LXNULL(x) __LX_TEST_EVENT_IMPL(x, nullptr, ==, __FILE__, __LINE__)
#define LXSTREQ(a, b) __LXSTR_IMPL(a, b, ==, __FILE__, __LINE__)
#define LXSTRNEQ(a, b) __LXSTR_IMPL(a, b, !=, __FILE__, __LINE__)

#define LXSKIPTEST(as)                                                         \
    self->controller.state = lx::test_controller::test_state::skipped;         \
    return lx::test_result::as;
#define LXSKIP                                                                 \
    ::lx::detail::__lx_test_event_skipper__ __LX_CONCAT(__skipper,             \
                                                        __COUNTER__) =
#define LXDEPRECATED                                                           \
    ::lx::detail::__lx_test_event_deprecater__ __LX_CONCAT(__deprecater,       \
                                                           __COUNTER__) =

#define LXTEST(category, name, description)                                    \
    lx::test_result category##name##LX_TEST_STATIC(lx::test_case *self);       \
    static lx::test_id __LX_CONCAT(__lx_test_case_id, __COUNTER__) =           \
        lx::add_case(lx::get_registry(), category##name##LX_TEST_STATIC,       \
                     {#category, #name, description});                         \
    lx::test_result category##name##LX_TEST_STATIC(lx::test_case *self)

#endif // LEXTEST_H
