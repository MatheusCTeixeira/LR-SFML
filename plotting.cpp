#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

// Estrutura para representar os coeficientes da regressão linear.
struct LRCoefficients {
    float b1;
    float b0;
};

// Variaveis globais.
sf::RenderWindow window{sf::VideoMode{600, 600}, "Screen"};
sf::View view{{0, 0}, {200, -200}};


// Obtém o ponto médio do vetor.
sf::Vector2f getAverage(std::vector<sf::Vector2f> points) {
    // Acumuladores.
    float xAcc = 0;
    float yAcc = 0;

    for (auto point: points) {
        xAcc += point.x;
        yAcc += point.y;
    }

    return sf::Vector2f{xAcc/points.size(), yAcc/points.size()};
}

// Calcula os parâmetros da regressão linear a partir dos vetores de pontos.
LRCoefficients getLRCoefficients(std::vector<sf::Vector2f> points) {
    sf::Vector2f averages = getAverage(points);
    float xAvg = averages.x;
    float yAvg = averages.y;

    float numAcc = 0, denAcc = 0;

    // Veja https://en.wikipedia.org/wiki/Regression_analysis#Linear_regression
    // para compreender as equações.
    for (auto point: points) {
        numAcc += ((point.x - xAvg)*(point.y - yAvg));
        denAcc += pow(point.x - xAvg, 2);
    }

    float b1 = numAcc / denAcc;
    float b0 = yAvg - b1 * xAvg;

    return {.b1 = b1, .b0 = b0}; // Usando o construtor como a sintaxe
                                 // designator.

}

// Gera ruídos.
float getNoise() {
    return (rand() % 30);
}

// Plota os pontos do vetor.
void drawPoints(std::vector<sf::Vector2f> points, sf::RenderWindow& window) {
    const sf::Vector2f size = window.getView().getSize();
    const int n = points.size();
    const float dx = size.x / n;
    const float dy = -size.y / n;

    for (auto pos: points) {
        sf::CircleShape point{1};
        point.setFillColor(sf::Color::Yellow);
        point.setOutlineThickness(0.5);
        point.setOutlineColor(sf::Color::Black);

        point.setPosition({pos.x, pos.y});
        const float radius = point.getRadius();
        point.move(-radius, -radius);

        window.draw(point);
    }
}

// Plota um reta com base nos coeficientes do parâmetro.
// Ex: coeff = {a, b} ==> Y = b × X + a.
void drawLine(LRCoefficients coeff, sf::RenderWindow& window) {
    auto b1 = coeff.b1, b0 = coeff.b0;

    // A função que representa a reta.
    auto f = [=](float x) -> float{ return b0 + b1 * x; };

    // Este método cria uma linha a partir de um retângulo. A vantagem é que a
    // a espessura da linha pode ser modificada, porém exige um pouco de conhe-
    // cimento em álgebra linear.
    sf::RectangleShape line{{400, 2}};
    line.setFillColor(sf::Color::Black);
    line.rotate(180 * atan(coeff.b1)/ M_PI);
    line.move(-100, f(-100) - line.getSize().y / 2);
    window.draw(line);

    // Apenas utilizando uma linha. Neste método não é possível aumentar a es-
    // pessura da linha. A vantagem é que é mais simples.
    sf::Vertex vertices[] = {
        {sf::Vector2f{-100, f(-100)}, sf::Color::Red},
        {sf::Vector2f{+100, f(+100)}, sf::Color::Red},
    };
    window.draw(vertices, 2, sf::Lines);
}


// Uma função para modular o dataset.
auto f(float x) -> float {
    const double a = (double)(rand() % 500) / 500;
    const double b = (double)(rand() % 400) / 10;
    return a * x + 5;
}

// Gera o dataset de pontos. Os pontos são gerados com base na função moduladora
// somado ao ruído.
std::vector<sf::Vector2f> generateDataset() {
    std::vector<sf::Vector2f> points{};

    for (int i = 0; i < 200; ++i) {
        float x = i - 100;
        float noise = getNoise();

        points.push_back({x, f(x) + noise});
    }

    return points;
}

int main() {
    srand(time(nullptr));
    window.setView(view);

    // Gera o vetor e calcula os coeficientes.
    auto dataset = generateDataset();
    auto coeff = getLRCoefficients(dataset);




    while (window.isOpen()) {
        sf::Event event;

        // Pool de eventos.
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Return) {
                    dataset = generateDataset();
                    coeff = getLRCoefficients(dataset);
                }

            default:
                ;
            }
        }

        // Exibe o resultado.
        window.clear(sf::Color::White);

        drawPoints(dataset, window);
        drawLine(coeff, window);

        window.display();

    }

    return 0;
}