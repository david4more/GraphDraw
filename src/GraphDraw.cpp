#include "GraphDraw.h"

double GraphDraw::Graph::x = 0.f;

GraphDraw::GraphDraw(RenderWindow& wnd)
	: window(wnd)
{
	state = appState::draw;

	view = window.getDefaultView();
	view.setCenter({ 0,0 });

	float defZoomFactor = 0.05f;
	view.zoom(defZoomFactor);
	axesThickness *= defZoomFactor;
	pointOffset *= defZoomFactor;
	updateView();

	xAxis.setFillColor(Color::Black);
	yAxis.setFillColor(Color::Black);
	updateUI();

	if (!(file = ifstream("../../../../files/file.graph"))) 
		std::cerr << "Unable to open the file.\n";
	else
		fileWrite = ofstream("../../../../files/file.graph", ios::app);

	const std::unordered_map<std::string, sf::Color> colorMap = {
		{ "Magenta", sf::Color::Magenta },
		{ "Black", sf::Color::Black },
		{ "Red", sf::Color::Red },
		{ "Green", sf::Color::Green },
		{ "Blue", sf::Color::Blue },
		{ "Yellow", sf::Color::Yellow },
		{ "Cyan", sf::Color::Cyan },
	};

	string str;
	string colorLine;
	while (getline(file, str))
	{
		if (str.empty())
			continue;

		auto pos = str.find('c');	// color
		if (str[0] == 't')	// table-type graph
		{
			tableLines.push_back(VertexArray());
			tableLines.back().setPrimitiveType(PrimitiveType::LineStrip);

			str = str.substr(2);

			float x, y;
			while ((pos = str.find(',')) != string::npos)
			{
				x = stof(str.substr(0, pos));
				str = str.substr(pos + 1);
				pos = str.find(';');
				y = stof(str.substr(0, pos));
				str = str.substr(pos + 1);

				Vertex v;
				v.color = defaultColor;
				v.position = { x, y };
				tableLines.back().append(v);
			}
		}
		else if (pos == string::npos)
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

	auto editButtonWidth = viewRect.size.x / 30;
	editButtonSize = { editButtonWidth, editButtonWidth };
	editButtonPos = editButtonSize / 4.f;
	if (!editButtonTexture.loadFromFile("../../../../files/editButton.png")) cerr << "Unable to load edit button icon.\n";
	if (!doneButtonTexture.loadFromFile("../../../../files/doneButton.png")) cerr << "Unable to load done button icon.\n";
	editButton.setSize(editButtonSize);
	editButton.setTexture(&editButtonTexture);
	editButton.setPosition(viewRect.position + editButtonPos);
}

void GraphDraw::pollEvents()
{
	window.handleEvents(
		[this](const Event::Closed& e) { 
			window.close(); },
		[this](const Event::KeyPressed& e) { 
			if (e.scancode == Keyboard::Scancode::Escape) window.close(); },
		[this](const Event::MouseButtonPressed& e) { 
			lmbHeld = true; initMousePos = Mouse::getPosition(window); },
		[this](const Event::MouseButtonReleased& e) { onMouseButtonReleased(e); },
		[this](const Event::MouseWheelScrolled& e) { onMouseWheelScrolled(e); },
		[this](const Event::MouseMoved& e) { onMouseMoved(e); }
	);
}

void GraphDraw::render()
{
	window.clear(Color::White);

	window.draw(xAxis);
	window.draw(yAxis);
	for (const Graph& graph : graphs)
		window.draw(graph.line);

	for (const VertexArray& array : tableLines)
		window.draw(array);

	window.draw(editButton);

	window.display();
}

void GraphDraw::loop()
{
	switch (state)
	{
	case appState::draw:
		pollEvents();
		break;
	case appState::edit:
		window.handleEvents( 
			[this](const Event::MouseButtonReleased& e) { editOnMouseButtonReleased(e); }, 
			[this](const Event::MouseMoved& e) { editOnMouseMoved(e); });
		break;
	}
	render();
}

void GraphDraw::updateView()
{
	viewRect = FloatRect(view.getCenter() - (view.getSize() / 2.f), view.getSize());
	window.setView(view);
}

void GraphDraw::updateUI()
{
	auto initAxis = [&](RectangleShape& axis, const Vector2f& size, const Vector2f& pos) {
		axis.setFillColor(sf::Color::Black);
		axis.setOrigin({ axesThickness / 2, axesThickness / 2 });
		axis.setSize(size);
		axis.setPosition(pos);
		};
	initAxis(xAxis, { viewRect.size.x, axesThickness }, { viewRect.position.x, 0.f });
	initAxis(yAxis, { axesThickness, viewRect.size.y }, { 0.f, viewRect.position.y });
	
	editButton.setSize(editButtonSize);
	editButton.setPosition(viewRect.position + editButtonPos);
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
void GraphDraw::onMouseWheelScrolled(const Event::MouseWheelScrolled& event)
{
	float zoomFactor = (event.delta > 0) ? 0.9f : 1.1f;

	mousePos = event.position;
	Vector2f beforeZoom = window.mapPixelToCoords(mousePos, view);
	view.zoom(zoomFactor);
	Vector2f afterZoom = window.mapPixelToCoords(mousePos, view);
	Vector2f offset = beforeZoom - afterZoom;
	view.move(offset);

	axesThickness *= zoomFactor;
	pointOffset *= zoomFactor;
	editButtonSize *= zoomFactor;
	editButtonPos *= zoomFactor;

	updateView();
	updateUI();
	updateGraphs();
}

void GraphDraw::onMouseMoved(const Event::MouseMoved& event)
{
	if (!lmbHeld)
		return;

	mousePos = event.position;
	auto offset = window.mapPixelToCoords(initMousePos) - window.mapPixelToCoords(mousePos);
	view.move((Vector2f)offset);
	initMousePos = mousePos;

	updateView();
	updateUI();
	updateGraphs();
}

void GraphDraw::onMouseButtonReleased(const Event::MouseButtonReleased& event)
{
	lmbHeld = false; initMousePos = { 0,0 }; 

	if (!editButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(window.mapPixelToCoords(event.position))))
		return;

	state = appState::edit;
	tableLines.push_back(VertexArray());
	tableLines.back().setPrimitiveType(PrimitiveType::LineStrip);

	editButton.setTexture(&doneButtonTexture);
	editButton.setSize(editButtonSize);

	Vertex v;
	v.color = defaultColor;
	v.position = static_cast<sf::Vector2f>(window.mapPixelToCoords(event.position));
	tableLines.back().append(v);
}

// edit mode callbacks
void GraphDraw::editOnMouseMoved(const Event::MouseMoved& event)
{
	auto vertexCount = tableLines.back().getVertexCount();
	if (vertexCount == 0)
		return;

	tableLines.back().resize(vertexCount - 1);
	Vertex v;
	v.color = defaultColor;
	v.position = window.mapPixelToCoords(event.position);
	tableLines.back().append(v);
}

void GraphDraw::editOnMouseButtonReleased(const Event::MouseButtonReleased& event)
{
	mousePos = event.position;
	auto coords = static_cast<sf::Vector2f>(window.mapPixelToCoords(mousePos));
	if (editButton.getGlobalBounds().contains(coords))
	{
		editButton.setTexture(&editButtonTexture);
		tableLines.back().resize(tableLines.back().getVertexCount() - 1);

		state = appState::draw;
	}
	else
	{
		if (tableLines.back().getVertexCount() == 1)
			fileWrite << "\nt ";

		Vertex v;
		v.position = coords;
		v.color = defaultColor;
		fileWrite << std::fixed << std::setprecision(2) << coords.x << ',' << coords.y << ';';

		tableLines.back().append(v);
	}
}
