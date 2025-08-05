#pragma once

#include "CSTF/Format/Events/BaseEvent.h"
#include "CSTF/Serialize/Serializable.h"
#include "CSTF/Utility/Types.h"

#include <format>

namespace cstf {

using serialize::Serializable;

struct KeepAliveEvent : Event<KeepAliveEvent> {
#pragma pack(1)
    struct Data : public Serializable<Data> {
        u16 padding;
    } data {};
#pragma pack()

    KeepAliveEvent() = default;

    [[nodiscard]] constexpr auto to_string() const noexcept -> std::string
    {
        return std::format("KeepAliveEvent(padding: {})",
                           data.padding);
    }
};

};