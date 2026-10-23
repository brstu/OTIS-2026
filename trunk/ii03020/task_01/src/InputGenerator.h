#pragma once

class InputGenerator {
    int    type;
    int    t;
    double constant_u;
public:
    InputGenerator(int type = 0, double constant_u = 0.0);
    double Next();
    void   Reset();
};