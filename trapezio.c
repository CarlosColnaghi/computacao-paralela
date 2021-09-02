#include<stdio.h>
#include<mpi.h>

double trapezio(double limite_esq, double limite_dir, int numero_trap, double tam_altura);

double f(double x);

int main(void){
    int meu_rank, comm_sz, n = 1024, local_n;
    double a = -2.0, b = 2.0, h, local_a, local_b;
    double local_int, total_int;
    int origem;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    h = (b - a)/n;
    local_n = n/comm_sz;
    local_a = a + meu_rank*local_n*h;
    local_b = local_a + local_n*h;
    local_int = trapezio(local_a, local_b, local_n, h);

    if (meu_rank != 0){
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }else{
        total_int = local_int;
        for (origem = 1; origem < comm_sz; origem++){
            MPI_Recv(&local_int, 1, MPI_DOUBLE, origem, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }

    if (meu_rank == 0){
        printf("Com n = %d trapezios, nossa estimativa\n", n);
        printf("da integral de %f ate %f = %.15e\n", a, b, total_int);
    }

    MPI_Finalize();
    
    return 0;

}

double trapezio(double limite_esq, double limite_dir, int numero_trap, double tam_altura){
    double estimativa, x;
    int i;

    estimativa = (f(limite_esq) + f(limite_dir))/2.0;
    for (i = 1; i <= numero_trap-1; i++){
        x = limite_esq + i*tam_altura;
        estimativa += f(x);
    }
    estimativa *= tam_altura;

    return estimativa;

}

double f(double x){
    return -(x*x) + 4;
}