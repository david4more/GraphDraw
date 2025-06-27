#include "GraphDraw.h"

int main()
{
    RenderWindow window(VideoMode({ 800, 600 }), "GraphDraw", Style::Default);
    window.setFramerateLimit(240);

    GraphDraw app(window);

    while (window.isOpen())
    {
        app.pollEvents();
        app.update();
        app.render();
    }

    return 0;
}