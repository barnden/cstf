from functools import partial

to_bytes = lambda b: partial(int.to_bytes, length=b, byteorder="little")

# HEADER
CSTF_magic = [0x8B7FC5, 3]
CSTF_flags = [0b00111100, 1]
CSTF_reserved = [0x0, 2]
CSTF_version = [0x1, 1]
CSTF_frame_rate = [0x40, 1]
CSTF_map_name = b"de_mirage\0"
CSTF_build_info = 10462

# GAME INFO
steamids = [
    76561198065277623,
    76561198065277623,
    76561198065277623,
    76561198065277623,
    76561198065277623,
    76561198039184405,
    76561198039184405,
    76561198039184405,
    76561198039184405,
    76561198039184405,
    76561198039184405,
]

tags = [b"Tag A\0", b"Tag B\0"]
teams = [b"Team A\0", b"Team B\0"]

usernames = [f"player {x}\0".encode("utf-8") for x in range(10)]
usernames += [b"dummy\0"]

# Round LUT
CSTF_round_LUT = [
    0b01_000000_0010101010000000_000000000000000000000000,
] * 24

# Insert a timeout after round 8
CSTF_round_LUT.insert(7, 0b10_0000000000011110000000_000000000000000000000000)

# Insert the size of the LUT in bytes at beginning
CSTF_round_LUT.insert(0, len(CSTF_round_LUT) * 6)
print(f"CSTF_round_LUT: {CSTF_round_LUT[0]}")
# Rounds
CSTF_round_event_LUT = [
    0b000001_0000_000000000000000010_00000000000000000000, # 0
    0b000000_0001_000000000000000001_00000000000000000011, # 3
    0b000000_0001_000000000000000110_00000000000000001101, # 13
    0b000001_0000_000000000000000001_00000000000000010111, # 23
    0b000001_0000_000000000000000001_00000000000000011010, # 26
    0b000001_0001_000000000000000001_00000000000000011101, # 29
    0b000001_0011_000000000000000001_00000000000000100000, # 32
    0b000001_0001_111111111111111111_00000000000000100011, # 35
    0b000000_0101_111111111111111111_00000000000000100110, # 38
]

# Insert the size of the LUT in bytes at beginning
CSTF_round_event_LUT.insert(0, len(CSTF_round_event_LUT) * 6)


# Assuming type 0 is PlayerMoveEvent and type 1 is PlayerDamageEvent
CSTF_round_event_data = [
    # DamageEvent: Player 0 damaged Player 1 (15HP, 7 Armor)
    0b00000000_00000000,
    0b00001111_00000111,
    0b00000000_00000001, # 0
    # MoveEvent: Player 2 set pitch to 27.99 deg and yaw to 275.98 deg
    #                         X: -1439.76, Y: -1322.04, Z: 11551.87
    # 0b0010_0010011111010_110001000100001,
    0b10100000_00000000, # 3
    0b1100010001000000,
    0b1100111011011001,
    0b00000010_00100111,
    0b1111100001010010, # X
    0b1100010010110011,
    0b0100000101001000, # Y
    0b1100010010100101,
    0b0111111101111011, # Z
    0b0100011000110100,
    # MoveEvent: Player 2 set pitch to 133.79 deg and yaw to 24.62 deg
    #                         X: -4759.16, Y: 1720.13, Z: 643.35
    0b11101111_00000000, # 13
    0b0001000110000001,
    0b0100011101110011,
    0b00000010_10111110,
    0b1011100101001000,
    0b1100010110010100,
    0b0000010000101001,
    0b0100010011010111,
    0b1101011001100110,
    0b0100010000100000,
    # DamageEvent: Player 0 damaged Player 1 (15HP, 7 Armor)
    0b00000000_00000000,
    0b00001111_00000111,
    0b00000000_00000001, # 23
    # DamageEvent: Player 2 damaged Player 8 (3HP, 1 Armor)
    0b00000000_00000000,
    0b00000011_00000001,
    0b00000010_00001000, # 26
    # DamageEvent: Player 4 damaged Player 9 (86HP, 0 Armor)
    0b00000000_00000000,
    0b01010110_00000000,
    0b00000100_00001001, # 29
    # DamageEvent: Player 10 damaged Player 10 (5HP, 1 Armor)
    0b00000000_00000000,
    0b00000101_00000001,
    0b00001010_00001010, # 32
    # DamageEvent: Player 1 damaged Player 0 (15HP, 7 Armor)
    0b00000000_00000000,
    0b00001111_00000111,
    0b00000001_00000000, # 35
    # MoveEvent: Player 2 set pitch to 160.25 deg and yaw to 195.14 deg
    #                         X: 478.91, Y: -3480.56, Z: 355.27
    0b00100111_00000000, # yaw / padding (38)
    0b1000101011000100, # yaw
    0b1110100100111110, # pitch
    0b00000010_11100011, # player / pitch (38)
    0b0111010001111011,
    0b0100001111101111, # X
    0b1000100011110110,
    0b1100010101011001, # Y
    0b1010001010001111,
    0b0100001110110001, # Z
]

# I put these here since I'm reusing the same exact events for every round
event_LUT_size = (len(CSTF_round_event_LUT) - 1) * 6
event_LUT_padding = (event_LUT_size + 4) & 3

round_count = 0
for i in range(len(CSTF_round_LUT[1:])):
    if (CSTF_round_LUT[i] >> 46) != 1:
        continue

    CSTF_round_LUT[i] |= (
        round_count
        * (
            4
            + len(CSTF_round_event_LUT[1:]) * 6
            + len(CSTF_round_event_data) * 2
            + event_LUT_padding
        )
        // 4
    )
    round_count += 1

CSTF_rounds = [
    to_bytes(4)(CSTF_round_event_LUT[0]),
    *map(to_bytes(6), CSTF_round_event_LUT[1:]),
    b"\x00" * event_LUT_padding,
    *map(to_bytes(2), CSTF_round_event_data),
]


CSTF_rounds = b"".join(CSTF_rounds)

# END DATA


def pad(file):
    x = file.tell()

    if x % 4 == 0:
        return

    file.write(b"\0" * ((x + 4) & 3))


with open("example.cstf", "wb") as file:
    file.write(
        b"".join(
            map(
                lambda x: to_bytes(x[1])(x[0]),
                [
                    CSTF_magic,
                    CSTF_flags,
                    CSTF_reserved,
                    CSTF_version,
                    CSTF_frame_rate,
                ],
            )
        )
    )
    file.write(CSTF_map_name)

    file.write(to_bytes(4)(CSTF_build_info))

    pad(file)

    file.write(to_bytes(4)(len(steamids)))
    file.write(b"".join(map(to_bytes(8), steamids)))

    for string in usernames + tags + teams:
        file.write(string)

    pad(file)

    file.write(to_bytes(4)(CSTF_round_LUT[0]))
    file.write(b"".join(map(to_bytes(6), CSTF_round_LUT[1:])))

    pad(file)

    for _ in range(24):
        file.write(CSTF_rounds)
