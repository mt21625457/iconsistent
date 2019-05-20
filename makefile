src = $(wildcard ./*.c)
obj = $(patsubst %.c,%.o,$(src))
appname = consistent


$(appname):$(obj)
	gcc $(obj) -o $(appname) -lpthread -lm

%.o:%.c
	gcc -g  -c $< -o $@

.PHONY:clean
clean:
	rm $(appname) $(obj) -f
