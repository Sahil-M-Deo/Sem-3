#!/usr/bin/env python3
import sys
import re

# =====================================================================
# Golden Model: CRC and 4B/5B Definitions
# =====================================================================
ENC_4B5B = {
    '0': '11110', '1': '01001', '2': '10100', '3': '10101',
    '4': '01010', '5': '01011', '6': '01110', '7': '01111',
    '8': '10010', '9': '10011', 'a': '10110', 'b': '10111',
    'c': '11010', 'd': '11011', 'e': '11100', 'f': '11101'
}
DEC_5B4B = {v: k for k, v in ENC_4B5B.items()}

START_SYM = '11000'
END_SYM   = '11001'

def calc_crc8(data_bytes):
    """Calculates CRC-8 using POLY=0xD5, INIT=0xFF (Task 1 specification)"""
    crc = 0xFF
    for b in data_bytes:
        crc ^= b
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0xD5) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc

def golden_tx(dst, src, payload):
    """Generates the expected encoded TX wire bytes"""
    hex_data = f"{dst:02x}{src:02x}{payload:016x}"
    crc = calc_crc8(bytes.fromhex(hex_data))
    hex_with_crc = f"{hex_data}{crc:02x}"
    
    # Build bitstream
    bits = START_SYM
    for char in hex_with_crc:
        bits += ENC_4B5B[char]
    bits += END_SYM
    
    # Pad with 0s to byte align
    while len(bits) % 8 != 0:
        bits += '0'
        
    # Convert back to hex bytes
    byte_list = [f"{int(bits[i:i+8], 2):02x}" for i in range(0, len(bits), 8)]
    return " ".join(byte_list)

def golden_rx(wire_hex):
    """Decodes a wire string and returns expected (valid, dst, src, payload)"""
    # Convert hex wire to bitstream
    bits = bin(int(wire_hex, 16))[2:].zfill(len(wire_hex) * 4)
    
    if not bits.startswith(START_SYM):
        return False, 0, 0, 0
        
    bits = bits[len(START_SYM):]
    nibbles = ""
    
    while len(bits) >= 5:
        sym = bits[:5]
        bits = bits[5:]
        if sym == END_SYM:
            break
        if sym in DEC_5B4B:
            nibbles += DEC_5B4B[sym]
        else:
            return False, 0, 0, 0 # Invalid symbol
            
    if len(nibbles) != 22: # 2(dst) + 2(src) + 16(payload) + 2(crc)
        return False, 0, 0, 0
        
    dst = int(nibbles[0:2], 16)
    src = int(nibbles[2:4], 16)
    payload = int(nibbles[4:20], 16)
    crc_recv = int(nibbles[20:22], 16)
    
    # Verify CRC
    data_bytes = bytes.fromhex(nibbles[:20])
    if crc_recv != calc_crc8(data_bytes):
        return False, 0, 0, 0
        
    return True, dst, src, payload

# =====================================================================
# Log Parser and Verifier
# =====================================================================
def main():
    if len(sys.argv) > 1:
        f = open(sys.argv[1], 'r')
    else:
        f = sys.stdin

    tx_pattern = re.compile(r'\$\$\$\$\$ TX_TEST: dst=0x([0-9a-fA-F]+) src=0x([0-9a-fA-F]+) payload=0x([0-9a-fA-F]+)')
    tx_out_pattern = re.compile(r'##### TX_OUTPUT:\s*(.*)')
    
    rx_pattern = re.compile(r'\$\$\$\$\$ RX_TEST: wire=([0-9a-fA-F]+)')
    rx_out_pattern = re.compile(r'##### RX_OUTPUT: frame_valid=([01]) dst=0x([0-9a-fA-F]+) src=0x([0-9a-fA-F]+) payload=0x([0-9a-fA-F]+)')

    pending_tx = None
    pending_rx = None
    tests_passed = 0
    tests_total = 0

    print("--- Starting Task 3 Verification ---")

    for line in f:
        line = line.strip()
        
        # 1. Match TX Test Stimulus
        m = tx_pattern.search(line)
        if m:
            pending_tx = (int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16))
            continue
            
        # 2. Match TX Output
        m = tx_out_pattern.search(line)
        if m and pending_tx:
            tests_total += 1
            actual = m.group(1).strip().lower()
            expected = golden_tx(*pending_tx)
            
            if actual == expected:
                print(f"[PASS] TX_TEST: dst={pending_tx[0]:02x}")
                tests_passed += 1
            else:
                print(f"[FAIL] TX_TEST: dst={pending_tx[0]:02x}")
                print(f"       Expected : {expected}")
                print(f"       Actual   : {actual}")
            pending_tx = None
            continue

        # 3. Match RX Test Stimulus
        m = rx_pattern.search(line)
        if m:
            pending_rx = m.group(1).lower()
            continue
            
        # 4. Match RX Output
        m = rx_out_pattern.search(line)
        if m and pending_rx:
            tests_total += 1
            act_valid = bool(int(m.group(1)))
            act_dst = int(m.group(2), 16)
            act_src = int(m.group(3), 16)
            act_payload = int(m.group(4), 16)
            
            exp_valid, exp_dst, exp_src, exp_payload = golden_rx(pending_rx)
            
            if (act_valid == exp_valid) and (not exp_valid or (act_dst == exp_dst and act_src == exp_src and act_payload == exp_payload)):
                print(f"[PASS] RX_TEST: wire={pending_rx}")
                tests_passed += 1
            else:
                print(f"[FAIL] RX_TEST: wire={pending_rx}")
                print(f"       Expected : valid={int(exp_valid)} dst={exp_dst:02x} src={exp_src:02x} payload={exp_payload:016x}")
                print(f"       Actual   : valid={int(act_valid)} dst={act_dst:02x} src={act_src:02x} payload={act_payload:016x}")
            pending_rx = None

    if f is not sys.stdin:
        f.close()

    print("-" * 34)
    print(f"Result: {tests_passed} / {tests_total} Tests Passed")
    if tests_total > 0 and tests_passed == tests_total:
        print("Verdict: ALL PERFECT!")
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()