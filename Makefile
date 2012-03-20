build:
	gcc -g -fopenmp -lm -o random-walk main.c 

tests:
	gcc -fopenmp -lm -o random-walk main.c 

	for t in 1 2 3 4 5 6 7 8 9 10; do \
		export OMP_NUM_THREADS=$$t ; \
		/usr/bin/time --format="$$t %e" ./random-walk ; \
	done	