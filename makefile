ARCH	=	microROS-BEC_HUET.zip
CC		=	gcc
CFLAGS	=	-g -Wall -I fastsim
LDFLAGS	=	-lm -L fastsim -lfastsim -lpthread

EXE	=	simu_ex master node_logger node_echo node_simu node_tele node_ia


all: $(EXE)


#Creation des executables
$(EXE): % :
	$(CC) -o $@ $^ $(LDFLAGS)

#Creation des .o
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)


#Dependances des executables 
simu_ex: main_simu_ex.o
master: main_master.o util_node.o util_config.o util_fifo.o
node_logger: main_logger.o
node_echo: main_echo.o
node_simu: main_simu.o util_simu.o util_fifo.o
node_tele: main_tele.o
node_ia: main_ia.o

#Dependances des .o
main_simu_ex.o: main_simu_ex.c
main_master.o: main_master.c util_node.h util_config.h
main_logger.o: main_logger.c
main_echo.o: main_echo.c
main_tele.o: main_tele.c
main_simu.o: main_simu.c util_simu.h
main_ia.o: main_ia.c
util_node.o: util_node.c util_node.h
util_config.o: util_config.c util_config.h util_node.h
util_fifo.o: util_fifo.c util_fifo.h
util_simu.o: util_simu.c util_simu.h


#Création des cibles .PHONY
.PHONY: clean fclean re zip

clean:
	@rm -rf *.o *.fifo

fclean:
	@rm -rf *.o *.fifo $(EXE)

re: fclean all

zip:
	@zip -r -u $(ARCH) *.c *.h config_*.txt README.TXT makefile fastsim/

