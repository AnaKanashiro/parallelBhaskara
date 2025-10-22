#include <iostream> // Para entrada e saída
#include <vector>   // vetor
#include <cmath>    // para operações matemátias
#include <omp.h>    // openMp API.

// Função para resolver uma equação de segundo grau usando Bhaskara
// e retornar a soma das raízes (ou 0 se não houver raízes reais).
double resolver_bhaskara(double a, double b, double c)
{
    // Calcula delta
    double delta = (b * b) - (4 * a * c);

    // Se o delta for negativo, não existem raízes reais.
    if (delta < 0)
    {
        return 0.0;
    }
    // // Calcula as duas raízes
 
    double x1 = (-b + std::sqrt(delta)) / (2 * a);

    double x2 = (-b - std::sqrt(delta)) / (2 * a);
  
    return x1 + x2;
//opcao de paralelismo por lock dentro do metodo:
//para isso precisa se cirar um ambiente paralelo omp
//ai se pode paralelizar a bhaskara e usar lock para somar as raizes
}

int main()
{
    int N;
    std::cout << "Digite o número de equações a serem resolvidas: ";
    std::cin >> N;

    std::vector<double> a(N), b(N), c(N);

    srand(time(0)); // inicializa a semente com o tempo atual

    int num = rand() % 21 - 10; // gera número entre -10 e 10

    // Preenche os coeficientes
    for (int i = 0; i < N; ++i)
    {
        b[i] = rand() % 21 - 10;
        ; // aleatório entre -10 e 10
        c[i] = rand() % 21 - 10;
        ; // aleatório entre -10 e 10
        if (i % 2 == 0)
        {
            a[i] = 1.0;
        }
        else
        {
            a[i] = -1.0;
        }
    }

    double soma_total = 0.0;
    double T0 = omp_get_wtime(); // tempo inicial

    omp_lock_t lock_soma;

    omp_init_lock(&lock_soma);

// Inicia a região paralela. O loop for será dividido entre as threads.
// A variável 'soma_total' é compartilhada por todas as threads.
#pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        // Cada thread calcula a soma das raízes para uma equação.
        double soma_local = resolver_bhaskara(a[i], b[i], c[i]);

        omp_set_lock(&lock_soma);
        soma_total += soma_local;
        omp_unset_lock(&lock_soma);

#pragma omp critical // para visualizar

        {
            std::cout << "Equação " << i + 1 << ": "
                      << a[i] << "x^2 + " << b[i] << "x + " << c[i] << " = 0"
                      << "\n"
                      << std::endl;
        }
    }

    double T1 = omp_get_wtime();
    double duracao = T1 - T0;

    omp_destroy_lock(&lock_soma);
    std::cout << "Soma total (com lock): " << soma_total << std::endl;
    std::cout << "Tempo total: " << duracao << " ms" << std::endl;

    return 0;
}
