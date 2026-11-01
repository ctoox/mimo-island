#pragma once

class SpringValue
{
public:
    explicit SpringValue(double value = 0.0);
    
    void setTarget(double target);
    bool tick(double dt = 0.016, double spring = 160.0, double damp = 10.0);
    
    double value() const { return m_current; }
    double target() const { return m_target; }
    
private:
    double m_current;
    double m_target;
    double m_velocity;
};
