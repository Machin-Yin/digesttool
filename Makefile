cc = gcc
prom = digesttool
deps = dealcmdpara.h sm3.h
obj = main.o dealcmdpara.o sm3.o

$(prom): $(obj)
	$(cc) -o $(prom) $(obj)
%.o: %.c $(deps)
	$(cc) -c $< -o $@ -g
clean:
	rm -rf $(obj) $(prom)
