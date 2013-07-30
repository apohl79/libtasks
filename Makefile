LIB = tasks/libtasks.a
EXAMPLES = examples
VERSION := $(shell cat VERSION)

all: $(LIB) $(EXAMPLES)

$(EXAMPLES): $(LIB)

$(LIB):
	cd tasks && $(MAKE) $(MAKE_ARGS) VERSION=$(VERSION)

$(EXAMPLES):
	for e in examples/*; do \
	  cd $$e && $(MAKE) $(MAKE_ARGS) -j1 && cd - ; \
	done 

clean:
	cd tasks; $(MAKE) clean; cd -
	for e in examples/*; do \
	  cd $$e; $(MAKE) clean; cd - ; \
	done 
