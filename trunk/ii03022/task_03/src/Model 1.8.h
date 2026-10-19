#pragma once
#include "Model.h"   

class GeneralizedAutoregressiveLinearModel : public Model{
    private:
        double a1, a2, b1, b2;     
        double y = 0;              
        double y_previous = 0;     
        double u_previous = 0;    
    public:
        GeneralizedAutoregressiveLinearModel(double a1, double a2, double b1, double b2)
        : a1(a1), a2(a2), b1(b1), b2(b2)
        {
        }

        double nextStep(double u) override{
            double y_next = (a1 * y) + (a2 * y_previous) + (b1 * u) + (b2 * u_previous);
            y_previous = y;
            y = y_next;
            u_previous = u;
            return y_next;
        }

        void reset() override{
            y = y_previous = u_previous = 0;
        };
};
