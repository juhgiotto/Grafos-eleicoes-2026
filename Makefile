CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra -Wdeclaration-after-statement -g -Iinclude
TARGET = eleicoes_grafos
TARGET_TEST = test_grafo
TARGET_TEST_DF = test_analise_df

.PHONY: all test valgrind clean

all: $(TARGET)

$(TARGET): src/main.c src/grafo.c src/analise_df.c src/analise_nacional.c include/grafo.h include/analise_df.h include/analise_nacional.h
	$(CC) $(CFLAGS) src/main.c src/grafo.c src/analise_df.c src/analise_nacional.c -o $(TARGET) -lm

$(TARGET_TEST): src/grafo.c tests/test_grafo.c include/grafo.h
	$(CC) $(CFLAGS) src/grafo.c tests/test_grafo.c -o $(TARGET_TEST)

$(TARGET_TEST_DF): src/grafo.c src/analise_df.c tests/test_analise_df.c include/grafo.h include/analise_df.h
	$(CC) $(CFLAGS) src/grafo.c src/analise_df.c tests/test_analise_df.c -o $(TARGET_TEST_DF) -lm

test: $(TARGET_TEST) $(TARGET_TEST_DF)
	./$(TARGET_TEST)
	./$(TARGET_TEST_DF)

valgrind: $(TARGET_TEST) $(TARGET_TEST_DF)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET_TEST)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET_TEST_DF)

clean:
	rm -f $(TARGET) $(TARGET).exe $(TARGET_TEST) $(TARGET_TEST).exe $(TARGET_TEST_DF) $(TARGET_TEST_DF).exe *.o
