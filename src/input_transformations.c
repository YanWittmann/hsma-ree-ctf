#include "input_transformations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ascii_renderer.h"
#include "generated/hint_system.h"

// SECTION: utility methods

volatile int nevermore = 0;

int random_between(const int min, const int max) {
    if (max <= min) {
        return min;
    }
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int) time(NULL) ^ (unsigned int) clock());
        seeded = 1;
    }
    return (rand() % (max - min + 1)) + min;
}


// END SECTION

// SECTION: methods for computing rotation values:
// - there are more methods than only the actively used ones to add to the confusion
// - the method names are obfuscated to make players have to understand the algorithm (using UUIDs)

// returns 0 if the first letter is a "x" or string is empty, 1 otherwise
#define rtv_check_for_letter rtv_bcdd7c3c1b99

int rtv_check_for_letter(const char *input) {
    if (strlen(input) == 0) {
        return 0;
    }
    return input[0] == 'x' ? 0 : 1;
}

// returns the length of the string as the value
#define rtv_string_length rtv_0e9ae27da403

int rtv_string_length(const char *input) {
    return (int) strlen(input);
}

// computes the sum of ASCII values of all characters in the input
#define rtv_ascii_sum rtv_c9ceb021e47b

int rtv_ascii_sum(const char *input) {
    int sum = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        sum += (int) input[i];
    }
    return sum;
}

// counts the number of vowels in the input
#define rtv_count_vowels rtv_5fde1cd1e0bc

int rtv_count_vowels(const char *input) {
    int count = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        char c = input[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
            ++count;
        }
    }
    return count;
}

// counts the number of consonants in the input
#define rtv_count_consonant rtv_ba45be1c9d61

int rtv_count_consonant(const char *input) {
    int count = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        char c = input[i];
        if (((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) && !(
                c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
                c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')) {
            ++count;
        }
    }
    return count;
}

// calculates the sum of all digits in the input
#define rtv_digit_sum rtv_c78fa75633ce

int rtv_digit_sum(const char *input) {
    int sum = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        if (input[i] >= '0' && input[i] <= '9') {
            sum += (int) input[i] - (int) '0';
        }
    }
    return sum;
}

// counts the number of alternating upper and lower case letters directly following each other in the input
#define rtv_alternating_case_count rtv_ac6e23ce4366

int rtv_alternating_case_count(const char *input) {
    int count = 0;
    for (size_t i = 1; input[i] != '\0'; ++i) {
        if ((input[i - 1] >= 'a' && input[i - 1] <= 'z' && input[i] >= 'A' && input[i] <= 'Z') ||
            (input[i - 1] >= 'A' && input[i - 1] <= 'Z' && input[i] >= 'a' && input[i] <= 'z')) {
            ++count;
        }
    }
    return count;
}

// calculates the difference between the largest and smallest ASCII values in the input
#define rtv_used_char_space_depth rtv_f6aec8261f3e

int rtv_used_char_space_depth(const char *input) {
    note("HNT{17d026b94253}");

    if (*input == '\0') return 0;

    unsigned char min_value = (unsigned char) *input;
    unsigned char max_value = min_value;

    for (size_t i = 1; input[i] != '\0'; ++i) {
        unsigned char current_char = (unsigned char) input[i];
        if (current_char < min_value) min_value = current_char;
        if (current_char > max_value) max_value = current_char;
    }

#ifdef DEBUG_LOGGING
    printf("  [depth] min: %u (%c), max: %u (%c)\n", min_value, min_value, max_value, max_value);
#else
    if (nevermore) {
        printf("%u - %u\n", min_value, max_value);
    }
#endif

    return max_value - min_value;
}

// counts the number of unique characters in the input
#define rtv_unique_characters rtv_872b016e8f00

int rtv_unique_characters(const char *input) {
    int seen[256] = {0};
    int unique_count = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        if (!seen[(unsigned char) input[i]]) {
            seen[(unsigned char) input[i]] = 1;
            ++unique_count;
        }
    }
    return unique_count;
}

// counts the number of set bits across all ASCII characters in the input
#define rtv_bitweight_sum rtv_1a7c5f9e3b02

int rtv_bitweight_sum(const char *input) {
    int sum = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        unsigned char c = input[i];
        while (c) {
            sum += c & 1;
            c >>= 1;
        }
    }

    note("HNT{c8EkrmOaB6awketsu}");

    return sum;
}

// XOR with rotating mask, not too complex here
#define rtv_rotating_xor_mask rtv_4a2f1b9e3f81

int rtv_rotating_xor_mask(const char *input) {
    const unsigned char mask[] = {0x5A, 0x3F, 0x7C};
    int sum = 0;
    for (size_t i = 0; input[i] != '\0'; ++i) {
        sum += input[i] ^ mask[i % 3];
    }
    return sum;
}

// END SECTION

// SECTION: the actual methods will use one or more of the above-defined methods

// all three methods compute_rotation_x, compute_rotation_y, compute_rotation_z calculate a base value using the methods
// defined above and then apply a scaling factor to it.
// this scaling factor is applied because it makes for a harder challenge,
// but also to make small differences in the input matter a lot more in the final calculated rotations.

int compute_rotation_x(const char *input) {
    // all set bits in the input are counted and multiplied by 7

    const int base_value = rtv_bitweight_sum(input);

    const int scaled_value = base_value * 7;

#ifdef DEBUG_LOGGING
    printf("[x] base: %d, scaled: %d\n", base_value, scaled_value);
#else
    // to make it easier for the players to find the inputs that lead to certain rotation values,
    // we introduce a condition that is always false that would print the base and scaled values if activated.
    // the players need to modify the binary such that the condition is met.
    // the 'nevermore' variable is marked as volatile to avoid the compiler from fully removing the condition below.
    if (nevermore) {
        printf("[x] base: %d, scaled: %d\n", base_value, scaled_value);
    }
#endif

    return scaled_value;
}

int compute_rotation_y(const char *input) {
    // the min and max ascii value are determined and their absolute difference is multiplied by 23

    const int base_value = rtv_used_char_space_depth(input);

    const int scaled_value = base_value * 23;

#ifdef DEBUG_LOGGING
    printf("[y] base: %d, scaled: %d\n", base_value, scaled_value);
#else
    if (nevermore) {
        printf("[y] base: %d, scaled: %d\n", base_value, scaled_value);
    }
#endif

    return scaled_value;
}

int compute_rotation_z(const char *input) {
    // if there are 5 or more vowels, at least one consonant and one digit, the base value is calculated as consonant * 5 + the sum of all digits.
    // if this condition is not met, a random value is picked from -45 to 45.
    // then, the base value is scaled by 17.
    // we wanted to include some amount of randomness into the CTF, but the randomness had to be disable-able to
    // produce reproducible rotation values.

    int base_value;

    const int vowels = rtv_count_vowels(input);
    const int consonant = rtv_count_consonant(input);
    const int digit_sum = rtv_digit_sum(input);

    if (vowels >= 5 && consonant > 0 && digit_sum > 0) {
        base_value = consonant * 5 + digit_sum;
#ifdef DEBUG_LOGGING
        printf("  [z] rule base: %d\n", base_value);
#endif
    } else {
        base_value = random_between(-45, 45);
#ifdef DEBUG_LOGGING
        printf("  [z] random base: %d\n", base_value);
#endif
    }

    const int scaled_value = base_value * 17;

#ifdef DEBUG_LOGGING
    printf("[z] base: %d, scaled: %d\n", base_value, scaled_value);
    printf("  [z] vowels: %d, consonants: %d, digit sum: %d\n", vowels, consonant, digit_sum);
#else
    if (nevermore) {
        note("HNT{2jfaL=3%f/=0}");
        printf("[z] base: %d, scaled: %d\n", base_value, scaled_value);
    }
#endif

    return scaled_value;
}

// END SECTION
