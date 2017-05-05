#ifndef TESTING_H_
#define TESTING_H_

#include <functional>
#include <iostream>
#include <set>
#include <string>

#define begin_test() std::cout << "Starting " << __FUNCTION__ << ".\n"

void check(bool test, std::string message) {
    if (test) {
        std::cout << "  Test ok. " << message << std::endl;
    } else {
        std::cout << "  TEST FAILED! " << message << std::endl;
    }
}

void check_false(bool test, std::string message) {
    check(!test, message);
}

template <class T>
void check_equal(T const& p1, T const& p2, std::string message) {
    check(p1 == p2, message);
}

template <class T>
void check_not_equal(T const& p1, T const& p2, std::string message) {
    check_false(p1 == p2, message);
}

template <class T>
void check_same_set(T const& p1, T const& p2, std::string message) {
    std::set<typename T::value_type> set1(p1.begin(), p1.end());
    std::set<typename T::value_type> set2(p2.begin(), p2.end());

    check(set1 == set2, message);
}

void check_no_exception_thrown(std::function<void(void)> function,
        std::string message) {
    bool thrown = false;

    try {
        function();
    } catch (...) {
        thrown = true;
    }

    check_false(thrown, message);
}

template <class E>
void check_exception_thrown(std::function<void(void)> function, std::string message) {
    bool thrown = false;

    try {
        function();
    } catch (E e) {
        thrown = true;
    } catch (...) {
        thrown = false;
    }

    check(thrown, message);
}

#endif
