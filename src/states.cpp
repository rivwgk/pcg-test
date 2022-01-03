#include "states.h"

Grid2DState::Grid2DState(Grid* const g)
 : grid(g)
{
	md.set_size(640, 480);
	md.load_matrix<BiomeType>(grid-> types(), [](const BiomeType& t)
	      { return sf::Color(BiomeColours[static_cast<unsigned>(t)]); });
}
Grid2DState::Grid2DState(const Grid2DState& other)
 : grid(other.grid)
{
	md = other.md;
}
Grid2DState&
Grid2DState::operator=(const Grid2DState& other)
{
	if (&other == this)
		return *this;
	grid = other.grid;
	md = other.md;
	return *this;
}
Grid2DState::~Grid2DState() {}
/* ----------------------------------------------------------------- */
void
Grid2DState::update(sf::RenderWindow& window, const sf::Event& e)
{
	double min, max;
	if (e.type == sf::Event::KeyPressed) {
		switch (e.key.code) {
		case sf::Keyboard::Key::F1:
			min = grid-> heightmap().minCoeff();
			max = grid-> heightmap().maxCoeff();
			md.load_matrix<double>(grid-> heightmap(),
			               [=](double x){ return greyscale(x, min, max); });
			break;
		case sf::Keyboard::Key::F2:
			min = grid-> temperature().minCoeff();
			max = grid-> temperature().maxCoeff();
			md.load_matrix<double>(grid-> temperature(),
			               [=](double x){ return temperature(x, min, max); });
			break;
		case sf::Keyboard::Key::F3:
			min = grid-> humidity().minCoeff();
			max = grid-> humidity().maxCoeff();
			md.load_matrix<double>(grid-> humidity(),
			               [=](double x){ return greyscale(x, min, max); });
			break;
		case sf::Keyboard::Key::F4:
			md.load_matrix<BiomeType>(grid-> types(), [](const BiomeType& t)
			      { return sf::Color(BiomeColours[static_cast<unsigned>(t)]); });
			break;
		default: break;
		}
	}
	md.update();
}
/* ----------------------------------------------------------------- */
void
Grid2DState::draw(sf::RenderWindow& window)
{
	md.draw(window);
}
/* ----------------------------------------------------------------- */
void
Grid2DState::resized(unsigned w,unsigned h)
{
	md.set_size(w, h);
}
/* ----------------------------------------------------------------- */
void
Grid2DState::scrolled(const sf::Event::MouseWheelScrollEvent& ev)
{
	if (ev.wheel == sf::Mouse::Wheel::VerticalWheel)
		md.scale(sf::Vector2f{static_cast<float>(ev.x),
		                      static_cast<float>(ev.y)},
		         ev.delta > 0 ? 2.0 : 0.5);
}
/* ================================================================= */
StateManager::StateManager() {}
StateManager::StateManager(const StateManager& other)
{
	states = other.states;
}
StateManager&
StateManager::operator=(const StateManager& other)
{
	if (&other == this)
		return *this;
	states = other.states;
	return *this;
}

void
StateManager::push(State* state)
{
	states.push_back(state);
}

State*
StateManager::pop()
{
	State* s = states.back();
	states.pop_back();
	return s;
}

const State&
StateManager::top()
const {
	return *states.back();
}

void
StateManager::update(sf::RenderWindow& window)
{
	sf::Event ev;
	while (window.pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			window.setView(sf::View{sf::Rect<float>{0.0f, 0.0f,
			                            static_cast<float>(ev.size.width),
			                            static_cast<float>(ev.size.height)}});
			for (State* s : states)
				s-> resized(ev.size.width, ev.size.height);
			break;
		case sf::Event::LostFocus:
			has_focus = false;
			break;
		case sf::Event::GainedFocus:
			has_focus = true;
			break;
		case sf::Event::KeyReleased:
			if (has_focus) {
				switch (ev.key.code) {
				case sf::Keyboard::Key::Escape:
					window.close();
					break;
				default:
					break;
				}
			} break;
		case sf::Event::MouseWheelScrolled:
			if (has_focus)
				states.back()-> scrolled(ev.mouseWheelScroll);
			break;
		default:
			break;
		}
		if (has_focus)
			states.back()-> update(window, ev);
	}
}

void
StateManager::draw(sf::RenderWindow& window)
const {
	states.back()-> draw(window);
}
