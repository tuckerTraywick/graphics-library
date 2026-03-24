args := $(shell pkg-config --cflags freetype2 x11 xft)
libraries := $(shell pkg-config --libs freetype2 x11 xft)
cflags := -std=gnu99 -Wall -Wpedantic -Wextra -g3
cc := gcc

source_files := $(shell find source -name '*.c' -not -name "main.c")
object_files := $(source_files:%=build/%.o)
d_files := $(source_files:%=build/%.d)

test_source_files := $(shell find tests -name '*.c')
test_object_files := $(test_source_files:%=build/%.o)
test_d_files := $(test_source_files:%=build/%.d)

.PHONY: all
all: build/run build/test

build/run: $(object_files) build/source/main.c.o
	@mkdir -p build
	@$(cc) $(libraries) $^ -o $@

build/test: $(test_object_files) $(object_files)
	@mkdir -p build
	@$(cc) $(libraries) $^ -o $@

build/source/%.o: source/%
	@mkdir -p $(dir $@)
	@$(cc) -c -MMD -MP -MT $@ -MF build/source/$*.d $(args) -Iinclude $(libraries) $(cflags) source/$* -o $@

build/tests/%.o: tests/%
	@mkdir -p $(dir $@)
	@$(cc) -c -MMD -MP -MT $@ -MF build/tests/$*.d $(args) -Iinclude -Isource -Itests $(libraries) $(cflags) tests/$* -o $@

-include $(d_files) $(test_d_files)

.PHONY: clean
clean:
	@rm -rf build
