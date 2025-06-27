#include "GraphDraw.h"

double GraphDraw::Graph::x = 0.f;

GraphDraw::GraphDraw(RenderWindow& wnd)
	: window(wnd)
{
	view = window.getDefaultView();
	view.setCenter({ 0,0 });
	updateView();

	xAxis.setFillColor(Color::Black);
	yAxis.setFillColor(Color::Black);
	updateAxis();

	if (!(file = ifstream("../../../../file.graphs")))
		std::cerr << "Unable to open the file.\n";

	const std::unordered_map<std::string, sf::Color> colorMap = {
		{ "Black", sf::Color::Black },
		{ "White", sf::Color::White },
		{ "Red", sf::Color::Red },
		{ "Green", sf::Color::Green },
		{ "Blue", sf::Color::Blue },
		{ "Yellow", sf::Color::Yellow },
		{ "Magenta", sf::Color::Magenta },
		{ "Cyan", sf::Color::Cyan },
		{ "Transparent", sf::Color::Transparent }
	};

	string str;
	string colorLine;
	while (getline(file, str))
	{
		auto pos = str.find('c');
		if (pos == string::npos)
			graphs.push_back(Graph(str, defaultColor));
		else
		{
			colorLine = str.substr(pos + 1);
			str = str.substr(0, pos - 1);

			Color graphColor = (colorMap.find(colorLine) == colorMap.end()) ? defaultColor : colorMap.at(colorLine);

			graphs.push_back(Graph(str, graphColor));
		}
	}

	updateGraphs();
}

void GraphDraw::pollEvents()
{
	window.handleEvents(
		[this](const Event::Closed& e) { window.close(); },
		[this](const Event::KeyPressed& e) { onKeyPressed(e); },
		[this](const Event::MouseButtonPressed& e) { onMouseButtonPressed(e); },
		[this](const Event::MouseButtonReleased& e) { onMouseButtonReleased(e); },
		[this](const Event::MouseWheelScrolled& e) { onMouseWheelScrolled(e); }
	);
}

void GraphDraw::update()
{
	if (lmbHeld)
	{
		mousePos = Mouse::getPosition(window);
		auto offset = window.mapPixelToCoords(initMousePos) - window.mapPixelToCoords(mousePos);
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

void GraphDraw::updateView()
{
	viewRect = FloatRect(view.getCenter() - (view.getSize() / 2.f), view.getSize());
	window.setView(view);
}

void GraphDraw::updateAxis()
{
	auto initAxis = [&](RectangleShape& axis, const Vector2f& size, const Vector2f& pos) {
		axis.setFillColor(sf::Color::Black);
		axis.setOrigin({ axesThickness / 2, axesThickness / 2 });
		axis.setSize(size);
		axis.setPosition(pos);
		};
	initAxis(xAxis, { viewRect.size.x, axesThickness }, { viewRect.position.x, 0.f });
	initAxis(yAxis, { axesThickness, viewRect.size.y }, { 0.f, viewRect.position.y });
}

void GraphDraw::updateGraphs()
{
	for (Graph& graph : graphs)
	{
		graph.line.clear();
		Graph::x = viewRect.position.x;
		float rightEdge = viewRect.position.x + viewRect.size.x + pointOffset;

		while (Graph::x <= rightEdge)
		{
			Vertex v;
			v.position = Vector2f(Graph::x, -graph.parser.Eval());

			v.color = graph.color;
			graph.line.append(v);

			Graph::x += pointOffset;
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

void GraphDraw::onMouseWheelScrolled(const Event::MouseWheelScrolled& event)
{
	float zoomFactor = (event.delta > 0) ? 0.9f : 1.1f;

	Vector2i pixelPos = Mouse::getPosition(window);
	Vector2f beforeZoom = window.mapPixelToCoords(pixelPos, view);

	view.zoom(zoomFactor);
	axesThickness *= zoomFactor;
	pointOffset *= zoomFactor;

	Vector2f afterZoom = window.mapPixelToCoords(pixelPos, view);
	Vector2f offset = beforeZoom - afterZoom;
	view.move(offset);

	updateView();
	updateAxis();
	updateGraphs();
}
