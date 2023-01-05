#include <iostream>
#include <vector>

template <class T>
class Compare
{
public:
    static bool IsEqual(const T& lh, const T& rh) {
        std::cout << "in the general class..." << std::endl;
        return lh == rh;
    }
};

template <>
class Compare<float>
{
public:
    static bool IsEqual(const float& lh, const float& rh) {
        std::cout << "in the float special class..." << std::endl;
        return std::abs(lh - rh) < 10e-3;
    }
};

template <>
class Compare<double>
{
public:
    static bool IsEqual(const double& lh, const double& rh) {
        std::cout << "in the double special class..." << std::endl;
        return std::abs(lh - rh) < 10e-6;
    }
};

template <class T>
class Compare<T*>
{
public:
    static bool IsEqual(const T* lh, const T* rh) {
        return Compare<T>::IsEqual(*lh, *rh);
    }
};

template <class T>
class Compare<std::vector<T>>
{
public:
    static bool IsEqual(const std::vector<T>& lh, const std::vector<T>& rh) {
        if (lh.size() != rh.size()) return false;
        for (auto i = 0; i < lh.size(); ++i)
        {
            if (!Compare<T>::IsEqual(lh[i], rh[i]))
            {
                return false;
            }
        }
        return true;
    }
};

int main()
{
    std::cout << Compare<int>::IsEqual(3, 4) << std::endl;
    std::cout << Compare<int>::IsEqual(3, 3) << std::endl;

    std::cout << Compare<float>::IsEqual(3.14f, 4.14f) << std::endl;
    std::cout << Compare<float>::IsEqual(3.14f, 3.14f) << std::endl;

    double d1 = 3.14159;
    double d2 = 4.14159;
    std::cout << Compare<double>::IsEqual(d1, d2) << std::endl;
    std::cout << Compare<double>::IsEqual(d1, d1) << std::endl;

    double* p1 = &d1;
    double* p2 = &d2;
    std::cout << Compare<double*>::IsEqual(p1, p2) << std::endl;
    std::cout << Compare<double*>::IsEqual(p1, p1) << std::endl;

    std::vector<double> d1_list = { 1.111111, 2.222222, 3.3333333 };
    std::vector<double> d2_list = { 1.111111, 2.222222, 4.4444444 };

    std::cout << Compare<std::vector<double>>::IsEqual(d1_list, d2_list) << std::endl;
    std::cout << Compare<std::vector<double>>::IsEqual(d1_list, d1_list) << std::endl;

    return 0;
}