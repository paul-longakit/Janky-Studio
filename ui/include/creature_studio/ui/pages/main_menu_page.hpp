#pragma once

#include <QWidget>

namespace creature_studio
{

class MainMenuPage final : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuPage(QWidget* parent = nullptr);

signals:
    void createCreatureRequested();
    void openCreatureRequested();
    void createArenaRequested();
    void simulatorRequested();
};
} // namespace creature_studio
