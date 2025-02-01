import hashlib
import argparse
import os
import re


def parse_preprocess_output(file_path):
    """Parses the content of a file with sections divided by '===', extracting rotation values and content."""
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()

        rotation_section_pairs = []
        current_rotation = None
        current_section = []

        for line in lines:
            line = line.strip()
            if line.startswith("# "):
                if re.match(r"# \d+\s+\d+\s+\d+", line):
                    # If we have a previous section, add it to the results
                    if current_rotation is not None:
                        rotation_section_pairs.append({
                            'rotation': current_rotation,
                            'section': '\n'.join(current_section).strip()
                        })
                        current_section = []

                    # Extract the rotation values
                    rotation_values = line.replace("#", "").strip().split()
                    current_rotation = {
                        'x': int(rotation_values[0]),
                        'y': int(rotation_values[1]),
                        'z': int(rotation_values[2])
                    }
                    print(f"New section: [rotation={current_rotation}]")
                else:
                    print("Comment line:", line)
                    continue
            elif line == "===":
                # Ignore separator lines
                continue
            else:
                # Add non-rotation, non-separator lines to the current section
                current_section.append(line)

        # Add the last section if it exists
        if current_rotation is not None:
            rotation_section_pairs.append({
                'rotation': current_rotation,
                'section': '\n'.join(current_section).strip()
            })

        return rotation_section_pairs

    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        return []


def ascii_sum(input_str):
    """Returns the sum of the ASCII values of all characters in a string. Logs the count of each character."""
    char_count = {}
    for char in input_str:
        if char == ' ' or char == '\n':
            continue
        char_count[char] = char_count.get(char, 0) + 1
    print("char count: ", char_count)
    return sum(ord(char) for char in input_str if char != ' ' and char != '\n')


def dec_to_hex_string(number: int) -> str:
    return hex(number).lstrip("0x") or "0"


def text_to_hex_string(text: str) -> str:
    return "".join(f"{ord(char):02X}" for char in text)


def pad_hex_strings(hex1: str, hex2: str) -> tuple[str, str]:
    hex1 += "0" * (len(hex1) % 2)
    hex2 += "0" * (len(hex2) % 2)
    # repeat to match or exceed the other's length
    hex1 = hex1 * ((len(hex2) + len(hex1) - 1) // len(hex1))
    hex2 = hex2 * ((len(hex1) + len(hex2) - 1) // len(hex2))
    max_length = max(len(hex1), len(hex2))
    # trim to max_length
    hex1 = hex1[:max_length]
    hex2 = hex2[:max_length]
    # pad from the right with 0s to ensure even length (len(hex1) + len(hex1) % 2)
    hex1 += "0" * (len(hex1) % 2)
    hex2 += "0" * (len(hex2) % 2)
    return hex1, hex2


def calculate_sha256(data):
    """Calculates the hex SHA-256 hash of a given string."""
    return hashlib.sha256(data.encode()).hexdigest()


def write_header_file(output_file):
    with open(output_file, 'w') as file:
        file.write("#ifndef CTF_FLAG_PASSWORD_FILE_H\n")
        file.write("#define CTF_FLAG_PASSWORD_FILE_H\n\n")
        file.write("#include <string.h>\n")
        file.write("#include <stdio.h>\n")
        file.write("#include <stdlib.h>\n")
        file.write("#include \"../B-con-crypto-algorithms/sha256.h\"\n\n")
        file.write("extern BYTE correct_hash[SHA256_BLOCK_SIZE];\n\n")
        file.write("extern char *encode_rotation(int x, int y, int z);\n\n")
        file.write("extern int check_rotation_combination(const char *input);\n\n")
        file.write("#endif // CTF_FLAG_PASSWORD_FILE_H\n")
    print(f"Header file written to {output_file}")


def generate_bytes_string(name, content):
    sha256_hash = calculate_sha256(content)
    byte_list = [int(sha256_hash[i:i + 2], 16) for i in range(0, len(sha256_hash), 2)]
    """Generates a string for the given byte list with the specified name."""
    byte_lines = ",\n".join(
        (" " * 8) + ", ".join(f"0x{byte:02x}" for byte in byte_list[i:i + 8])
        for i in range(0, len(byte_list), 8)
    )
    return f"const BYTE {name}[SHA256_BLOCK_SIZE] = {{\n{byte_lines}\n}};\n"


def generate_source_file(password, rotation_strings, output_file):
    with open(output_file, 'w') as file:
        file.write("#include <string.h>\n")
        file.write("#include <stdio.h>\n")
        file.write("#include <stdlib.h>\n")
        file.write('#include "../B-con-crypto-algorithms/sha256.h"\n\n')
        file.write(generate_bytes_string("correct_hash", password))
        file.write("\n")

        for i, rotation_string in enumerate(rotation_strings):
            file.write(generate_bytes_string(f"rotation_{i}", rotation_string))
        file.write("\n")

        file.write("char* encode_rotation(int x, int y, int z) {\n")
        file.write("    char* result = (char*)malloc(20 * sizeof(char));\n")
        file.write("    if (result != NULL) {\n")
        file.write("        sprintf(result, \"%d_%d_%d\", x, y, z);\n")
        file.write("#ifdef DEBUG_LOGGING\n")
        file.write("        printf(\"Encoded rotation: %s\\n\", result);\n")
        file.write("#endif\n")
        file.write("    }\n")
        file.write("    return result;\n")
        file.write("}\n\n")

        file.write("int check_rotation_combination(const char *input) {\n")
        file.write("    SHA256_CTX ctx;\n")
        file.write("    sha256_init(&ctx);\n")
        file.write("    sha256_update(&ctx, (BYTE *) input, strlen(input));\n")
        file.write("    BYTE hash[SHA256_BLOCK_SIZE];\n")
        file.write("    sha256_final(&ctx, hash);\n")
        file.write("#ifdef DEBUG_LOGGING\n")
        file.write("    printf(\"The calculated hash for the input rotation is: \");\n")
        file.write("    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {\n")
        file.write("        printf(\"%02x\", hash[i]);\n")
        file.write("    }\n")
        file.write("    printf(\"\\n\");\n")
        file.write("#endif\n")
        for i in range(len(rotation_strings)):
            file.write("    { // check rotation " + str(i) + "\n")
            # again debug logging
            file.write("#ifdef DEBUG_LOGGING\n")
            file.write("    printf(\"Checking rotation " + str(i) + " with correct hash: \");\n")
            file.write("    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {\n")
            file.write("        printf(\"%02x\", rotation_" + str(i) + "[i]);\n")
            file.write("    }\n")
            file.write("    printf(\"\\n\");\n")
            file.write("#endif\n")
            file.write("        if (memcmp(hash, rotation_" + str(i) + ", SHA256_BLOCK_SIZE) == 0) {\n")
            file.write("            return 1;\n")
            file.write("        }\n")
            file.write("    }\n")
        file.write("    return 0;\n")
        file.write("}\n")
    print(f"Source file written to {output_file}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create the hash key for the password')
    parser.add_argument('input', type=str, help='Path to the file to be processed')
    parser.add_argument('password_hash', type=str, help='Path to the password_hash output C file, without extension')

    args = parser.parse_args()
    sections = parse_preprocess_output(args.input)

    sums = [ascii_sum(section['section']) for section in sections]

    product = 1
    for i in range(len(sections)):
        print(f"Section {i + 1}: [length={len(sections[i]['section'])}] [sum={sums[i]}]")
        product *= sums[i]

    product_hex = dec_to_hex_string(product)
    print("Product of all sums:", product, f"[hex={product_hex}]")

    # XOR the target password with the product
    target_password = "PW{OK_I_PULL_UP}"
    target_password_hex = text_to_hex_string(target_password)
    print("Target password:", target_password, f"[hex={target_password_hex}]")

    # Pad the hexadecimal strings
    target_password_hex, product_hex = pad_hex_strings(target_password_hex, product_hex)

    # Perform XOR
    key = bytes([a ^ b for a, b in zip(bytes.fromhex(target_password_hex), bytes.fromhex(product_hex))])
    key = key.hex().upper()

    print("XOR(", target_password_hex,
          ",", product_hex,
          ") =", key)
    print("key =", key)

    # assert that key is expected
    assert key == "CF146B53D41C5943CF165C50C0164061"

    # individually concatenate the rotation values to a string separated with "_"
    rotation_strings = [f"{rotation['rotation']['x']}_{rotation['rotation']['y']}_{rotation['rotation']['z']}" for
                        rotation in sections]

    parent_dir = os.path.dirname(args.password_hash)
    if parent_dir:
        os.makedirs(parent_dir, exist_ok=True)

    # write the key to a source file for the next build
    write_header_file(args.password_hash + ".h")
    generate_source_file(target_password, rotation_strings, args.password_hash + ".c")
