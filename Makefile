CC = g++
EDL= g++
EXE=Parking
LIB_PATH=-L /public/tp/tp-multitache/
LIB_TP=-ltp
LIB_NCURSES=-lncurses
LIB_TCL=-ltcl
CPPFLAGS=-c -Wall

CLEAN=clean

OBJ_DIR= bin
SRC_DIR= src

PARKING_OBJ = $(OBJ_DIR)/Mere.o\
		$(OBJ_DIR)/Clavier.o\
		$(OBJ_DIR)/Entree.o\
		$(OBJ_DIR)/Configuration.o\
		$(OBJ_DIR)/Sortie.o			

$(EXE): $(PARKING_OBJ)
	$(EDL) -o $(EXE) $(PARKING_OBJ) $(LIB_PATH) $(LIB_NCURSES) $(LIB_TCL) $(LIB_TP) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
		$(CC) $(CPPFLAGS) $< -o $@

$(CLEAN):
	rm -f $(OBJ_DIR)/*.o $(EXE) core


		
	



