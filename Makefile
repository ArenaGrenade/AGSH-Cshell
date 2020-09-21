TARGET=agsh

build: # compiles all the C files as well as headers
	@gcc main.c -o $(TARGET)
	@chmod +x $(TARGET)
run: # runs the built shell
	@./$(TARGET)
debug: #runs a gdb on the run from the begining of the shell
	@gcc -g main.c -o $(TARGET)
	@gdb $(TARGET)
clean: # removes built shell
	@rm $(TARGET)
