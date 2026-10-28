#pragma once
#include "DynamicModel.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include <memory>
#include <iostream>

class ModelCreator
{
public:
    static std::unique_ptr<DynamicModel> create(int choice)
    {
        switch (choice)
        {
        case 1: return createModel1_6();
        case 2: return createModel2_8();
        case 3: return createModel3_10();
        default: return nullptr;
        }
    }

private:
    static std::unique_ptr<DynamicModel> createModel1_6()
    {
        double a1, a2, a3, b;
        std::cout << "  a1 = "; std::cin >> a1;
        std::cout << "  a2 = "; std::cin >> a2;
        std::cout << "  a3 = "; std::cin >> a3;
        std::cout << "  b  = "; std::cin >> b;
        return std::make_unique<Model1_6>(a1, a2, a3, b);
    }

    static std::unique_ptr<DynamicModel> createModel2_8()
    {
        double a, b, c;
        std::cout << "  a = "; std::cin >> a;
        std::cout << "  b = "; std::cin >> b;
        std::cout << "  c = "; std::cin >> c;
        return std::make_unique<Model2_8>(a, b, c);
    }

    static std::unique_ptr<DynamicModel> createModel3_10()
    {
        double a, b, dt;
        std::cout << "  a  = "; std::cin >> a;
        std::cout << "  b  = "; std::cin >> b;
        std::cout << "  dt = "; std::cin >> dt;
        return std::make_unique<Model3_10>(a, b, dt);
    }
};