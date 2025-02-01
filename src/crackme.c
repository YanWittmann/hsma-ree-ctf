#include "crackme.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#include "win.h"
#include "ascii_renderer.h"
#include "input_transformations.h"
#include "generated/model_data.h"
#include "generated/hint_system.h"

#ifndef PREPROCESSING_MODELS
#include "generated/password_hash.h"
#endif

#include "B-con-crypto-algorithms/sha256.h"

static void alarmHandler(int signal) {
    // this method will most likely not be visited by the CTF players.
    // we therefore hide a bunch of fake hint codes in here to make it harder for them to find the real ones.
    note(
        "HNT{You do realize that you can not only solve this CTF entirely without hint codes, but also find all of the hint codes without strings? I guess we can't stop you now, but still, let's give it another try before using them, all right?}"
    );
    note("HNT{-------------------------------------------------------------------}");
    note("the following hint codes are all fake. do not bother entering them.");
    note("HNT{U*e*d251/NbAc}");
    note("HNT{bclVZjwUQgP}");
    note("HNT{h1IK?ZffD6afWLrU79w*37}");
    note("HNT{Ad)s0(4jWznpNv}");
    note("HNT{/lr#UeKvw/c}");
    note("HNT{Qt6GEclX-*?tS}");
    note("HNT{vvWLrU7eKv-yfOG!$KQuX*PO9uemCem8w*1?ve}");
    note("HNT{GEHTQiA*Y98AI6}");
    note("HNT{QnX$Rczi0+4fWUUn6A+mF(}");
    note("HNT{6uemCem8CI*qx!)%Qo6(}");
    note("HNT{7zi0+4fW)#FoF}");
    note("HNT{2O(I*qx!I3U}");
    note("HNT{mu%RV6Fll}");
    note("HNT{rEq7h+HC4zO&Vi0+4fWU)a}");
    note("HNT{2u%vNKQuX*P4}");
    note("HNT{K=PRc%UQ(*4=)5}");
    note("HNT{H%gMsIHNHt20a5}");
    note("HNT{Z(fPg1LB!g!!2}");
    note("HNT{56T$Ko1x7?gsdC}");
    note("HNT{OQtL1GOq$4}");
    note("HNT{JgKE0LNI!z}");
    note("HNT{uYEZ2LfDy=uy}");
    note("HNT{Rlt57XOxE*2Tm=}");
    note("HNT{exw-yfOG!tbeww}");
    note("HNT{z8))XF4rd}");
    note("HNT{NorPLBg0KVjU}");
    note("HNT{z(+hOk0=$}");
    note("HNT{c=qC5HHHjAxkE}");
    note("HNT{9273v/gSFFw}");
    note("HNT{VE=+7pu0b49G=}");
    note("HNT{lc)!SVF)$FK}");
    note("HNT{W)&lsCN}");
    exit(1);
}

void setup(void) {
    setvbuf(stdout, (char *) 0, _IONBF, 0);
    setvbuf(stdin, (char *) 0, _IONBF, 0);
    signal(0xe, alarmHandler);
    alarm(0x3c);
}

void compensation_capybara(const char *input) {
    // set up the 3D model for the ascii renderer
    const Model model = copy_model(reference_model);
    const Vertex center = calculate_center(model);
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic push
    const RenderConfig config = {
        .width = 90,
        .height = 40,
        .scale_global = 90.0f,
        .scale_x = 2.0f,
        .scale_y = 1.0f,
        .camera_position = {0, 0, 1},
        .light_direction = {-0.5f, 0, -1}
    };
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

#ifdef DEBUG_LOGGING
    for (size_t i = 0; i < strlen(input); ++i) {
        printf("%02x ", (unsigned char) input[i]);
    }
    printf("\n");
#endif

    // hint code
    note("HNT{=LNh4(HAjbfdiw$}");

    // use the conversion methods for all three axes.
    // the main challenge this CTF presents is reverse-engineering the algorithms that transform the user input into rotations,
    // so they are obscurely written by design.
    const int angle_x = compute_rotation_x(input);
    const int angle_y = compute_rotation_y(input);
    const int angle_z = compute_rotation_z(input);
    ctf_impl_rotate_model_deg(&model, center, angle_x, angle_y, angle_z);

    note("HNT{pk(Z)kdHBA55/&}");

#ifndef PREPROCESSING_MODELS
    // check if the input lead to a correct rotation, as calculated by the preprocessor.
    // if so, display a different hint text and color the output to let the users know they are on the right path.
    const int success = check_rotation_combination(encode_rotation(angle_x, angle_y, angle_z));
    if (success) {
        printf("\n");
        printf(" .-------------------------------------------------------------------------------------------.\n");
        printf(" | The capybara awakens, its movements now aligned with the cosmic dance of the void.        |\n");
        printf(" | The forgotten creature acknowledges your mastery over the unseen forces that guide it.    |\n");
        printf(" | But one capybara alone cannot complete the dance - its twin must also align.              |\n");
        printf(" | Sum their essence, multiply their bond, and break the cipher to unlock the path.          |\n");
        printf(" '-------------------------------------------------------------------------------------------'\n");
        printf("\n");
        printf("\033[1;36m");
        printf("=== v v v v v v v capy bounds v v v v v v v ===\n");
        render_ascii(model, config);
        printf("=== ^ ^ ^ ^ ^ ^ ^ capy bounds ^ ^ ^ ^ ^ ^ ^ ===\n");
        printf("\033[0m");
    } else {
        printf("\n");
        printf(" .-------------------------------------------------------------------------------------------.\n");
        printf(" | Within the void's vast silence, a forgotten capybara shifts, responding to unseen forces. |\n");
        printf(" | Moved by the subtle force of unseen hands, its rules embedded in the depths.              |\n");
        printf(" | To comprehend its binary dance, walk the hidden path, peek yet deeper inside.             |\n");
        printf(" '-------------------------------------------------------------------------------------------'\n");
        printf("\n");
        render_ascii(model, config);
    }
#endif

    free_model(model);
}


#ifdef PREPROCESSING_MODELS

void preprocessing_model(const char *input) {
    // method called during build.
    // used to simply render a 3D model from a string.
    int angle_x = compute_rotation_x(input);
    int angle_y = compute_rotation_y(input);
    int angle_z = compute_rotation_z(input);

    printf("# %s\n", input);
    printf("# %d %d %d\n", angle_x, angle_y, angle_z);

    const RenderConfig config = {
        .width = 90,
        .height = 40,
        .scale_global = 90.0f,
        .scale_x = 2.0f,
        .scale_y = 1.0f,
        .camera_position = {0, 0, 1},
        .light_direction = {-0.5f, 0, -1}
    };

    const Model model = copy_model(reference_model);
    const Vertex center = calculate_center(model);
    ctf_impl_rotate_model_deg(&model, center, angle_x, angle_y, angle_z);
    render_ascii(model, config);

    free_model(model);
}

#endif

void ascii_input(char *input, size_t size) {
    printf("Enter the secret code: ");
    fflush(stdout);
    if (fgets(input, size, stdin)) {
        // strip the newline character, if present
        const size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
    }
}

int main() {
    // the PREPROCESSING_MODELS flag is set by the Makefile in the preprocessing step.
    // the two inputs are transformed into rotations and the corresponding capybaras are printed to the stdout.
    // the stdout is redirected into a file, which is interpreted by a python file to generate several hash codes to
    // check whether the user input is correct.
#ifdef PREPROCESSING_MODELS
    printf("===\n");
    preprocessing_model("\x75\x61\x61\x61\x61\x61\x62\x33\x20\x01\x01\x01\x01");
    printf("===\n");
    preprocessing_model("\x61\x65\x69\x6F\x75\x79\x79\x79\x79\x39\x02\x05");

    return 0;
#else
    // debugging flag
#ifdef ENABLE_MODEL_ROTATION
    win_capybara();
    return 0;
#else

    // here starts the real program that is distributed in the CTF

    // allow the user to input a text
    char input[64];
    ascii_input(input, sizeof(input));

    // hash the input using sha256 to prevent players from being able to look up the solution password string in the crackme
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *) input, strlen(input));
    BYTE hash[SHA256_BLOCK_SIZE];
    sha256_final(&ctx, hash);

#ifdef DEBUG_LOGGING
    printf("The calculated hash is: ");
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        printf("%02x", hash[i]);
    }
    printf("\n");
    printf("Comparing correct hash: ");
    for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        printf("%02x", correct_hash[i]);
    }
    printf("\n");
#endif

    // check if the correct password was entered by comparing the hashes
    if (memcmp(hash, correct_hash, SHA256_BLOCK_SIZE) == 0) {
        // display the flag if so
        win_capybara();
        return 0;
    }

    // if input starts with "HNT{", let the hint system handle it
    if (strlen(input) > 5 && strncmp(input, "HNT{", 4) == 0) {
        handle_hint_system(input);
        return 0;
    }

    // otherwise, the input was not the password or a hint code.
    // display the capybara with the rotation derived from the user input.
    compensation_capybara(input);

    return 0;
#endif

#endif
}
