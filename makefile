NAME = "Landing"
all:
	@echo "Compiling..."
	g++ -o $(NAME) $(NAME).cpp -L/usr/X11R6/lib -lX11 -lstdc++

run: all
	@echo "Running..."
	./$(NAME)
