#include "GraphDraw.h"

float GraphDraw::x = 0.f;

GraphDraw::GraphDraw(RenderWindow& wnd)
	: window(wnd)
{
	view = window.getDefaultView();
	view.setCenter({ 0,0 });
	updateView();

	auto initAxis = [&](RectangleShape& axis, const Vector2f& size) {
		axis.setFillColor(sf::Color::Black);
		axis.setOrigin({ axesThickness / 2, axesThickness / 2 });
		axis.setSize(size);
		};
	initAxis(xAxis, { viewRect.size.x, axesThickness });
	initAxis(yAxis, { axesThickness, viewRect.size.y });
	updateAxis();

	//if (!(file = ifstream("graphs.txt")))
		//std::cerr << "Unable to open the file.\n";


	graphs.push_back(Graph("3 + 2 * 4", Color::Green));
	string str;
	while (getline(file, str))
	{
		graphs.push_back(Graph(str, Color::Green));
	}

	updateGraphs();
}

void GraphDraw::pollEvents()
{
	window.handleEvents(
		[this](const Event::Closed& e) { window.close(); },
		[this](const Event::KeyPressed& e) { onKeyPressed(e); },
		[this](const Event::MouseButtonPressed& e) { onMouseButtonPressed(e); },
		[this](const Event::MouseButtonReleased& e) { onMouseButtonReleased(e); }
	);
}

void GraphDraw::update()
{
	if (lmbHeld)
	{
		mousePos = Mouse::getPosition(window);
		auto offset = initMousePos - mousePos;
		view.move((Vector2f)offset);
		initMousePos = mousePos;

		updateView();
		updateAxis();

		updateGraphs();
	}
}

void GraphDraw::render()
{
	window.clear(Color::White);

	window.draw(xAxis);
	window.draw(yAxis);
	for (const Graph& graph : graphs)
		window.draw(graph.line);

	window.display();
}

void GraphDraw::updateAxis()
{
	xAxis.setPosition({ viewRect.position.x, 0.f });
	yAxis.setPosition({ 0.f, viewRect.position.y });
}

void GraphDraw::updateView()
{
	viewRect = FloatRect(view.getCenter() - (view.getSize() / 2.f), view.getSize());
	window.setView(view);
}

void GraphDraw::updateGraphs()
{
	int steps = static_cast<int>(viewRect.size.x / pointOffset);
	for (Graph& graph : graphs)
	{
		graph.line.clear();

		x = viewRect.position.x;

		for (int i = 0; i <= steps; i++)
		{
			Vertex v;
			v.position = Vector2f(x, -graph.expression.Eval()); // Update to use Eval
			v.color = graph.color;
			graph.line.append(v);

			x += pointOffset;
		}
	}
}

// event callbacks
void GraphDraw::onKeyPressed(const Event::KeyPressed& event)
{
	if (event.scancode == Keyboard::Scancode::Escape)
		window.close();
}

void GraphDraw::onMouseButtonPressed(const Event::MouseButtonPressed& event)
{
	lmbHeld = true;
	initMousePos = Mouse::getPosition(window);
}

void GraphDraw::onMouseButtonReleased(const Event::MouseButtonReleased& event)
{
	lmbHeld = false;
	initMousePos = { 0,0 };
}