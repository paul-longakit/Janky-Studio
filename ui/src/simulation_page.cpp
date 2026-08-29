#include <creature_studio/ui/pages/simulation_page.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace creature_studio
{

SimulationPage::SimulationPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* label = new QLabel("Simulation", this);
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
}

} // namespace creature_studio
