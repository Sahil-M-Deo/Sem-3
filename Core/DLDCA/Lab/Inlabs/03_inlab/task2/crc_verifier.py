import argparse
import re
import subprocess


def crc(bits, width, poly, init):
    mask = (1 << width) - 1
    value = init & mask

    for bit in bits:
        feedback = ((value >> (width - 1)) & 1) ^ bit
        value = (value << 1) & mask
        if feedback:
            value ^= poly

    return value


def parse_bits(message):
    message = message.replace(" ", "").replace("_", "")

    value = int(message, 16)
    num_bits = len(message) * 4

    return [
        (value >> i) & 1
        for i in range(num_bits - 1, -1, -1)
    ]


def interactive():
    message = input("Message (hex): ")
    width = int(input("CRC width: "))
    poly = int(input("Polynomial (hex): "), 16)
    init = int(input("Initial CRC (hex): "), 16)

    bits = parse_bits(message)
    result = crc(bits, width, poly, init)

    digits = max(1, (width + 3) // 4)

    print(f"Expected CRC: {result:0{digits}X}")


def autograde():
    print("=== RUNNING CRC_VERIFIER.PY ===\n")
    print("Running task2.vvp...\n")

    try:
        output = subprocess.check_output(
            ["vvp", "task2.vvp"],
            text=True,
            stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as e:
        print("FAIL: task2.vvp exited with an error")
        print(e.output)
        return

    pattern = re.compile(
        r"CRC_RESULT\|"
        r"TEST=(?P<test>\d+)\|"
        r"MSG=(?P<msg>[0-9A-Fa-f]+)\|"
        r"WIDTH=(?P<width>\d+)\|"
        r"POLY=(?P<poly>[0-9A-Fa-f]+)\|"
        r"INIT=(?P<init>[0-9A-Fa-f]+)\|"
        r"CRC=(?P<crc>[0-9A-Fa-f]+)"
    )

    results = pattern.findall(output)

    if not results:
        print("FAIL: no CRC_RESULT lines found")
        return

    passed = 0

    for test, message, width, poly, init, actual in results:
        width = int(width)
        poly = int(poly, 16)
        init = int(init, 16)
        actual = int(actual, 16)

        bits = parse_bits(message)
        expected = crc(bits, width, poly, init)

        if actual == expected:
            print(
                f"TEST {test}: PASS "
                f"(MSG={message}, WIDTH={width}, "
                f"POLY={poly:X}, INIT={init:X}, CRC={actual:X})"
            )
            passed += 1
        else:
            print(
                f"TEST {test}: FAIL "
                f"(expected {expected:X}, got {actual:X})"
            )

    print(f"\n{passed}/{len(results)} CRC tests passed.")


parser = argparse.ArgumentParser()
parser.add_argument(
    "--autograde",
    action="store_true",
    help="Run task2.vvp and automatically verify CRC results"
)

args = parser.parse_args()

if args.autograde:
    autograde()
else:
    interactive()