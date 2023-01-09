#include <iostream>
#include <memory>
#include <shared_mutex>
#include <vector>
#include <iomanip>

template<typename T> auto func(T t) { return t; }

template<class T>
constexpr T pi = T(3.1415926535897932385L);

template<typename T, typename U>
struct A {
    T t;
    U u;
};

template<typename T>
using B = A<T, int>;

constexpr int factorial(int n) { // C++14 和 C++11均可
    return n <= 1 ? 1 : (n * factorial(n - 1));
}

constexpr int factorial2(int n) { // C++11中不可，C++14中可以
    int ret = 0;
    for (int i = 0; i < n; ++i) {
        ret += i;
    }
    return ret;
}

constexpr int func(bool flag) { // C++11中不可，C++14中可以
    if (flag) return 1;
    else return 0;
}

struct [[deprecated]] C { };

struct ThreadSafe {
    mutable std::shared_timed_mutex mutex_;
    int value_;

    ThreadSafe() {
        value_ = 0;
    }

    int get() const {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_, std::chrono::milliseconds(200));
        return value_;
    }

    void increase() {
        std::unique_lock<std::shared_timed_mutex> lock(mutex_);
        value_ += 1;
    }
};

int main()
{
    // lambda support return auto type
    auto lambda = [](auto x, auto y) {return x + y; };
    auto a = lambda(10.1, 2);
    std::cout << a << " type name is:" << typeid(a).name() << std::endl;

    std::unique_ptr<int> ptr(new int(10));
    auto lambda2 = [value = std::move(ptr)]{ return *value; };
    auto a1 = lambda2();
    std::cout << a1 << " " << ptr.get() << std::endl;

    std::cout << func(4) << std::endl;
    std::cout << func(3.4) << std::endl;

    std::cout << pi<int> << std::endl; // 3
    std::cout << pi<double> << std::endl; // 3.14159

    B<double> b;
    b.t = 10;
    b.u = 20;
    std::cout << b.t << std::endl;
    std::cout << b.u << std::endl;

    std::cout << factorial2(10) << std::endl;

    // C c; // complete fail

    int a3 = 0b0001'0011'1010;
    double b3 = 3.14'1234'1234'1234;
    double b4 = 3.1'4'1'234'12'34'1234;

    std::unique_ptr<int> ptr2 = std::make_unique<int>();

    //ThreadSafe thread_safe;

    //for (auto i = 0; i < 10; ++i)
    //{
    //    std::thread([](int i, ThreadSafe* thread_safe) {
    //        thread_safe->increase();
    //        //std::cout << "thread id " << i << ": " << thread_safe.get() << std::endl;
    //        }, i, &thread_safe).detach();
    //}
    //std::cout << "thread safe value: " << thread_safe.get() << std::endl;

    std::vector<int> v;
    std::exchange(v, { 1,2,3,4 });
    std::cout << v.size() << std::endl;
    for (int a : v) {
        std::cout << a << " ";
    }

    std::string str = "hello world";
    std::cout << str << std::endl;
    std::cout << std::quoted(str) << std::endl;

    return 0;
}