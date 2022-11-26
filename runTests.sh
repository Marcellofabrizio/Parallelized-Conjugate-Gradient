#!/bin/bash

programme="$0"

CG_MPI_C_FILE="./conjugate_gradient_mpi.c"
CG_ORI_C_FILE="./conjugate_gradient.c"

CG_MPI_BIN_FILE="./conjugate_gradient_mpi"
CG_ORI_BIN_FILE="./conjugate_gradient"

RESULTS_FILE=""

ORDER_MATRIX=""

Help() {

    cat << EOM

    Executa rodada de testes do gradiente conjugado e grava os resultados em
    um arquivo CSV

    usage: $programme <order_of_matrix>

EOM

    exit
}

ParseArgs() {

    ORDER_MATRIX=$1

    if [ -z "$ORDER_MATRIX" ]; then
        Help
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

}

CreateResultFile() {
    
    timeStamp=$(date '+%T')
    RESULTS_FILE="./"$timeStamp"_"$ORDER_MATRIX"_results.csv"

    touch $RESULTS_FILE
    
    echo "Número de processos,Tempo,Resultado" > $RESULTS_FILE

}

ExecuteTests() {

    for i in {1..30}
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

ParseArgs "$@"
CompileBinaries
CreateResultFile
ExecuteTests