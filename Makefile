.PHONY: ruby
.PHONY: parser
.PHONY: vm

ruby: parser vm
	make -C ruby

parser:
	make -C parser

vm:
	make -C vm

clean:
	make -C ruby clean
	make -C parser clean
	make -C vm clean
