#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <random>

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

    // Vetor para armazenar o tempo gasto por thread
    double T0 = omp_get_wtime(); // tempo inicial

#pragma omp parallel
{
    // Vetores locais para armazenar raízes
    std::vector<double> local_x1(N), local_x2(N);

    // Fase 1: cálculo das raízes
    #pragma omp for
    for (int i = 0; i < N; ++i)
    {
        double b2 = 0.0;
        double quatroac = 0.0;
        double dois_a = 0.0;
        double delta = 0.0;

        // Usando atomic para cálculo de cada componente
        #pragma omp atomic
        b2 += b[i] * b[i];

        #pragma omp atomic
        quatroac += 4.0 * a[i] * c[i];

        #pragma omp atomic
        dois_a += 2.0 * a[i];

        #pragma omp critical //uso de critical por ser uma area mais complexa
        // Calcula delta
        delta = b2 - quatroac;

        if (delta >= 0)
        {
            local_x1[i] = (-b[i] + std::sqrt(delta)) / dois_a;
            local_x2[i] = (-b[i] - std::sqrt(delta)) / dois_a;
        }
        else
        {
            local_x1[i] = 0.0;
            local_x2[i] = 0.0;
        }
    }

    // Sincroniza todas as threads antes de somar e imprimir
    #pragma omp barrier

    // Fase 2: soma e impressão
    #pragma omp for
    for (int i = 0; i < N; ++i)
    {
        #pragma omp critical
        {
            soma_total += local_x1[i] + local_x2[i];
            std::cout << "Equação " << i + 1 << ": "
                      << a[i] << "x^2 + " << b[i] << "x + " << c[i] << " = 0"
                      << "\n  Raízes: x1=" << local_x1[i] << ", x2=" << local_x2[i]
                      << "\n"
                      << std::endl;
        }
    }
}
    double T1 = omp_get_wtime();
    double duracao = T1 - T0;

    std::cout << "\nSoma total de todas as raízes: " << soma_total << std::endl;
    std::cout << "Tempo total: " << duracao << " ms" << std::endl;
    std::cout << "----------------------------------------\n";

    return 0;
}
