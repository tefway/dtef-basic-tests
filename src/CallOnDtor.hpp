#pragma once
#include <functional>
#include <utility>

struct CallOnDtor {
    CallOnDtor(std::function<void()> fn) : f(std::move(fn)) {}
    ~CallOnDtor() {
        try {
            f();
        } catch (...) {
        }
    }
    std::function<void()> f;
};
