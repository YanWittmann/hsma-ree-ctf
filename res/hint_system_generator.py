import argparse
import os

TEMPLATE = '''#include "hint_system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned char BYTE;

{hints}

void note(const char *input) {{
}}

void process_hint(const char *input, const BYTE *data, size_t data_len, const char *hint) {{
    size_t input_len = strlen(input);
    char *result = (char *)malloc(data_len + 1);

    for (size_t i = 0; i < data_len; i++) {{
        result[i] = data[i] ^ input[i % input_len];
    }}
    result[data_len] = '\\0';

    const int is_correct = strlen(result) >= 4 && strncmp(result + strlen(result) - 4, ":ABC", 4) == 0;
    if (is_correct) {{
        printf("\\033[1;33m");
    }} else {{
        printf("\\033[1;90m");
    }}

    printf(" ╔═══ %s\\n", hint);
    if (is_correct) {{
        printf(" ║ %.*s\\n", (int)(strlen(result) - 4), result);
    }} else {{
        printf(" ║ ");
        for (size_t i = 4; result[i] != '\\0'; i++) {{
            if (isprint(result[i]) && result[i] != '\\n') {{
                printf("%c", result[i]);
            }}
        }}
        printf("\\n");
    }}
    printf(" ╚═══════════════════════════════════════════════════════════════════\\n");
    
    printf("\\033[0m");

    free(result);
    
    note("HNT{{J=b7r(&j3rLp?)AzQBdmy}}");
}}

void handle_hint_system(const char *input) {{
    note("These hints are optional. All information required to solve the challenge is available through other means. Use this system if you are stuck.");
    printf("\\n");
    printf("         \\033[1;90m╔═══════════════════════════════════════════════════════╗\\033[0m\\n");
    printf("         \\033[1;90m║\\033[0m                     HINT SYSTEM                       \\033[1;90m║\\033[0m\\n");
    printf("         \\033[1;90m║\\033[0m   The void-bound capybara is a creature of mystery.   \\033[1;90m║\\033[0m\\n");
    printf("         \\033[1;90m║\\033[0m  It is willing to lift a few boundaries that prevent  \\033[1;90m║\\033[0m\\n");
    printf("         \\033[1;90m║\\033[0m  you from communicating with it. Pay its price: find  \\033[1;90m║\\033[0m\\n");
    printf("         \\033[1;90m║\\033[0m   the passwords hidden in this world to unlock them.  \\033[1;90m║\\033[0m\\n");
    printf("         \\033[1;90m╚═══════════════════════════════════════════════════════╝\\033[0m\\n\\n");
{process_calls}
}}
'''


def generate_hint_block(index, hint_text, password, hint_for_hint):
    hint_text = hint_text + ":ABC"
    password = "HNT{" + password + "}"
    xor_data = [ord(h) ^ ord(password[i % len(password)]) for i, h in enumerate(hint_text)]
    xor_data_hex = ", ".join(f"0x{byte:02x}" for byte in xor_data)
    return f'''
const char *hnt_{index:02d}_hint = "{hint_for_hint}";
const BYTE hnt_{index:02d}_data[] = {{ {xor_data_hex} }};'''


def generate_process_call(index):
    return f"    process_hint(input, hnt_{index:02d}_data, sizeof(hnt_{index:02d}_data), hnt_{index:02d}_hint);"


def generate_file(hints):
    hint_blocks = []
    process_calls = []

    for i, hint in enumerate(hints, 1):
        hint_blocks.append(generate_hint_block(i, hint['hint'], hint['password'], hint['hint_for_hint']))
        process_calls.append(generate_process_call(i))

    return TEMPLATE.format(hints="\n".join(hint_blocks), process_calls="\n    printf(\"\\n\");\n".join(process_calls))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create the hash key for the password')
    parser.add_argument('hint_system', type=str, help='Path to the hint_system output C file, without extension')
    args = parser.parse_args()

    parent_dir = os.path.dirname(args.hint_system)
    if parent_dir:
        os.makedirs(parent_dir, exist_ok=True)

    hints = [
        {
            "hint": "Find the file embedded within by taking the binary for a walk. "
                    "Though it's contents may appear cryptic at first, "
                    "trust that it will lead you throughout the entire journey.",
            "hint_for_hint": "[A] capybara compensation",
            "password": "=LNh4(HAjbfdiw$"
        },
        {
            "hint": "Unsigned characters store the data. "
                    "Remember other ways to input data into the program to type invisible characters. "
                    "echo -ne '\\x65' | ./crackme",
            "hint_for_hint": "sd_chr_spc_dpth",
            "password": "17d026b94253"
        },
        {
            "hint": "Ask the mighty dragon to forge you an assembler to modify the 'nevermore' instruction. "
                    "Suddenly, the previously silent capybara will share it's viewing angles with you. "
                    "It shall make the task of cracking the rotations a breeze.",
            "hint_for_hint": "nevermore in the z (ebra) axis",
            "password": "2jfaL=3%f/=0"
        },
        {
            "hint": "Hint [01] [A] was serious: "
                    "Before you can do anything, you must find the hidden file within. "
                    "You might want to --dd='.*' over it and take a closer look at the c [omp] ute_ro[t] ation functions. "
                    "The ren [de]r_a [sci]i function is off limits.",
            "hint_for_hint": "[B] capybara compensation",
            "password": "pk(Z)kdHBA55/&"
        },
        {
            "hint": "At first, this document may seem overwhelming. "
                    "Break down the flowchart [ from top to bottom ], multiple operations are performed on some data. "
                    "Understanding the functions and data it operates on will bring clarity. "
                    "HNT{how about you sum up all the ascii characters in this message ignoring spaces (and newlines technically even though there are none)}",
            "hint_for_hint": "ransom note meta data",
            "password": "mf04hj=H)&5FCif"
        },
        {
            "hint": "Force the two rotations seen at the top, sum up the resulting ascii values (using the same rules as you just did to reach this hint) and multiply these two sums. "
                    "Then, you shall apply the functions in the given order on that data and finally XOR the result with the provided key. "
                    "Remember that every character and digit matters on this journey.",
            "hint_for_hint": "sum up chars @HNT_05",
            "password": "31291"
        },
        {
            "hint": "You may use yanwittmann.de/ projects/ tools/ xor-convert.html to perform any XOR operations you may need to. "
                    "The same goes for converting between bases: yanwittmann.de/ projects/ tools/ base-converter.html "
                    "Your goal is not to crack this hint system.",
            "hint_for_hint": "hnt-sys xor fctn",
            "password": "J=b7r(&j3rLp?)AzQBdmy"
        },
        {
            "hint": "We do not expect you to crack sha256 hashes. "
                    "Search for your next hint elsewhere.",
            "hint_for_hint": "sha256",
            "password": "l6)dgD7=2kVuW"
        },
        {
            "hint": "The capybara usually recites it's alphabet the other way around, starting at z, y, x, and so on. Also, my capy, use an ASCII table! Don't make your life this hard.",
            "hint_for_hint": "[x] axis",
            "password": "c8EkrmOaB6awketsu"
        },
        {
            "hint": "You've ventured too far into the depths. "
                    "The answers you seek were already within your grasp. "
                    "Revisit earlier stages to find what you missed.",
            "hint_for_hint": "a place too deep",
            "password": "1185e13dd020"
        },
    ]

    # iterate over them with the index to add [01], [02], etc. to the hint_for_hint
    for i, hint in enumerate(hints, 1):
        hint['hint_for_hint'] = f"[{i:02d}] {hint['hint_for_hint']}"
        # print("echo \"HNT{" + hint['password'] + "}\" | ./crackme") # testing purposes

    output = generate_file(hints)

    with open(f"{args.hint_system}.c", "w") as f:
        f.write(output)
