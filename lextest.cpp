#include "lextest.h"
#include <cstdio>
#include <vector>
#include <string>

void lx::parse_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--no-color") {
            config.use_color = false;
        }
        else if (arg == "--color") {
            config.use_color = true;
        }
        else if (arg == "--json") {
            config.json = true;
        }
        else if (arg == "--json-pretty") {
            config.json_pretty = true;
        }
        else if (arg == "--list") {
            config.list_only = true;
        }
        else if (arg == "--verbose") {
            config.verbose = true;
        }
        else if (arg == "--fail-fast") {
            config.fail_fast = true;
        }
        else if (arg.rfind("--filter=", 0) == 0) {
            config.filter = arg.substr(9);
        }
    }
}

inline std::string paint(const char* code, const std::string& text) {
    if (!lx::config.use_color) return text;
    return "\033[" + std::string(code) + "m" + text + "\033[0m";
}

inline std::string bold(const std::string& s)   { return paint("1", s); }
inline std::string red(const std::string& s)    { return paint("31", s); }
inline std::string green(const std::string& s)  { return paint("32", s); }
inline std::string yellow(const std::string& s) { return paint("33", s); }
inline std::string blue(const std::string& s)   { return paint("34", s); }
inline std::string magenta(const std::string& s){ return paint("35", s); }
inline std::string gray(const std::string& s)   { return paint("90", s); }

inline std::string bg_red(const std::string& s)   { return paint("41", s); }
inline std::string bg_green(const std::string& s) { return paint("42", s); }
inline std::string bg_blue(const std::string& s)  { return paint("44", s); }

using namespace lx;

test_id lx::add_case(registry &registry_, test_case::test_function func,
                     test_description description) {
    registry_.test_cases.push_back({func, description});
    return {registry_.test_cases.size() - 1};
}

int lx::run_all(registry &REGISTRY) {
    if(config.json){
        printf("%s",lx::run_json(REGISTRY,config.json_pretty).c_str());
        return 0;
    }

    printf("\n");

    std::string header = blue(bold("----LEXTEST----"));
    printf("%s\n", header.c_str());
    printf("%s\n", green("starting...").c_str());

    printf("%s\n",(yellow("FILTERING CATEGORY: ")+config.filter).c_str());

    std::vector<test_result> results;

    int passed{0};
    int failed{0};

    printf("%s\n\n", yellow("iterating tests...").c_str());

    for (auto &test : REGISTRY.test_cases) {
        int passed_in{0};
        int failed_in{0};

        if(config.filter!="ALL" && config.filter != test.description.category){
            continue;
        }

        test.function(&test);

        if (test.controller.state == lx::test_controller::test_state::skipped) {
            printf("%s%s (%s # %s)\n",
                   gray("SKIP: ").c_str(),
                   "skipping this test...",
                   test.description.category,
                   test.description.name);

            results.push_back(test_result::fail);
            continue;
        }

        if(!config.list_only)
            printf("---- TEST : %s # %s\n%s%s",
                yellow(test.description.category).c_str(),
                magenta(test.description.name).c_str(),
                config.verbose ? "DESCRIPTION : " : "",
                config.verbose ? (green(test.description.description)+"\n").c_str(): "");

        for (auto &ev : test.events) {
            if (ev.control.skipped) {
                if(!config.list_only)
                    printf("%s%s (%s)\n\n",
                        gray("  SKIP: ").c_str(),
                        "skipping this event...",
                        ev.assertion.expression);
                continue;
            }

            if (ev.control.deprecated) {
                if(!config.list_only)
                    printf("%s%s (%s)\n",
                        yellow("  WARN: ").c_str(),
                        "this event is deprecated.",
                        ev.assertion.expression);
            }

            bool pass = ev.assertion.result == test_event_result::pass;

            if(!config.list_only){
                if(config.verbose)
                    printf("%s    %s\n",
                        bold(green("  $")).c_str(),
                        ev.assertion.expression);

                printf("  %s    %s%s",
                   (pass ? green("PASS") : bg_red("FAIL")).c_str(),
                   ev.assertion.evaluated.c_str(),config.verbose ? "\n" : "");

                if (ev.log.message != nullptr && config.verbose) {
                    if (ev.log.level == lx::log_level::NO_LOG_JUST_DETAIL) {
                        printf("%s%s\n",
                            gray("  ").c_str(),
                            ev.log.message);
                    } else {
                    printf("%s%s\n",
                           gray("  LOG: ").c_str(),
                           ev.log.message);
                    }
                }
            }

            if (pass)
                passed_in++;
            else
                failed_in++;

            if(!config.list_only)
                printf("\n");
        }

        bool test_failed = failed_in > 0;


        std::string p_str = "p:" + std::to_string(passed_in);
        std::string f_str = "f:" + std::to_string(failed_in);

        if(!config.list_only)
            printf("---- TEST : %s # %s  [%s %s] %s\n",
                yellow(test.description.category).c_str(),
                bold(magenta(test.description.name)).c_str(),
                green(p_str).c_str(),
                red(f_str).c_str(),
                (test_failed ? bg_red("FAIL") : bg_green("PASS")).c_str());

        if (test_failed) {
            results.push_back(test_result::fail);
            failed++;
        } else {
            results.push_back(test_result::pass);
            passed++;
        }

        if(!config.list_only)
            printf("\n");
    }

    printf("%s\n", bold(green("finished test suite")).c_str());
    printf("%s\n\n", yellow("getting overall results...").c_str());

    printf("%s\n\n", bg_blue("OVERALL").c_str());

    for (size_t i = 0; i < REGISTRY.test_cases.size(); i++) {
        auto &test = REGISTRY.test_cases[i];
        
        if(config.filter!="ALL" && config.filter != test.description.category){
            continue;
        }

        if (test.controller.state == lx::test_controller::test_state::skipped) {
            printf("%s %s # %s\n",
                   gray("SKIPPED").c_str(),
                   yellow(test.description.category).c_str(),
                   magenta(test.description.name).c_str());
            continue;
        }

        if(!config.verbose)
            if (results[i] == test_result::pass)
                continue;

        const char* res_str =
            (results[i] == test_result::pass)
                ? bg_green("PASS").c_str()
                : bg_red("FAIL").c_str();

        printf("%s # %s: %s\n",
               yellow(test.description.category).c_str(),
               magenta(test.description.name).c_str(),
               res_str);
    }

    printf("\n");

    std::string fail_str = "FAILURES:" + std::to_string(failed);
    std::string pass_str = "PASSES:" + std::to_string(passed);

    printf("%s %s\n",
           bg_red(fail_str).c_str(),
           bg_green(pass_str).c_str());

    printf("\n");
    return 0;
}

test_case &lx::get_case(registry &registry_, test_id &id) {
    return registry_.test_cases[id.id];
}


struct json_event {
    std::string expression;
    std::string evaluated;
    bool pass;
    bool skipped;
    bool deprecated;
    std::string file;
};

struct json_test {
    std::string category;
    std::string name;
    std::string description;

    std::vector<json_event> events;

    int passed = 0;
    int failed = 0;
    bool skipped = false;
};

struct json_report {
    std::vector<json_test> tests;
    int total_passed = 0;
    int total_failed = 0;
};

struct json_writer {
    std::ostringstream out;
    int indent_level = 0;
    bool pretty = true;

    json_writer(bool pretty_mode = true)
        : pretty(pretty_mode) {}

    // ---- indent helper ----
    void indent() {
        if (!pretty) return;
        for (int i = 0; i < indent_level; i++)
            out << "  ";
    }

    void newline() {
        if (pretty) out << "\n";
    }

    void key(const std::string &k) {
        indent();
        out << "\"" << k << "\":";
    }

    void value(const std::string &v) {
        out << "\"" << v << "\"";
    }

    void value(bool v) {
        out << (v ? "true" : "false");
    }

    void value(int v) {
        out << v;
    }

    void begin_object() {
        indent();
        out << "{";
        indent_level++;
        newline();
    }

    void end_object(bool comma = false) {
        indent_level--;
        newline();
        indent();
        out << "}";
        if (comma) out << ",";
        newline();
    }

    void begin_array() {
        indent();
        out << "[";
        indent_level++;
        newline();
    }

    void end_array(bool comma = false) {
        indent_level--;
        newline();
        indent();
        out << "]";
        if (comma) out << ",";
        newline();
    }

    std::string str() {
        return out.str();
    }
};

std::string lx::run_json(registry &REGISTRY, bool pretty) {
    json_writer jw(pretty);

    jw.begin_object();

    jw.key("tests");
    jw.begin_array();

    for (size_t i = 0; i < REGISTRY.test_cases.size(); i++) {
        auto &test = REGISTRY.test_cases[i];

        test.function(&test);

        jw.begin_object();

        jw.key("category"); jw.value(test.description.category); jw.out << ",";
        jw.newline();

        jw.key("name"); jw.value(test.description.name); jw.out << ",";
        jw.newline();

        jw.key("description"); jw.value(test.description.description); jw.out << ",";
        jw.newline();

        jw.key("events");
        jw.begin_array();

        for (size_t j = 0; j < test.events.size(); j++) {
            auto &ev = test.events[j];

            jw.begin_object();

            jw.key("expression"); jw.value(ev.assertion.expression); jw.out << ",";
            jw.newline();

            jw.key("evaluated"); jw.value(ev.assertion.evaluated); jw.out << ",";
            jw.newline();

            jw.key("pass"); jw.value(ev.assertion.result == test_event_result::pass);

            jw.end_object(j + 1 < test.events.size());
        }

        jw.end_array();

        jw.end_object(i + 1 < REGISTRY.test_cases.size());
    }

    jw.end_array();
    jw.end_object();

    return jw.str();
}