#pragma once
#include "DynamicModel.h"
#include "Model1_3.h"
#include "Model2_10.h"
#include "Model3_2.h"
#include <memory>
#include <iostream>

class ModelCreator
{
public:
    static std::unique_ptr<DynamicModel> create(int choice)
    {
        switch (choice)
        {
        case 1:
            return createModel1_3();
        case 2:
            return createModel2_10();
        case 3:
            return createModel3_2();
        default:
            std::cerr << "Unknown model number: " << choice << "\n";
            return nullptr;
        }
    }

private:
    // 1.3
    static std::unique_ptr<DynamicModel> createModel1_3()
    {
        double a1;
        double a2;
        double b;
        std::cout << "  a1 = "; std::cin >> a1;
        std::cout << "  a2 = "; std::cin >> a2;
        std::cout << "  b  = "; std::cin >> b;
        return std::make_unique<Model1_3>(a1, a2, b);
    }

    // 2.10
    static std::unique_ptr<DynamicModel> createModel2_10()
    {
        double a;
        double b;
        std::cout << "  a = "; std::cin >> a;
        std::cout << "  b = "; std::cin >> b;
        return std::make_unique<Model2_10>(a, b);
    }

    // 3.2
    static std::unique_ptr<DynamicModel> createModel3_2()
    {
        double b;
        double dt;
        std::cout << "  b  = "; std::cin >> b;
        std::cout << "  dt = "; std::cin >> dt;
        return std::make_unique<Model3_2>(b, dt);
    }
};