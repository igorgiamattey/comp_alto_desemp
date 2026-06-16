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

LAST_DIR_FILE := .last_dir
compile-ocl:
	@gcc $(file) -lOpenCL -O3
	@echo $(dir $(file)) > $(LAST_DIR_FILE)
	@clear
	@echo "Compiled Successfully!"

run-omp:
	@./$(OUTPUT) $(RUN_ARGS)

NP ?= 4

run-mpi:
	@mpirun -np $(NP) --oversubscribe ./$(OUTPUT) $(RUN_ARGS)

run-ocl:
	@cd $$(cat $(LAST_DIR_FILE)) && $(CURDIR)/$(OUTPUT) $(RUN_ARGS)