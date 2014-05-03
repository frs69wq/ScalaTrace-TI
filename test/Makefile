TESTS=\
	  1dstencil \
	  looplcs \

all:
	(for i in $(TESTS); do (cd $$i; $(MAKE) $@) || break; done)

clean:
	(for i in $(TESTS); do (cd $$i; $(MAKE) $@); done)

veryclean:
	(for i in $(TESTS); do (cd $$i; $(MAKE) $@); done)
