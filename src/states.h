#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "grid.h"
#include "matrixdisplay.h"

class State {
public:
	State() = default;
	State(const State& other) = default;
	State& operator=(const State& other) = default;
	virtual ~State() = default;

	virtual void update(sf::RenderWindow&, const sf::Event&) = 0;
	virtual void draw(sf::RenderWindow&) = 0;
	virtual void resized(unsigned,unsigned) = 0;
	virtual void scrolled(const sf::Event::MouseWheelScrollEvent&) = 0;
private:
};

class Grid2DState : public State {
public:
	Grid2DState() = delete;
	Grid2DState(Grid* const g);
	Grid2DState(const Grid2DState& other);
	Grid2DState& operator=(const Grid2DState& other);
	~Grid2DState();

	void update(sf::RenderWindow&, const sf::Event&) override;
	void draw(sf::RenderWindow&) override;
	void resized(unsigned,unsigned) override;
	void scrolled(const sf::Event::MouseWheelScrollEvent&) override;
private:
	Grid* grid;
	MatrixDisplay md{};
};

class StateManager {
public:
	StateManager();
	StateManager(const StateManager& other);
	StateManager& operator=(const StateManager& other);

	void push(State* state);
	State* pop();
	const State& top() const;

	void update(sf::RenderWindow&);
	void draw(sf::RenderWindow&) const;
private:
	bool has_focus = true;
	std::vector<State*> states{};
};
