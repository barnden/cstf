/*
 * Generates a random EventLUT and corresponding event data.
 */

#include <CSTF.h>

#include <print>
#include <random>

using namespace cstf;

template <typename T>
struct create_event {
    static auto random(std::mt19937&) -> T
    {
        std::println("Unimplemented create_event<{}>::random().", demangle(typeid(T)));
        ASSERT_NOT_REACHED;
    }
};

template <>
struct create_event<PlayerDamageEvent> {
    static auto random(std::mt19937& mt)
    {
        static auto dist_player = std::uniform_int_distribution<u8>(0, 10);
        static auto dist_hp = std::uniform_int_distribution<u8>(0, 100);
        auto event = PlayerDamageEvent {};

        event.data = {
            .padding = 0,
            .source = 0,
            .armor = dist_hp(mt),
            .health = dist_hp(mt),
            .victim = dist_player(mt),
            .attacker = dist_player(mt),
        };

        return event;
    }
};

template <>
struct create_event<PlayerMoveEvent> {
    static auto random(std::mt19937& mt)
    {
        static auto dist_player = std::uniform_int_distribution<u8>(0, 10);
        static auto dist_xy_position = std::uniform_real_distribution<float>(-5000, 5000);
        static auto dist_z_position = std::uniform_real_distribution<float>(-500, 500);
        static auto dist_angle = std::uniform_int_distribution<u32>(0, (1u << 24) - 1u);

        auto event = PlayerMoveEvent {};

        event.data = {
            .padding = 0,
            .yaw = dist_angle(mt),
            .pitch = dist_angle(mt),
            .player = dist_player(mt),
            .position = {
                .x = dist_xy_position(mt),
                .y = dist_xy_position(mt),
                .z = dist_z_position(mt),
            }
        };

        return event;
    }
};

auto main() -> int
{
    constexpr auto num_events = 16;

    auto mt = std::mt19937 {};
    EventLUT events;

    auto dist_events = std::uniform_int_distribution<size_t>(0, EventTypes::size - 1uz);
    auto dist_duration = std::uniform_int_distribution<u32>(0u, 15u);

    auto frame = 0u;

    for (auto _ = 0; _ < num_events; _++) {
        auto event_type = dist_events(mt);

        for_sequence<EventTypes::size>([&](auto I) {
            using EventType = EventTypes::get<I>;

            if (event_type != I)
                return;

            events.add(create_event<EventType>::random(mt), frame);
        });

        frame += dist_duration(mt);
    }

    std::println("{}", events);
}