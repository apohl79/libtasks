LIB = tasks/libtasks.a
EXAMPLES = examples/echoserver

all: $(LIB) $(EXAMPLES)

$(EXAMPLES): $(LIB)

$(LIB):
	cd tasks && $(MAKE) $(MAKE_ARGS)

$(EXAMPLES):
	cd examples && $(MAKE) $(MAKE_ARGS)

clean:
	cd tasks && make clean && cd ..
	cd examples && make clean && cd ..
