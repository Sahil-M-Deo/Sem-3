#!/usr/bin/env python3

import argparse
import sys
import re
from pathlib import Path

# ==============================================================================
# TOP-LEVEL COMMAND CONFIGURATION
# Syntax:
#   - "MAP <MACHINE> <PORT>"       (e.g., "MAP A 0")
#   - "SEND <SRC> <DST> <PAYLOAD>" (e.g., "SEND A B 0xAABBCCDDEEFF0011")
# ==============================================================================
COMMANDS = [
    "MAP A 0",
    "MAP B 1",
    "MAP C 2",
    "SEND A B 0xAABBCCDDEEFF0011",
    "SEND B A 0b1010101010101010101010101010101010101010101010101010101010101010",
    "MAP A 3",  # Overrides machine A to port 3
    "MAP D 1",  # Overrides machine A to port 3
    "SEND A C 0x1234567890ABCDEF",
    "SEND C D 0xFEDCBA9876543210",
]

# ------------------------------------------------------------------------------
# 8B/10B Encoding Table & CRC Utilities
# ------------------------------------------------------------------------------
START = 0b11000
END   = 0b11001

ENC = {
    0x0: 0b11110, 0x1: 0b01001, 0x2: 0b10100, 0x3: 0b10101,
    0x4: 0b01010, 0x5: 0b01011, 0x6: 0b01110, 0x7: 0b01111,
    0x8: 0b10010, 0x9: 0b10011, 0xA: 0b10110, 0xB: 0b10111,
    0xC: 0b11010, 0xD: 0b11011, 0xE: 0b11100, 0xF: 0b11101,
}

# Reverse lookup table for 5B -> 4B decoding
DEC_5B = {
    0b11110: 0x0, 0b01001: 0x1, 0b10100: 0x2, 0b10101: 0x3,
    0b01010: 0x4, 0b01011: 0x5, 0b01110: 0x6, 0b01111: 0x7,
    0b10010: 0x8, 0b10011: 0x9, 0b10110: 0xA, 0b10111: 0xB,
    0b11010: 0xC, 0b11011: 0xD, 0b11100: 0xE, 0b11101: 0xF,
}

START_5B = 0b11000
END_5B   = 0b11001

MACHINE_ADDR = {chr(ord('A') + i): (i + 1) for i in range(26)}

def crc8(data):
    crc = 0xFF
    for byte in data:
        for bit in range(7, -1, -1):
            feedback = ((crc >> 7) & 1) ^ ((byte >> bit) & 1)
            crc = (crc << 1) & 0xFF
            if feedback:
                crc ^= 0xD5
    return crc

def make_frame_bytes(dst, src, payload):
    fields = [
        dst, src,
        (payload >> 56) & 0xFF, (payload >> 48) & 0xFF,
        (payload >> 40) & 0xFF, (payload >> 32) & 0xFF,
        (payload >> 24) & 0xFF, (payload >> 16) & 0xFF,
        (payload >> 8) & 0xFF,  payload & 0xFF,
    ]
    checksum = crc8(fields)
    fields.append(checksum)

    bits = f"{START:05b}"
    for byte in fields:
        bits += f"{ENC[byte >> 4]:05b}"
        bits += f"{ENC[byte & 0xF]:05b}"
    bits += f"{END:05b}"

    assert len(bits) == 120
    return [int(bits[i:i + 8], 2) for i in range(0, 120, 8)]

# ------------------------------------------------------------------------------
# Command Parsing
# ------------------------------------------------------------------------------
def load_commands_from_file(file_path):
    """Reads commands from a file, ignoring comments and blank lines."""
    path = Path(file_path)
    if not path.exists():
        raise FileNotFoundError(f"Input file '{file_path}' does not exist.")

    commands = []
    for line in path.read_text().splitlines():
        # Remove inline comments and whitespace
        clean_line = line.split('#')[0].strip()
        # Ignore empty lines or lines setting a python variable (e.g., COMMANDS = [...])
        if not clean_line or clean_line.startswith("COMMANDS"):
            continue
        # Strip trailing commas/quotes if copied directly from a list format
        clean_line = clean_line.strip('",\'')
        commands.append(clean_line)

    return commands

def parse_and_validate_commands(cmd_list):
    if len(cmd_list) > 10:
        raise ValueError(f"Too many commands: {len(cmd_list)} provided, max allowed is 8.")

    active_machine_to_port = {}
    parsed_transmissions = []

    for idx, raw_cmd in enumerate(cmd_list, start=1):
        tokens = raw_cmd.strip().split()
        if not tokens:
            continue
        
        op = tokens[0].upper()

        if op == "MAP":
            if len(tokens) != 3:
                raise ValueError(f"Command {idx} ('{raw_cmd}'): MAP requires machine and port.")
            
            m, p_str = tokens[1].upper(), tokens[2]
            if len(m) != 1 or not ('A' <= m <= 'Z'):
                raise ValueError(f"Command {idx}: Machine '{m}' must be A..Z.")
            if not p_str.isdigit() or int(p_str) not in range(4):
                raise ValueError(f"Command {idx}: Port '{p_str}' must be 0, 1, 2, or 3.")

            port = int(p_str)
            for existing_m, existing_p in list(active_machine_to_port.items()):
                if existing_p == port or existing_m == m:
                    del active_machine_to_port[existing_m]
            
            active_machine_to_port[m] = port

        elif op == "SEND":
            if len(tokens) != 4:
                raise ValueError(f"Command {idx} ('{raw_cmd}'): SEND requires M1 M2 PAYLOAD.")
            
            m1, m2, payload_str = tokens[1].upper(), tokens[2].upper(), tokens[3]

            if m1 not in active_machine_to_port or m2 not in active_machine_to_port:
                raise ValueError(f"Command {idx}: Unmapped machine referenced.")
            if m1 == m2:
                raise ValueError(f"Command {idx}: Sender and Receiver must be distinct.")

            ingress_port = active_machine_to_port[m1]
            target_port = active_machine_to_port[m2]
            
            if ingress_port == target_port:
                raise ValueError(f"Command {idx}: Machines mapped to same port {ingress_port}.")

            if payload_str.lower().startswith("0x"):
                payload = int(payload_str, 16)
            elif payload_str.lower().startswith("0b"):
                payload = int(payload_str, 2)
            else:
                raise ValueError(f"Command {idx}: Payload must start with '0x' or '0b'.")

            parsed_transmissions.append({
                "cmd_id": idx,
                "raw": raw_cmd,
                "ingress": ingress_port,
                "src_addr": MACHINE_ADDR[m1],
                "dst_addr": MACHINE_ADDR[m2],
                "m1": m1,
                "m2": m2,
                "payload": payload
            })
        else:
            raise ValueError(f"Command {idx}: Unknown operation '{op}'.")

    return parsed_transmissions

# ------------------------------------------------------------------------------
# Golden Reference Model
# ------------------------------------------------------------------------------
def switch_simulation(transmissions, num_ports=4):
    table = {}
    test_cases_expected = []

    for tc_idx, tx in enumerate(transmissions, start=1):
        ingress = tx["ingress"]
        src = tx["src_addr"]
        dst = tx["dst_addr"]
        payload = tx["payload"]

        table[src] = ingress

        if dst in table:
            target = table[dst]
            out_ports = [target] if target != ingress else []
        else:
            out_ports = [p for p in range(num_ports) if p != ingress]

        expected_bytes = make_frame_bytes(dst, src, payload)
        
        test_cases_expected.append({
            "tc_id": tc_idx,
            "raw": tx["raw"],
            "ingress": ingress,
            "expected_ports": sorted(out_ports),
            "expected_bytes": expected_bytes,
            "m1": tx["m1"],
            "m2": tx["m2"]
        })

    return test_cases_expected

# ------------------------------------------------------------------------------
# Handlers
# ------------------------------------------------------------------------------
def generate_verilog_code(transmissions):
    lines = []
    for tx in transmissions:
        lines.append("        $display(\"=== TC_START ===\");")
        lines.append(f"        make_frame(8'h{tx['dst_addr']:02X}, 8'h{tx['src_addr']:02X}, 64'h{tx['payload']:016X});")
        lines.append(f"        send_current_frame({tx['ingress']});")
        lines.append("        repeat (40) @(posedge clk);")
    return "\n".join(lines)

def replace_integrated_section(tb_text, generated_code):
    begin = "// BEGIN INTEGRATED TESTS"
    end = "// END INTEGRATED TESTS"
    before = tb_text.split(begin, 1)[0]
    after = tb_text.split(end, 1)[1]
    return f"{before}{begin}\n{generated_code}\n        {end}{after}"    

def decode_and_expand_frame(frame_bytes):
    if len(frame_bytes) != 15:
        return f"    [DECODE ERROR] Incomplete frame length ({len(frame_bytes)}/15 bytes)"

    bit_str = "".join(f"{b:08b}" for b in frame_bytes)
    symbols_5b = [int(bit_str[i:i+5], 2) for i in range(0, 120, 5)]

    # Extract 5B payload symbols (excluding START and END)
    payload_symbols = symbols_5b[1:23]

    decoded_nibbles = [f"{DEC_5B.get(sym, '?'):X}" for sym in payload_symbols]
    nibble_str = "".join(decoded_nibbles)

    dst_hex, src_hex = nibble_str[0:2], nibble_str[2:4]
    payload_hex, crc_hex = nibble_str[4:20], nibble_str[20:22]

    expansion = [
        f"    ├─ 5B Symbols    : {' '.join(f'{s:05b}' for s in symbols_5b)}",
        f"    └─ Decoded Frame : DST=0x{dst_hex} | SRC=0x{src_hex} | PAYLOAD=0x{payload_hex} | CRC=0x{crc_hex}"
    ]
    return "\n".join(expansion)


def handle_setup(args, commands):
    print("=== [SETUP AUTOGRADER] Checking command semantics ===")
    transmissions = parse_and_validate_commands(commands)
    print("Semantics check PASSED!")
    
    verilog_code = generate_verilog_code(transmissions)
    tb_path = Path(args.tb_file)
    tb_content = tb_path.read_text()
    updated_tb = replace_integrated_section(tb_content, verilog_code)
    tb_path.write_text(updated_tb)
    print(f"Successfully modified '{args.tb_file}'. Exiting.\n")

def handle_run(args, commands):
    print("=== [RUN AUTOGRADER] Verifying DUT Output ===")
    out_path = Path(args.output_file)
    if not out_path.exists():
        print(f"Error: Output file '{args.output_file}' does not exist.")
        sys.exit(1)

    transmissions = parse_and_validate_commands(commands)
    expected_tcs = switch_simulation(transmissions)

    raw_text = out_path.read_text()
    if "=== [SETUP AUTOGRADER]" in raw_text:
        raw_text = raw_text.split("=== [SETUP AUTOGRADER]")[-1]

    tc_chunks = raw_text.split("=== TC_START ===")[1:]
    if len(tc_chunks) < len(expected_tcs):
        print(f"[WARNING] Expected {len(expected_tcs)} '=== TC_START ===' markers in log, but found {len(tc_chunks)}.")

    passed_count = 0
    total_count = len(expected_tcs)
    pattern = re.compile(r"\[TX(\d)\]\s+([0-9a-fA-F]{2})")

    switch_learnt_table = {}
    true_mapping_state = {}
    cmd_cursor = 0

    for idx, tc in enumerate(expected_tcs):
        while cmd_cursor < len(commands):
            tokens = commands[cmd_cursor].strip().split()
            cmd_cursor += 1
            if not tokens:
                continue
            if tokens[0].upper() == "MAP":
                m, p = tokens[1].upper(), int(tokens[2])
                for ex_m, ex_p in list(true_mapping_state.items()):
                    if ex_p == p or ex_m == m:
                        del true_mapping_state[ex_m]
                true_mapping_state[m] = p
            elif tokens[0].upper() == "SEND":
                break

        print(f"\n---------------------------------------------------")
        print(f"Test Case {tc['tc_id']}/{total_count}: {tc['m1']} -> {tc['m2']} (Ingress Port {tc['ingress']})")
        print(f"  Command               : '{tc['raw']}'")

        true_map_str = ", ".join([f"{m}:{p}" for m, p in sorted(true_mapping_state.items())])
        learnt_map_str = ", ".join([f"{m}:{p}" for m, p in sorted(switch_learnt_table.items())]) if switch_learnt_table else "EMPTY"

        print(f"  True Mapping          : {{ {true_map_str} }}")
        print(f"  Switch-Learnt Mapping : {{ {learnt_map_str} }}")
        print(f"  EXPECTED Output Ports : {tc['expected_ports'] if tc['expected_ports'] else 'NONE (Filtered)'}")

        switch_learnt_table[tc['m1']] = tc['ingress']

        port_bytes = {0: [], 1: [], 2: [], 3: []}
        if idx < len(tc_chunks):
            for line in tc_chunks[idx].splitlines():
                match = pattern.search(line)
                if match:
                    port = int(match.group(1))
                    val = int(match.group(2), 16)
                    port_bytes[port].append(val)

        actual_ports = []
        tc_failed = False
        exp_bytes = tc["expected_bytes"]

        for p in range(4):
            got_bytes = port_bytes[p]
            should_receive = p in tc['expected_ports']

            if should_receive:
                if len(got_bytes) > 0:
                    actual_ports.append(p)

                print(f"\n  [Port TX{p} Transmission Details]")
                print(decode_and_expand_frame(got_bytes))

                if len(got_bytes) != 15:
                    print(f"  [ERROR] Port {p}: Expected 15 bytes, but received {len(got_bytes)} bytes!")
                    tc_failed = True
                elif got_bytes != exp_bytes:
                    print(f"  [ERROR] Port {p} Data Mismatch!")
                    print(f"    Expected Bytes : {[f'{b:02X}' for b in exp_bytes]}")
                    print(f"    Actual Bytes   : {[f'{b:02X}' for b in got_bytes]}")
                    tc_failed = True
            else:
                if len(got_bytes) > 0:
                    actual_ports.append(p)
                    print(f"\n  [ERROR] Port TX{p}: Unexpected Frame Leaked to Port!")
                    print(decode_and_expand_frame(got_bytes))
                    tc_failed = True

        actual_ports = sorted(actual_ports)
        print(f"\n  ACTUAL Output Ports   : {actual_ports if actual_ports else 'NONE'}")

        if not tc_failed and actual_ports == tc["expected_ports"]:
            print(f"  STATUS                : PASSED")
            passed_count += 1
        else:
            print(f"  STATUS                : FAILED")

    print("\n===================================================")
    print(f"FINAL RESULT: {passed_count}/{total_count} TCs PASSED")
    print("===================================================")

    if passed_count != total_count:
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Interactive Testbench Generator and Autograder")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--setup-autograder", action="store_true", help="Validate global COMMANDS and update Verilog testbench")
    group.add_argument("--run-autograder", action="store_true", help="Verify output log against expected results")

    parser.add_argument("--input-file", default="task5/input.txt", help="Path to commands text file")
    parser.add_argument("--tb-file", default="task5/tb_task5.v", help="Path to Verilog testbench file")
    parser.add_argument("--output-file", default="task5_output.txt", help="Path to output simulation log file")

    args = parser.parse_args()
    commands = load_commands_from_file(args.input_file)

    if args.setup_autograder:
        handle_setup(args, commands)
    elif args.run_autograder:
        handle_run(args, commands)

if __name__ == "__main__":
    main()