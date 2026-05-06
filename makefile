OUTPUT=a.out

RUN_ARGS := $(filter-out run, $(MAKECMDGOALS))

%:
	@:

compile-omp:
	@gcc $(file) -fopenmp -O3
	@echo "Compiled Successfully!"

compile-mpi:
	@mpicxx $(file) -O3
	@echo "Compiled Successfully!"

run-omp:
	@./$(OUTPUT) $(RUN_ARGS)

NP ?= 4

run-mpi:
	@mpirun -np $(NP) --oversubscribe ./$(OUTPUT) $(RUN_ARGS)