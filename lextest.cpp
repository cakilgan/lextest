#include "lextest.h"
#include <cstdio>
#include <cstring>
#include <vector>

#define _PAINT(code, x) "\033[" code "m" x "\033[0m"

#define BOLD(x) "\033[1m" x "\033[0m"
#define DIM(x) "\033[2m" x "\033[0m"

#define BLACK(x) _PAINT("30", x)
#define RED(x) _PAINT("31", x)
#define GREEN(x) _PAINT("32", x)
#define YELLOW(x) _PAINT("33", x)
#define BLUE(x) _PAINT("34", x)
#define MAGENTA(x) _PAINT("35", x)
#define CYAN(x) _PAINT("36", x)
#define WHITE(x) _PAINT("37", x)
#define GRAY(x) _PAINT("90", x)

#define BG_RED(x) _PAINT("41", x)
#define BG_GREEN(x) _PAINT("42", x)
#define BG_YELLOW(x) _PAINT("43", x)
#define BG_BLUE(x) _PAINT("44", x)

#define BOLD_RED(x) "\033[1;31m" x "\033[0m"
#define BOLD_GREEN(x) "\033[1;32m" x "\033[0m"
#define BOLD_YELLOW(x) "\033[1;33m" x "\033[0m"
#define BOLD_CYAN(x) "\033[1;36m" x "\033[0m"

using namespace lx;

test_id lx::add_case(registry &registry_, test_case::test_function func,
                     test_description description) {
    registry_.test_cases.push_back({func, description});
    return {registry_.test_cases.size() - 1};
}
int lx::run_all(registry &REGISTRY) {
    printf("\n");
    printf(BLUE(BOLD("----%s----\n")), "LEXTEST");
    printf(GREEN("starting...\n"));

    std::vector<const char *> results;

    int passed{0};
    int failed{0};

    printf(YELLOW("iterating tests...\n\n"));
    for (auto &test : REGISTRY.test_cases) {
        int passed_in{0};
        int failed_in{0};
        test.function(&test);

        if (test.controller.state == lx::test_controller::test_state::skipped) {
            printf(GRAY("SKIP: ") "%s (%s # %s)\n", "skipping this test...",
                   test.description.category, test.description.name);
            results.push_back((BG_RED("FAIL")));
            continue;
        }

        printf("---- TEST : " YELLOW("%s") " # " MAGENTA(
                   "%s") "\nDESCRIPTION : " GREEN("\"%s\"") "\n",
               test.description.category, test.description.name,
               test.description.description);
        for (auto &ev : test.events) {
            if (ev.control.skipped) {
                printf(GRAY("  SKIP: ") "%s (%s)\n", "skipping this event...",
                       ev.assertion.expression);
                continue;
            }
            if (ev.control.deprecated) {
                printf(YELLOW("  WARN: ") "%s (%s)\n",
                       "this event is deprecated.", ev.assertion.expression);
            }

            auto pass = ev.assertion.result == test_event_result::pass;
            printf(BOLD(GREEN("  $")) "    %s\n", ev.assertion.expression);
            printf("  %s    %s\n", pass ? GREEN("PASS") : BG_RED("FAIL"),
                   ev.assertion.evaluated.c_str());

            if (ev.log.message != nullptr &&
                ev.log.level == lx::log_level::NO_LOG_JUST_DETAIL) {
                printf(GRAY("  %s\n"), ev.log.message);
            } else if (ev.log.message != nullptr) {
                printf(GRAY("  LOG: %s\n"), ev.log.message);
            }

            if (pass) {
                passed_in++;
            } else {
                failed_in++;
            }
        }

        printf("---- TEST : " YELLOW("%s") " # " BOLD(
                   MAGENTA("%s")) "  [" GREEN("p:%d") " " RED("f:%d") "] %s"
                                                                      "\n",
               test.description.category, test.description.name, passed_in,
               failed_in, failed_in > 0 ? BG_RED("FAIL") : BG_GREEN("PASS"));

        if (failed_in > 0) {
            results.push_back((BG_RED("FAIL")));
            failed++;
        } else {
            results.push_back((BG_GREEN("PASS")));
            passed++;
        }

        printf("\n");
    }

    printf("%s \n", BOLD(GREEN("finished test suite")));
    printf("%s \n", YELLOW("getting overall results..."));

    printf("\n");
    printf("\n");

    printf(BG_BLUE("OVERALL") "\n");
    printf("\n");

    for (size_t i{0}; i < REGISTRY.test_cases.size(); i++) {
        auto &test = REGISTRY.test_cases[i];
        if (test.controller.state == lx::test_controller::test_state::skipped) {
            printf(GRAY("SKIPPED") " " YELLOW("%s") " # " MAGENTA("%s") "\n",
                   test.description.category, test.description.name);
            continue;
        }
        if (strcmp(results[i] ,BG_RED("FAIL")) == 0) {
            continue;
        }
        printf(YELLOW("%s") " # " MAGENTA("%s") ": %s"
                                                "\n",
               test.description.category, test.description.name, results[i]);
    }

    printf("\n");
    printf(BG_RED("FAILURES:%d") " " BG_GREEN("PASSES:%d") "\n", failed,
           passed);

    printf("\n");
    return 0;
}

test_case &lx::get_case(registry &registry_, test_id &id) {
    return registry_.test_cases[id.id];
}
