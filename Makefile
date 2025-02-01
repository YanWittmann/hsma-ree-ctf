CC = gcc
PYTHON = python3

CFLAGS = -Wall -Iinclude -fno-stack-protector

SRC_INITIAL = src/crackme.c src/win.c src/input_transformations.c src/ascii_renderer.c src/generated/model_data.c src/generated/hint_system.c src/B-con-crypto-algorithms/sha256.c
SRC_FINAL = $(SRC_INITIAL) src/generated/password_hash.c
OUTPUT = crackme

PY_OBJ_CONVERTER = res/obj_converter.py
PY_HINT_SYSTEM_GENERATOR = res/hint_system_generator.py
EMBEDDED_FILE = res/capy_ransom.pdf

ADDON_CFLAGS =
# ADDON_CFLAGS += -DENABLE_MODEL_ROTATION
# ADDON_CFLAGS += -DDEBUG_LOGGING

PREPROCESS_OUTPUT = preprocess_output.txt
PASSWORD_HASH_C = src/generated/password_hash.c
PASSWORD_HASH_H = src/generated/password_hash.h

all: $(OUTPUT)

crackme: $(OUTPUT)

$(OUTPUT): $(SRC_FINAL) $(PASSWORD_HASH_H)
		$(CC) $(CFLAGS) $(SRC_FINAL) -o $(OUTPUT) -lm $(ADDON_CFLAGS)
		@cat $(EMBEDDED_FILE) >> $@

initial_build: $(SRC_INITIAL)
		$(CC) $(CFLAGS) $(SRC_INITIAL) -o $(OUTPUT) -lm -DPREPROCESSING_MODELS
		@cat $(EMBEDDED_FILE) >> $@

$(PREPROCESS_OUTPUT): initial_build
		./$(OUTPUT) > $(PREPROCESS_OUTPUT)

$(PASSWORD_HASH_C) $(PASSWORD_HASH_H): $(PREPROCESS_OUTPUT)
		$(PYTHON) res/hash_preprocessor.py $(PREPROCESS_OUTPUT) "src/generated/password_hash"

src/generated/model_data.c src/generated/model_data.h: preprocess
preprocess:
		$(PYTHON) $(PY_OBJ_CONVERTER) "res/capybara.obj" "src/generated/model_data"
		$(PYTHON) $(PY_HINT_SYSTEM_GENERATOR) "src/generated/hint_system"

clean:
		rm -f $(OUTPUT) $(PREPROCESS_OUTPUT) $(PASSWORD_HASH_C) $(PASSWORD_HASH_H) src/generated/model_data.c src/generated/model_data.h

.PHONY: all clean preprocess initial_build
