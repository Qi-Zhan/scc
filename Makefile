
all:
	@cd src && make
	mv src/scc .

run:
	@cd src && make run

clean:
	@cd src && make clean
	@rm scc

test: all
