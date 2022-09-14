#pragma once

#include <modules/joystick/joystickmodule.tcc>

#include <switch.h>
#include <vector>

namespace love
{
    template<>
    class JoystickModule<Console::HAC> : public JoystickModule<Console::ALL>
    {
      public:
        JoystickModule();

        virtual ~JoystickModule();

        std::vector<HidNpadIdType> AcquireCurrentJoystickIds();

        ::Joystick* AddJoystick(int index);
    };
} // namespace love
