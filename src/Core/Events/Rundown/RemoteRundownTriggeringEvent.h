#pragma once

#include "../../Shared.h"

class CORE_EXPORT RemoteRundownTriggeringEvent
{
    public:
        explicit RemoteRundownTriggeringEvent(bool enabled);

        bool getEnabled() const;

    private:
        bool enabled;
};
