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

#pragma omp parallel for
    for (int i = 0; i < N; ++i)
    {
        double b2 = 0.0;
        double quatroac = 0.0;
        double dois_a = 0.0;
        double delta = 0.0;
        double x1 = 0.0, x2 = 0.0;

        // Usando atomic para cálculo de cada componente
#pragma omp atomic
        b2 += b[i] * b[i];

#pragma omp atomic
        quatroac += 4.0 * a[i] * c[i];

#pragma omp atomic
        dois_a += 2.0 * a[i];

#pragma omp critical
        // Calcula delta
        delta = b2 - quatroac;

        if (delta >= 0)
        {
            x1 = (-b[i] + std::sqrt(delta)) / dois_a;
            x2 = (-b[i] - std::sqrt(delta)) / dois_a;
        }
        else
        {
            x1 = 0.0;
            x2 = 0.0;
        }

        // Soma total e impressão usando critical
#pragma omp critical
        {
            soma_total += x1 + x2;
            std::cout << "Equação " << i + 1 << ": "
                      << a[i] << "x^2 + " << b[i] << "x + " << c[i] << " = 0"
                      << "\n  Raízes: x1=" << x1 << ", x2=" << x2
                      << "\n"
                      << std::endl;
        }
    }

    double T1 = omp_get_wtime();
    double duracao = T1 - T0;

    std::cout << "\nSoma total de todas as raízes: " << soma_total << std::endl;
    std::cout << "Tempo total: " << duracao << " ms" << std::endl;
    std::cout << "----------------------------------------\n";

    return 0;
}
