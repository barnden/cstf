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
#include "CSTF/Serialize/Serializers/CSTFSerializer.h"
#include "CSTF/Serialize/Serializers/BaseEventSerializer.h"
#include "CSTF/Serialize/Serializers/GameDataSerializer.h"
#include "CSTF/Serialize/Serializers/HeaderSerializer.h"
#include "CSTF/Serialize/Serializers/TableSerializer.h"

// clang-fornat on