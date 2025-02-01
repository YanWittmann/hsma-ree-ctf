import subprocess
import hashlib
import argparse
import re


def run_crackme(payload, crackme_path):
    try:
        echo_result = subprocess.run(
            ["echo", "-ne", payload],
            stdout=subprocess.PIPE,
            text=True
        )

        result = subprocess.run(
            [crackme_path],
            input=echo_result.stdout,
            capture_output=True,
            text=True
        )

        if result.returncode != 0:
            raise RuntimeError(
                f"Error running crackme: {result.stderr.strip()}"
            )

        return result.stdout.strip()
    except Exception as e:
        raise RuntimeError(f"Failed to execute crackme: {e}") from e


def strip_header(output):
    lines = output.splitlines()
    start_marker = "=== v"
    end_marker = "=== ^"

    start_index = None
    end_index = None
    for i, line in enumerate(lines):
        if start_marker in line:
            start_index = i + 1
        elif end_marker in line:
            end_index = i
            break

    if start_index is not None and end_index is not None:
        return "\n".join(lines[start_index:end_index])

    raise ValueError("Could not find the start and end markers in the output.")


def ascii_sum(input_str):
    char_count = {}
    for char in input_str:
        if char == ' ' or char == '\n':
            continue
        char_count[char] = char_count.get(char, 0) + 1
    print("char count: ", char_count)
    return sum(ord(char) for char in input_str if char != ' ' and char != '\n')


def calculate_sha256(data):
    return hashlib.sha256(data.encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "crackme_path", type=str, help="Path to the crackme binary."
    )
    parser.add_argument(
        "payload1", type=str, help="First payload in hex format ('\\x75\\x61\\x61')."
    )
    parser.add_argument(
        "payload2", type=str, help="Second payload in hex format ('\\x61\\x65\\x69')."
    )
    args = parser.parse_args()

    final_result = None

    try:
        parsed_payload1 = bytes(args.payload1, "utf-8").decode("unicode_escape")
        parsed_payload2 = bytes(args.payload2, "utf-8").decode("unicode_escape")

        print(f"Running crackme with payload 1: {args.payload1}, {parsed_payload1}")
        output1 = run_crackme(parsed_payload1, args.crackme_path)
        stripped_output1 = strip_header(output1)

        ascii_sum1 = ascii_sum(stripped_output1)
        print(f"Payload 1 ASCII Sum: {ascii_sum1}")

        print(f"\nRunning crackme with payload 2: {args.payload2}, {parsed_payload2}")
        output2 = run_crackme(parsed_payload2, args.crackme_path)
        stripped_output2 = strip_header(output2)

        ascii_sum2 = ascii_sum(stripped_output2)
        print(f"Payload 2 ASCII Sum: {ascii_sum2}")

        product = ascii_sum1 * ascii_sum2
        print(f"Product of ASCII Sums: {product}")

        product_binary = f"{product:032b}"
        print(f"Product in binary: {product_binary}")

        product_binary = product_binary * 4
        print(f"Repeated 4 times: {product_binary}")

        hex_key = "CF146B53D41C5943CF165C50C0164061"
        hex_key = bytes.fromhex(hex_key)
        print(f"Hex key: {hex_key}")

        product_bytes = int(product_binary, 2).to_bytes((len(product_binary) + 7) // 8, byteorder="big")

        xor_length = min(len(hex_key), len(product_bytes))
        xor_result = bytes(a ^ b for a, b in zip(hex_key[:xor_length], product_bytes[:xor_length]))
        xor_result = "".join(chr(byte) for byte in xor_result)
        print(f"XOR Result: {xor_result}")

        result = subprocess.run(
            [args.crackme_path],
            input=xor_result,
            capture_output=True,
            text=True,
            timeout=1
        )

        final_result = result.stdout.strip()
    except subprocess.TimeoutExpired as e:
        output = (e.stdout.decode('utf-8', errors='ignore') if e.stdout else "").strip()
        error = (e.stderr.decode('utf-8', errors='ignore') if e.stderr else "").strip()
        final_result = output + "\n" + error

    except Exception as e:
        print(f"Error: {e}")

    flag = re.search(r"FLAG\{.*}", final_result)
    if flag:
        print(f"FLAG: {flag.group()}")
    else:
        print("No flag found.")


if __name__ == "__main__":
    main()
