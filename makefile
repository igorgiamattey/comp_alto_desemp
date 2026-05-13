OUTPUT=a.out

RUN_ARGS := $(filter-out run-mpi run-omp, $(MAKECMDGOALS))

%:
	@:

compile-omp:
	@gcc $(file) -fopenmp -O3
	@clear
	@echo "Compiled Successfully!"

compile-mpi:
	@mpicxx $(file) -O3
	@clear
	@echo "Compiled Successfully!"

run-omp:
	@./$(OUTPUT) $(RUN_ARGS)

NP ?= 4

run-mpi:
	@mpirun -np $(NP) --oversubscribe ./$(OUTPUT) $(RUN_ARGS)