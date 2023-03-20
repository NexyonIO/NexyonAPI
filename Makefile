name = napi
exec = lib$(name).so
service_exec = $(name)d
flags = -Wall -std=c11 

source_files := $(shell find src -path src/service -prune -o -name '*.c' -print)
objects_files := $(patsubst src/%.c, build/%.o, $(source_files))

source_files_service := $(shell find src/service -name *.c)
objects_files_service := $(patsubst src/service/%.c, build/service/%.o, $(source_files_service))

$(objects_files): build/%.o : src/%.c
	mkdir -p $(dir $@) && \
	gcc -fPIC -c -I include -I src $(patsubst build/%.o, src/%.c, $@) -o $@

$(objects_files_service): build/service/%.o : src/service/%.c
	mkdir -p $(dir $@) && \
	gcc -fPIC -c -I include -I src $(patsubst build/service/%.o, src/service/%.c, $@) -o $@

clean:
	-rm -rf build $(exec) $(service_exec) && \
	mkdir build

install:
	cp $(service_exec) /bin && \
	cp $(exec) /lib && \
	cp -r include /usr/

all: clean build-service build-lib

build-service: $(objects_files_service) $(objects_files)
	gcc -o $(service_exec) $(objects_files_service) $(objects_files) $(flags)

build-lib: $(objects_files)
	gcc -shared -o $(exec) $(objects_files) $(flags)
