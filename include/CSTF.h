#pragma once

// clang-format off

#include "CSTF/Utility/Types.h"
#include "CSTF/Utility/Stream.h"

#include "CSTF/Format/GameData.h"
#include "CSTF/Format/Header.h"

#include "CSTF/Format/Events/BaseEvent.h"

// Major assumption is that Events.h pulls in all concrete Event types
#include "CSTF/Format/Events/Events.h"

#include "CSTF/Format/Tables/LookupTable.h"
#include "CSTF/Format/Tables/RoundLUT.h"
#include "CSTF/Format/Tables/EventLUT.h"

#include "CSTF/Format/CSTF.h"

#include "CSTF/Serialize/Serializer.h"
#include "CSTF/Serialize/Serializable.h"

// Serializer specializations
#include "CSTF/Serialize/Serializers/CSTF.h"
#include "CSTF/Serialize/Serializers/Event.h"
#include "CSTF/Serialize/Serializers/GameData.h"
#include "CSTF/Serialize/Serializers/Header.h"
#include "CSTF/Serialize/Serializers/Tables.h"

// clang-fornat on