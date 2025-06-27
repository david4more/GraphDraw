#pragma once

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <muParser.h>
using namespace sf;
using namespace std;

class GraphDraw
{
public:
	GraphDraw(RenderWindow& wnd);
	void pollEvents();
	void update();
	void render();

private:
	RenderWindow& window;
	RectangleShape xAxis, yAxis;
	const float axesThickness = 2.f;
	void updateAxis();

	bool lmbHeld = false;		// left mouse button
	Vector2i mousePos, initMousePos;

	View view;
	FloatRect viewRect;
	void updateView();

	float pointOffset = 0.5f;
	ifstream file;
	static float x;
	struct Graph
	{
		Graph(string str, Color color) : color(color) {

			expression.SetExpr(str);
		}

		mu::Parser expression;
		Color color;
		VertexArray line;
	};
	vector<Graph> graphs;
	void updateGraphs();

	// event callbacks
	void onKeyPressed(const Event::KeyPressed& event);
	void onMouseButtonPressed(const Event::MouseButtonPressed& event);
	void onMouseButtonReleased(const Event::MouseButtonReleased& event);
};