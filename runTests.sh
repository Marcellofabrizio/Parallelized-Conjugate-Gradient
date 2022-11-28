#!/bin/bash

programme="$0"

CG_MPI_C_FILE="./conjugate_gradient_mpi.c"
CG_OMP_C_FILE="./conjugate_gradient_omp.c"
CG_ORI_C_FILE="./conjugate_gradient.c"

CG_MPI_BIN_FILE="./conjugate_gradient_mpi"
CG_OMP_BIN_FILE="./conjugate_gradient_omp"
CG_ORI_BIN_FILE="./conjugate_gradient"

RESULTS_FILE=""
OMP_RESULTS_FILE=""

ORDER_MATRIX=""

TIMESTAMP=$(date '+%T')

TEST_RANGE=30

Help() {

    cat << EOM

    Executa rodada de testes do gradiente conjugado e grava os resultados em
    um arquivo CSV

    usage: $programme <order_of_matrix>

EOM

    exit
}

ParseArgs() {

    runBoth=false
    runMpi=false
    runOmp=false

    while getopts ":b:p:t" flag; do
        case ${flag} in
        b)
            runBoth=true
            ;;
        p)
            runMpi=true
            ;;
        t)
            runOmp=true
            ;;
        \?) # incorrect option
            Help
            ;;
        *) # incorrect option
            Help
            ;;
        esac
    done

    echo $2
    ORDER_MATRIX=$2

    if [ -z "$ORDER_MATRIX" ]; then
        Help
    fi

    if $runBoth; then
        CreateMpiResultFile
        CreateOmpResultFile
        ExecuteMpiTests
        ExecuteOmpTests
    fi

    if $runMpi; then
        CreateMpiResultFile
        ExecuteMpiTests
    fi

    if $runOmp; then
        CreateOmpResultFile
        ExecuteOmpTests
    fi

}

CompileBinaries() {

    if [ -f "$CG_MPI_C_FILE" ]; then
        if [ ! -f "$CG_MPI_BIN_FILE" ]; then
            echo "Compiling Conjugate Gradient MPI Binary..."
            mpicc -o $CG_MPI_BIN_FILE $CG_MPI_C_FILE
            if [ $? -eq 0 ]; then
                echo "Successfully compiled MPI Binary"
            else 
                echo "Error: Failed to compile MPI Binary"
                exit
            fi
        fi
    else 
        echo "Could not find $CG_MPI_C_FILE"
    fi

    if [ -f "$CG_OMP_C_FILE" ]; then
        if [ ! -f "$CG_OMP_BIN_FILE" ]; then
            echo "Compiling Conjugate Gradient OMP Binary..."
            gcc -o $CG_OMP_BIN_FILE $CG_OMP_C_FILE -fopenmp
            if [ $? -eq 0 ]; then
                echo "Successfully compiled OMP Binary"
            else 
                echo "Error: Failed to compile OMP Binary"
                exit
            fi
        fi
    else 
        echo "Could not find $CG_OMP_C_FILE"
    fi

}

CreateMpiResultFile() {
    
    RESULTS_FILE="./results/"$TIMESTAMP"_mpi_"$ORDER_MATRIX"_results.csv"

    touch $RESULTS_FILE

    echo "Número de processos,Tempo,Resultado" > $RESULTS_FILE

}

CreateOmpResultFile() {
    
    OMP_RESULTS_FILE="./results/"$TIMESTAMP"_omp_"$ORDER_MATRIX"_results.csv"

    touch $OMP_RESULTS_FILE

    echo "Número de threads,Tempo,Resultado" > $OMP_RESULTS_FILE

}

ExecuteMpiTests() {

    for i in {1..$TEST_RANGE}
    do
    if [ $(expr $ORDER_MATRIX % $i) -eq 0 ]; then
        echo "Executing test with $i processes..."
        mpirun -np $i --oversubscribe $CG_MPI_BIN_FILE $ORDER_MATRIX >> $RESULTS_FILE
        if [ $? -eq 0 ]; then
            echo "Successfully ran MPI test"
        else
            echo "Error: Failed to run MPI test"
            exit
        fi
    fi
    done

}

ExecuteOmpTests() {

    for i in {1..$TEST_RANGE}
    do
    if [ $(expr $ORDER_MATRIX % $i) -eq 0 ]; then
        echo "Executing test with $i threads..."
        ./$CG_OMP_BIN_FILE $ORDER_MATRIX $i >> $OMP_RESULTS_FILE
        if [ $? -eq 0 ]; then
            echo "Successfully ran OMP test"
        else
            echo "Error: Failed to run OMP test"
            exit
        fi
    fi
    done

}

CompileBinaries
ParseArgs "$@"