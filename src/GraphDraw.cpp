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
		if (str[0] == '/')
			continue;
		if (str[0] == '#')
			break;

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

	if (!font.openFromFile("../../../../files/font.ttf")) cerr << "Unable to load font.\n";
}

void GraphDraw::update()
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
	for (const Graph& graph : graphs) window.draw(graph.line);

	for (const VertexArray& array : tableLines) window.draw(array);

	for (const VertexArray& line : grid) window.draw(line);

	for (const Text& number : numbers) window.draw(number);

	window.draw(editButton);

	window.display();
}

void GraphDraw::loop()
{
	switch (state)
	{
	case appState::draw:
		update();
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
		axis.setOrigin({ axesThickness / 2, axesThickness / 2 });
		axis.setSize(size);
		axis.setPosition(pos);
		};
	initAxis(xAxis, { viewRect.size.x, axesThickness }, { viewRect.position.x, 0.f });
	initAxis(yAxis, { axesThickness, viewRect.size.y }, { 0.f, viewRect.position.y });

	editButton.setSize(editButtonSize);
	editButton.setPosition(viewRect.position + editButtonPos);

	updateGrid();
}

void GraphDraw::updateGrid()
{
	grid.clear();
	numbers.clear();

	float rawStepX = viewRect.size.x / idealLines;
	float rawStepY = viewRect.size.y / idealLines;

	auto niceStep = [](float rawStep) {
		float power = std::floor(std::log10(rawStep));
		float base = std::pow(10.f, power);
		float fraction = rawStep / base;

		if (fraction < 2.f)      return 1.f * base;
		else if (fraction < 5.f) return 2.f * base;
		else                    return 5.f * base;
		};

	// Use the smaller nice step for both axes to ensure squares
	float step = std::min(niceStep(rawStepX), niceStep(rawStepY));

	float startX = std::floor(viewRect.position.x / step) * step;
	float endX = viewRect.position.x + viewRect.size.x;

	float startY = std::floor(viewRect.position.y / step) * step;
	float endY = viewRect.position.y + viewRect.size.y;

	// Offset for text to avoid overlap with axes
	const float textOffset = 5.0f * step / idealLines;

	// Vertical grid lines and X numbers
	for (float x = startX; x < endX; x += step)
	{
		VertexArray line(PrimitiveType::LineStrip, 2);
		line[0].position = { x, viewRect.position.y };
		line[1].position = { x, viewRect.position.y + viewRect.size.y };
		line[0].color = line[1].color = Color(200, 200, 200, 150);

		grid.push_back(line);

		numbers.push_back(Text(font, to_string(static_cast<int>(x)), defFontSize));
		numbers.back().setScale({ fontSize, fontSize });
		numbers.back().setFillColor(Color::Black);
		numbers.back().setPosition({ x, 0 });
	}

	// Horizontal grid lines and Y numbers
	for (float y = startY; y < endY; y += step)
	{
		VertexArray line(PrimitiveType::LineStrip, 2);
		line[0].position = { viewRect.position.x, y };
		line[1].position = { viewRect.position.x + viewRect.size.x, y };
		line[0].color = line[1].color = Color(200, 200, 200, 150);

		grid.push_back(line);

		numbers.push_back(Text(font, to_string(static_cast<int>(-y)), defFontSize));
		numbers.back().setScale({ fontSize, fontSize });
		numbers.back().setFillColor(Color::Black);
		numbers.back().setPosition({ 0, y });
	}
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
	fontSize *= zoomFactor;

	updateView();
	updateUI();
	updateGraphs();
	cerr << fontSize << '\n';
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
