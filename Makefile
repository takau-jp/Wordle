all:
	${MAKE} init
	${MAKE} run

init:
	npm install

clean:
	rm -f dist/*

fclean: clean
	rm -rf dist
	rm -rf node_modules

re:
	${MAKE} fclean
	${MAKE} all

run:
	npm run build
	@-pkill -f "http.server 8080"
	@echo "\n\033[1;32mhttp://localhost:8080/src/\033[0m\n"
	python3 -m http.server 8080

.PHONY: all clean fclean re run
