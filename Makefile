CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET_TEST = test_grafo
TARGET_TEST_DF = test_analise_df

.PHONY: test valgrind clean

$(TARGET_TEST): src/grafo.c tests/test_grafo.c include/grafo.h
	$(CC) $(CFLAGS) src/grafo.c tests/test_grafo.c -o $(TARGET_TEST)

$(TARGET_TEST_DF): src/grafo.c src/analise_df.c tests/test_analise_df.c include/grafo.h include/analise_df.h
	$(CC) $(CFLAGS) src/grafo.c src/analise_df.c tests/test_analise_df.c -o $(TARGET_TEST_DF)

test: $(TARGET_TEST) $(TARGET_TEST_DF)
	./$(TARGET_TEST)
	./$(TARGET_TEST_DF)

valgrind: $(TARGET_TEST) $(TARGET_TEST_DF)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET_TEST)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET_TEST_DF)

clean:
	rm -f $(TARGET_TEST) $(TARGET_TEST).exe $(TARGET_TEST_DF) $(TARGET_TEST_DF).exe *.o
