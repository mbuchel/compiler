ifndef VERBOSE
.SILENT:
endif

STACK_PATH := `stack path --local-install-root`

all:
	mkdir -p bin
	stack build
	cp $(STACK_PATH)/bin/compiler bin/
	echo "Made compiler"

docs:
	mkdir -p docs
	stack exec -- haddock --html src/* --odir=docs

clean:
	rm -rf .stack-work bin dist docs
