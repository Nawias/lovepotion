#include <algorithm>

#include <utilities/sensor/gyroscope.hpp>

using namespace love;

Gyroscope::Gyroscope(HidNpadIdType playerId, HidNpadStyleTag style) :
    playerId(playerId),
    style(style)
{
    this->handleCount = 1;

    if (style == HidNpadStyleTag_NpadJoyDual)
        this->handleCount = 2;

    this->handles = std::make_unique<HidSixAxisSensorHandle[]>(this->handleCount);
    hidGetSixAxisSensorHandles(this->handles.get(), this->handleCount, playerId, style);
}

Gyroscope& Gyroscope::operator=(Gyroscope&& other)
{
    this->handles  = std::move(other.handles);
    this->playerId = other.playerId;
    this->style    = other.style;

    return *this;
}

Gyroscope::~Gyroscope()
{
    this->SetEnabled(false);
}

void Gyroscope::SetEnabled(bool enabled)
{
    if (enabled)
    {
        Result results[this->handleCount];
        for (int index = 0; index < this->handleCount; index++)
            results[index] = hidStartSixAxisSensor(this->handles[index]);
    }
    else
    {
        if (!this->handles)
            return;

        for (int index = 0; index < this->handleCount; index++)
            hidStopSixAxisSensor(this->handles[index]);
    }

    SensorBase::SetEnabled(enabled);
}

std::vector<float> Gyroscope::GetData()
{
    std::vector<float> results {};

    // clang-format off
    for (size_t index = 0; index < this->handleCount; index++)
    {
        HidSixAxisSensorState state {};
        hidGetSixAxisSensorStates(this->handles[index], &state, 1);

        results.insert(results.end(), { state.angular_velocity.x, state.angular_velocity.y, state.angular_velocity.z });
    }

    return results;
}
