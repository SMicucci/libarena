CC = gcc
CPPFLAGS = -Wall -Wextra -Wpedantic -Werror
CPPEXTRA = -std=c99 -fPIC -O3 -fsanitize=address,undefined

SRC_d = src/
OBJ_d = obj/
LIB_d = lib/
# test
T_SRC_d = test/
T_OBJ_d = t_obj/
BIN_d = bin/

SRCs = $(wildcard $(SRC_d)*.c)
OBJs = $(patsubst $(SRC_d)%.c,$(OBJ_d)%.o,$(SRCs))

#
# test
#
T_SRCs = $(filter-out $(T_SRC_d)test.c,$(wildcard $(T_SRC_d)*.c))
T_OBJs = $(patsubst $(T_SRC_d)%.c,$(T_OBJ_d)%.o,$(T_SRCs))
T_LIBs = $(patsubst $(T_SRC_d)%.c,$(BIN_d)%.so,$(T_SRCs))

LIB = arena
BIN = test

all: dynamic static

dynamic: $(LIB_d)lib$(LIB).so
static: $(LIB_d)lib$(LIB).a

$(LIB_d)lib$(LIB).so: $(OBJs) | $(LIB_d)
	$(CC) -shared -o $@ $^

$(LIB_d)lib$(LIB).a: $(OBJs) | $(LIB_d)
	@ar rcs $@ $^

$(OBJ_d)%.o: $(SRC_d)%.c | $(OBJ_d)
	$(CC) $(CPPFLAGS) $(CPPEXTRA) -c $< -o $@

$(OBJ_d) $(LIB_d) $(BIN_d) $(T_OBJ_d):
	mkdir -p $(OBJ_d) $(LIB_d) $(BIN_d) $(T_OBJ_d)

#
# test
#
test: $(LIB_d)lib$(LIB).so $(BIN_d)$(BIN)

$(T_OBJ_d)%.o: $(T_SRC_d)%.c | $(T_OBJ_d)
	$(CC) $(CPPFLAGS) $(CPPEXTRA) -c $< -o $@

$(BIN_d)%.so: $(T_OBJ_d)%.o | $(BIN_d)
	$(CC) -shared -o $@ $< -L$(PWD)/lib -larena -Wl,-rpath,$(PWD)/lib

$(BIN_d)$(BIN): $(T_SRC_d)test.c $(T_LIBs) | $(BIN_d)
	$(CC) $(CPPFLAGS) $(CPPEXTRA) -ldl -o $@ $<

clean-test:
	rm -rf $(BIN_d) $(T_OBJ_d)

clean:
	rm -rf $(OBJ_d) $(LIB_d) $(BIN_d) $(T_OBJ_d)

.PHONY: all clean dynamic static test clean-test
