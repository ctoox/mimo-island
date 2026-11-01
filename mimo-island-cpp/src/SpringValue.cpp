#include "SpringValue.h"
#include <cmath>
#include <algorithm>

SpringValue::SpringValue(double value)
    : m_current(value)
    , m_target(value)
    , m_velocity(0.0)
{
}

void SpringValue::setTarget(double target)
{
    m_target = target;
}

bool SpringValue::tick(double dt, double spring, double damp)
{
    double distance = m_target - m_current;
    m_velocity += distance * spring * dt;
    m_velocity *= std::max(0.0, 1.0 - damp * dt);
    m_current += m_velocity * dt;
    
    return std::abs(distance) > 0.0005 || std::abs(m_velocity) > 0.005;
}
