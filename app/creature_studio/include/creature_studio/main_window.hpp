#pragma once

#include <creature_studio/domain/creature.hpp>

#include <QMainWindow>

class QStackedWidget;

namespace creature_studio
{

class MainWindow final : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    domain::Creature m_creature;
    QStackedWidget* m_pages;
};

} // namespace creature_studio
