#include <iostream>
#include <charconv>
#include <array>
#include <shared_mutex>
#include <string>
#include <filesystem>
#include <map>
#include <optional>
#include <variant>
#include <execution>

#pragma warning(4267)

template <typename T>
std::string convert(T input) {
    if constexpr (std::is_same_v<T, const char*> ||
        std::is_same_v<T, std::string>) {
        return input;
    }
    else {
        return std::to_string(input);
    }
}

class ThreadSafeCounter {
public:
    ThreadSafeCounter() = default;

    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const {
        std::shared_lock lock(mutex_);
        return value_;
    }

    // Only one thread/writer can increment/write the counter's value.
    unsigned int increment() {
        std::unique_lock lock(mutex_);
        return ++value_;
    }

    // Only one thread/writer can reset/write the counter's value.
    void reset() {
        std::unique_lock lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    unsigned int value_ = 0;
};

int add_ten(int first, int second) { return first + second; }

struct ReturnType
{
    bool ret = false;
};

struct Out {
    std::string out1{ "" };
    std::string out2{ "" };
};

// c++17 更安全和直观
std::optional<Out> func(const std::string& in) {
    Out o;
    if (in.size() == 0)
        return std::nullopt;
    o.out1 = "hello";
    o.out2 = "world";
    return { o };
}

struct A {
    int a = 0;
};

struct B {
    std::string str = 0;
};

using Two = std::pair<double, double>;
using Roots = std::variant<Two, double, void*>;

Roots FindRoots(double a, double b, double c)
{
    auto d = b * b - 4 * a * c;

    if (d > 0.0)
    {
        auto p = sqrt(d);
        return std::make_pair((-b + p) / 2 * a, (-b - p) / 2 * a);
    }
    else if (d == 0.0)
        return (-1 * b) / (2 * a);
    return nullptr;
}

struct RootPrinterVisitor
{
    void operator()(const Two& roots) const
    {
        std::cout << "2 roots: " << roots.first << " " << roots.second << '\n';
    }
    void operator()(double root) const
    {
        std::cout << "1 root: " << root << '\n';
    }
    void operator()(void*) const
    {
        std::cout << "No real roots found.\n";
    }
};

struct Triangle {
    void Draw() const {
        std::cout << "△" << std::endl;
    }
};

struct Circle {
    void Draw() const {
        std::cout << "○" << std::endl;
    }
};

// to parallelize non-associate accumulative operation, you'd better choose
// transform_reduce instead of reduce; e.g., a + b * b != b + a * a
void print_sum_squared(long const num)
{
    std::cout.imbue(std::locale{ "en_US.UTF8" });
    std::cout << "num = " << num << '\n';

    // create an immutable vector filled with pattern: 1,2,3,4, 1,2,3,4 ...
    const std::vector<long> v{ [n = num * 4] {
        std::vector<long> v;
        v.reserve(n);
        std::generate_n(std::back_inserter(v), n,
            [i = 0]() mutable { return 1 + i++ % 4; });
        return v;
    }() };

    auto squared_sum = [](auto sum, auto val) { return sum + val * val; };

    auto sum1 = std::accumulate(v.cbegin(), v.cend(), 0L, squared_sum);
    std::cout << "accumulate(): " << sum1 << '\n';

    auto sum2 = std::reduce(std::execution::par, v.cbegin(), v.cend(), 0L, squared_sum);
    std::cout << "reduce(): " << sum2 << '\n';

    auto sum3 = std::transform_reduce(std::execution::par, v.cbegin(), v.cend(), 0L, std::plus{},
        [](auto val) { return val * val; });
    std::cout << "transform_reduce(): " << sum3 << "\n\n";
}

int main()
{
    //auto x3{ 1, 3 }; //error: not a single element
    auto x4 = { 3 };
    std::cout << typeid(x4).name() << std::endl;

    auto x5{ 3 };
    std::cout << typeid(x5).name() << std::endl;

    std::array<char, 3> str{ "42" };
    int result;
    std::from_chars(str.data(), str.data() + str.size(), result);
    std::cout << result << std::endl;

    // p是填充到str以后的最后一个迭代器
    if (auto [p, ec] = std::to_chars(str.data(), str.data() + str.size(), 425);
        ec == std::errc()) {
        /*if (p == str.end()) {
            std::cout << "hello world\n";
        }*/
        std::cout << std::string_view(str.data(), p - str.data());
    }

    std::pair p3{ 3.14, "pi" };

    auto str1 = convert<int>(10);
    std::cout << str1 << std::endl;

    /*ThreadSafeCounter thread_safe_counter;
    for (int i = 0; i < 10; ++i)
    {
        std::thread([&thread_safe_counter]() {
            thread_safe_counter.increment();
            std::cout << thread_safe_counter.get() << std::endl;
            }).detach();
    }
    std::cout << thread_safe_counter.get() << std::endl;*/

    namespace fs = std::filesystem;
    fs::path pathToShow("c:\\windows");
    std::cout << "exists() = " << fs::exists(pathToShow) << "\n"
        << "root_name() = " << pathToShow.root_name() << "\n"
        << "root_path() = " << pathToShow.root_path() << "\n";

    std::map<std::string, std::string> m;
    // emplace的原地构造需要使用std::piecewise_construct，因为是直接插入std::pair<key, value>
    m.emplace(std::piecewise_construct,
        std::forward_as_tuple("c"),
        std::forward_as_tuple(10, 'c'));

    // try_emplace可以直接原地构造，因为参数列表中key和value是分开的
    m.try_emplace("c", 10, 'c');

    auto add_ten_lambda = [](auto first, auto second) { return first + second; };

    std::cout << std::apply(add_ten, std::pair(1, 2)) << '\n';
    //std::cout << add(std::pair(1, 2)) << "\n"; // error
    std::cout << std::apply(add_ten_lambda, std::tuple(2.2f, 3.0f)) << '\n';

    if (auto ret = func("hi"); ret.has_value())
    {
        std::cout << ret->out1 << std::endl;
        std::cout << ret->out2 << std::endl;
    }

    if (auto ret = func(""); !ret.has_value())
    {
        std::cout << "ret don't has value" << std::endl;
    }

    using AB = std::variant<A, B>;
    AB ab;
    ab = A{ 1 };
    std::cout << std::get<A>(ab).a << std::endl;
    ab = B{ "str" };
    std::cout << std::get<B>(ab).str << std::endl;

    std::visit(RootPrinterVisitor(), FindRoots(1, -2, 1)); //(x-1)*(x-1)=0
    std::visit(RootPrinterVisitor(), FindRoots(1, -3, 2)); //(x-2)*(x-1)=0
    std::visit(RootPrinterVisitor(), FindRoots(1, 0, 2));  //x*x + 2 = 0

    using Draw = std::variant<Triangle, Circle>;
    Draw draw;
    std::vector<Draw> draw_list{ Triangle{}, Circle{}, Triangle{} };
    auto DrawVisitor = [](const auto& t) { t.Draw(); };
    for (const auto& item : draw_list) {
        std::visit(DrawVisitor, item);
    }

    std::vector<int> intVec{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };                        // 1
    std::for_each_n(std::execution::par,                       // 2
        intVec.begin(), 5, [](int& arg) { arg *= arg; });

    std::cout << "for_each_n: ";
    for (auto v : intVec) std::cout << v << " ";
    std::cout << std::endl;

    // exclusive_scan and inclusive_scan
    std::vector<int> resVec{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::exclusive_scan(std::execution::par,                   // 3
        resVec.begin(), resVec.end(), resVec.begin(), 1,
        [](int fir, int sec) { return fir * sec; });

    std::cout << "exclusive_scan: ";
    for (auto v : resVec) std::cout << v << " ";
    std::cout << std::endl;

    std::vector<int> resVec2{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    std::inclusive_scan(std::execution::par,                   // 5 
        resVec2.begin(), resVec2.end(), resVec2.begin(),
        [](int fir, int sec) { return fir * sec; }, 1);

    std::cout << "inclusive_scan: ";
    for (auto v : resVec2) std::cout << v << " ";
    std::cout << std::endl;

    // transform_exclusive_scan and transform_inclusive_scan
    std::vector<int> resVec3{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> resVec4(resVec3.size());
    std::transform_exclusive_scan(std::execution::par,         // 6
        resVec3.begin(), resVec3.end(),
        resVec4.begin(), 0,
        [](int fir, int sec) { return fir + sec; },
        [](int arg) { return arg *= arg; });

    /* transform_exclusive_scan(6) 执行的操作有些复杂, 他首先将 lambda 函数 function[](int arg) { return arg *= arg; }
       应用到列表 resVec3 的每一个元素上,接着再对中间结果(由上一步 lambda 函数产生的临时列表)应用二元运算 [](int fir, int sec){ return fir + sec; }
       (以 0 作为初始元素),最终结果存储于 resVec4.begin() 开始的内存处 */

    std::cout << "transform_exclusive_scan: ";
    for (auto v : resVec4) std::cout << v << " ";
    std::cout << std::endl;

    std::vector<std::string> strVec{ "Only", "for", "testing", "purpose" };             // 7
    std::vector<int> resVec5(strVec.size());

    std::transform_inclusive_scan(std::execution::par,         // 8
        strVec.begin(), strVec.end(),
        resVec5.begin(),
        [](auto fir, auto sec) { return fir + sec; },
        [](auto s) { return s.length(); });

    std::cout << "transform_inclusive_scan: ";
    for (auto v : resVec5) std::cout << v << " ";
    std::cout << std::endl;

    // reduce and transform_reduce
    std::vector<std::string> strVec2{ "Only", "for", "testing", "purpose" };

    std::string res = std::reduce(std::execution::par,         // 9
        strVec2.begin() + 1, strVec2.end(), strVec2[0],
        [](auto fir, auto sec) { return fir + ":" + sec; });

    std::cout << "reduce: " << res << std::endl;

    // 11
    print_sum_squared(1);
    print_sum_squared(1'000);
    print_sum_squared(1'000'000);

    return 0;
}