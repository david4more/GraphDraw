#pragma once
#include <iostream>
#include <fstream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <muParser.h>
using namespace sf;
using namespace std;

class GraphDraw
{
public:
	GraphDraw(RenderWindow& wnd);
	void loop();

private:
	void update();
	void render();
	enum appState { draw, edit } state;
	RenderWindow& window;

	RectangleShape xAxis, yAxis;
	vector<VertexArray> grid;
	float idealLines = 10.f;
	//vector<Number> numbers;
	//float fontSize = 1.f;
	//Font font;

	float axesThickness = 2.f;
	void updateUI();
	void updateGrid();

	bool lmbHeld = false;		// left mouse button
	Vector2i mousePos, initMousePos;

	View view;
	FloatRect viewRect;
	void updateView();

	float pointOffset = 1.f;
	ifstream file;
	struct Graph
	{
		static double x;

		Graph(string str, Color color) : color(color) {
			parser.DefineVar("x", &x);
			parser.SetExpr(str);
			line.setPrimitiveType(PrimitiveType::LineStrip);
		}

		mu::Parser parser;
		Color color;
		VertexArray line;
	};

	vector<Graph> graphs;
	void updateGraphs();
	Color defaultColor = Color::Black;

	Texture editButtonTexture, doneButtonTexture;
	RectangleShape editButton;
	Vector2f editButtonSize = { 200.f, 200.f };
	Vector2f editButtonPos;
	vector<VertexArray> tableLines;

	ofstream fileWrite;
	void editOnMouseButtonReleased(const Event::MouseButtonReleased& event);
	void editOnMouseMoved(const Event::MouseMoved& event);

	// event callbacks
	void onMouseButtonReleased(const Event::MouseButtonReleased& event);
	void onMouseWheelScrolled(const Event::MouseWheelScrolled& event);
	void onMouseMoved(const Event::MouseMoved& event);
};