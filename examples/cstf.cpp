/*
 * Generates a random EventLUT and corresponding event data.
 */

#include <CSTF.h>

#include <CSTF/Format/Tables/RoundLUT.h>
#include <chrono>
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

auto create_event_lut(std::mt19937& mt, size_t num_events)
{
    EventLUT events;

    auto dist_events = std::uniform_int_distribution<size_t>(0, EventTypes::size - 1uz);
    auto dist_duration = std::uniform_int_distribution<u32>(0u, 15u);

    auto frame = 0u;

    for (auto _ = 0uz; _ < num_events; _++) {
        auto event_type = dist_events(mt);

        for_sequence<EventTypes::size>(
            [&](auto I) {
                using EventType = EventTypes::get<I>;

                if (event_type != I)
                    return;

                events.add(create_event<EventType>::random(mt), frame);
            });

        frame += dist_duration(mt);
    }

    return events;
}

auto main() -> int
{
    using namespace std::chrono;
    constexpr auto num_rounds = 4uz;
    constexpr auto num_events_per_round = 4uz;

    // CS2 build_info field is days since 24 Oct. 1994
    // In practice, use the actual build_info field from the demo/GSI.
    constexpr auto epoch = sys_days { 1994y / 10 / 24 };

    auto mt = std::mt19937 { 0xC57F };

    // Header
    auto map_name = "de_mirage";
    auto build_info = static_cast<u32>((sys_days { floor<days>(system_clock::now()) } - epoch).count());
    u8 flags = Header::Flags::KILLS | Header::Flags::DAMAGE | Header::Flags::POSITIONS | Header::Flags::UTILITY;

    CSTF cstf { map_name, build_info, flags, 64 };

    // GameData
    cstf.add_player({ 76561198065277623 }, "brandon");
    cstf.add_player({ 76561198039184405 }, "BirdehBox");

    cstf.set_team(0, "Tag A", "Team A");
    cstf.set_team(1, "Tag B", "Team B");

    // Rounds
    auto frames = 0;
    auto pauses = 0;
    auto rounds = 0;

    static auto dist_pause = std::bernoulli_distribution(.125);

    auto random_round_time = [&mt, &frames]() {
        // Generate a random round time, in ticks, and add it to the frame counter
        // Returns the number of ticks added to counter.

        static auto dist_round_time = std::uniform_int_distribution<int>(10, 105);
        static auto dist_bomb_plant = std::bernoulli_distribution(.33);
        static auto dist_bomb_diffused = std::bernoulli_distribution(0.5);
        static auto dist_bomb_duration = std::uniform_int_distribution<int>(5, 45);

        auto seconds = 0;

        seconds += 0; // Freeze-time
        seconds += dist_round_time(mt); // Round duration (0:10 - 1:45)

        if (dist_bomb_plant(mt)) {
            if (dist_bomb_diffused(mt)) {
                seconds += 45; // Bomb exploded.
            } else {
                seconds += dist_bomb_duration(mt); // Time until bomb defused
            }
        }

        seconds += 7; // Round-end time

        auto ticks = seconds * 64;

        frames += ticks;

        return ticks;
    };

    while (rounds != num_rounds) {
        // Upto 4 30-second pauses per game
        if ((pauses < 4 && dist_pause(mt)) || (pauses == 0 && rounds > 0)) {
            cstf.add_round(RoundLUTEntry::PAUSE, 30 * 64);
            pauses++;

            continue;
        }

        cstf.add_round(
            RoundLUTEntry::ROUND,
            random_round_time(),
            create_event_lut(mt, num_events_per_round));

        rounds++;
    }

    std::println("{}", cstf);
}
