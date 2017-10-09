vpath %.h include
vpath %.c src
vpath %.o obj

CC = gcc
CFLAGS = -g -Wall
IFLAGS = -Iinclude
ALL = all
REPSOURCE = Dossier

BACKUP = backup
RESTORE = restore
OBJBACKUP = obj/Archi.o obj/fonctions.o obj/chemin.o 
OBJRESTORE = obj/restore.o obj/fonctions.o obj/chemin.o
OPATH = obj
CPATH = src
BPATH = bin

$(ALL) : $(BACKUP) $(RESTORE)

$(BACKUP) : $(OBJBACKUP)
	$(CC) -o $@ $^ 
	mv $@ $(BPATH)

$(RESTORE) : $(OBJRESTORE)
	$(CC) -o $@ $^ 
	mv $@ $(BPATH)
	
$(OPATH)/%.o : $(CPATH)/%.c
	$(CC) $(CFLAGS) -c $< $(IFLAGS) -o $@


clean : 
	rm -r $(OPATH)/* $(BPATH)/* 

check :
	./$(BPATH)/$(BACKUP) -p 2 $(REPSOURCE) ArchiAvecFork
	./$(BPATH)/$(BACKUP) $(REPSOURCE) ArchiSansFork
	./$(BPATH)/$(RESTORE) -p 6 ArchiAvecFork RestoreAvecForkAAF
	./$(BPATH)/$(RESTORE) -p 2 ArchiSansFork RestoreAvecForkASF
	./$(BPATH)/$(RESTORE) ArchiAvecFork RestoreSansForkAAF
	./$(BPATH)/$(RESTORE) ArchiSansFork RestoreSansForkASF
