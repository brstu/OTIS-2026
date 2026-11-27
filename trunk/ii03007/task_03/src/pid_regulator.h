#pragma once

/// @file pid_regulator.h
/// @brief Позиционный ПИД-закон: u = Kp*e + Ki*sum(e*dt) + Kd*(e - ePrev)/dt.

class PidRegulator{
    private:
        double kp_;
        double ki_;
        double kd_;
        double dt_;
        double uMin_;
        double uMax_;
        double integral_ = 0.0;
        double ePrev_    = 0.0;
        bool   hasPrev_  = false;
    public:
        PidRegulator(double kp, double ki, double kd, double dt, double uMin, double uMax)
        : kp_(kp), ki_(ki), kd_(kd), dt_(dt), uMin_(uMin), uMax_(uMax)
        {
        }

        double compute(double w, double y){
            const double e = w - y;
            integral_ += e * dt_;
            const double derivative = hasPrev_ ? (e - ePrev_) / dt_ : 0.0;
            double u = kp_ * e + ki_ * integral_ + kd_ * derivative;
            if (u > uMax_) u = uMax_;
            else if (u < uMin_) u = uMin_;
            ePrev_ = e;
            hasPrev_ = true;
            return u;
        }

        void reset(){
            integral_ = 0.0;
            ePrev_ = 0.0;
            hasPrev_ = false;
        }
};