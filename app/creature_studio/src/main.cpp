#include <creature_studio/main_window.hpp>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    creature_studio::MainWindow window;
    window.show();

    return application.exec();
}