#include "cvrp/graphics_utils.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <tuple>
#include <vector>

void UpdateColours(std::vector<int>& colours) {
  for (auto& colour : colours) {
    colour += 25;
    colour %= 255;
  }
}

void DrawCoordinates(
    const std::vector<std::vector<std::tuple<int, int, int>>>& all_coords) {
  const int w_size = 1000;
  sf::RenderWindow window(sf::VideoMode(w_size, w_size), "Solution");
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::KeyPressed) {
        window.close();
      }
    }
    window.clear();
    const auto scale = w_size / 50;
    std::vector<int> colours{50, 50, 50};
    for (const auto& coords : all_coords) {
      for (size_t i = 0; i < coords.size() - 1; i++) {
        sf::Text text;
        text.setString(std::to_string(std::get<2>(coords[i])));
        text.setCharacterSize(10);
        text.setFillColor(sf::Color::Red);
        text.setPosition(scale * std::get<0>(coords[i]) + w_size / 2,
                         scale * std::get<1>(coords[i]) + w_size / 2);
        window.draw(text);
        sf::Vertex line[] = {
            sf::Vertex(
                sf::Vector2f(scale * std::get<0>(coords[i]) + w_size / 2,
                             scale * std::get<1>(coords[i]) + w_size / 2)),
            sf::Vertex(
                sf::Vector2f(scale * std::get<0>(coords[i + 1]) + w_size / 2,
                             scale * std::get<1>(coords[i + 1]) + w_size / 2))};
        line[0].color = sf::Color(colours[0], colours[1], colours[2]);
        line[1].color = sf::Color(colours[0], colours[1], colours[2]);
        window.draw(line, 2, sf::Lines);
      }
      UpdateColours(colours);
    }
    window.display();
  }
}

void DisplaySolution(const Solution& s) {
  const auto& nodes = s.GetNodes();
  const auto& vehicles = s.GetVehicles();

  std::vector<std::vector<std::tuple<int, int, int>>> all_coords;
  for (const auto& v : vehicles) {
    std::vector<std::tuple<int, int, int>> coords;
    for (const auto node_id : v.nodes_) {
      const auto it = std::find_if(
          std::begin(nodes), std::end(nodes),
          [node_id](const auto& node) { return node.id_ == node_id; });
      // TODO(vss): Why does this not work:
      // coords.push_back(std::make_tuple<int, int, size_t>(it->x_, it->y_,
      // node_id));
      coords.push_back(std::make_tuple(it->x_, it->y_, node_id));
    }
    all_coords.push_back(coords);
  }
  DrawCoordinates(all_coords);
}
