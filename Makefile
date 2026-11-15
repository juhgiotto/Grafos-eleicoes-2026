CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET_TEST = test_grafo

.PHONY: test valgrind clean

$(TARGET_TEST): src/grafo.c tests/test_grafo.c include/grafo.h
	$(CC) $(CFLAGS) src/grafo.c tests/test_grafo.c -o $(TARGET_TEST)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

valgrind: $(TARGET_TEST)
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET_TEST)

clean:
	rm -f $(TARGET_TEST) $(TARGET_TEST).exe *.o
