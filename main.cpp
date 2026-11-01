#include <QApplication>
#include <QTimer>
#include "game_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GameWindow* window = new GameWindow;
    window->show();

    QTimer::singleShot(0, window, &GameWindow::askPlayerName);

    return app.exec();
}
