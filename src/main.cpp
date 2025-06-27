#include "GraphDraw.h"

int main()
{
    RenderWindow window(VideoMode({ 800, 600 }), "GraphDraw", Style::Default);
    window.setFramerateLimit(240);

    GraphDraw app(window);

    while (window.isOpen())
        app.loop();

    return 0;
}