#include <creature_studio/main_window.hpp>

#include <creature_studio/ui/pages/arena_editor_page.hpp>
#include <creature_studio/ui/pages/creature_editor_page.hpp>
#include <creature_studio/ui/pages/main_menu_page.hpp>
#include <creature_studio/ui/pages/simulation_page.hpp>
#include <creature_studio/ui/pages/state_editor_page.hpp>

#include <QStackedWidget>

namespace creature_studio
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_pages(new QStackedWidget(this))
{
    setWindowTitle("Janky Studio");
    resize(1280, 720);

    auto* mainMenu = new MainMenuPage(m_pages);
    auto* creatureEditor = new CreatureEditorPage(m_creature, m_pages);
    auto* stateEditor = new StateEditorPage(m_creature, m_pages);
    auto* arenaEditor = new ArenaEditorPage(m_pages);
    auto* simulation = new SimulationPage(m_pages);

    m_pages->addWidget(mainMenu);
    m_pages->addWidget(creatureEditor);
    m_pages->addWidget(stateEditor);
    m_pages->addWidget(arenaEditor);
    m_pages->addWidget(simulation);

    connect(
        mainMenu,
        &MainMenuPage::createCreatureRequested,
        this,
        [this, creatureEditor]()
        {
            m_pages->setCurrentWidget(creatureEditor);
        }
    );

    connect(
        creatureEditor,
        &CreatureEditorPage::stateEditorRequested,
        this,
        [this, stateEditor]()
        {
            m_pages->setCurrentWidget(stateEditor);
        }
    );

    connect(
        stateEditor,
        &StateEditorPage::backRequested,
        this,
        [this, creatureEditor]()
        {
            m_pages->setCurrentWidget(creatureEditor);
        }
    );

    connect(
        creatureEditor,
        &CreatureEditorPage::backRequested,
        this,
        [this, mainMenu]()
        {
            m_pages->setCurrentWidget(mainMenu);
        }
    );

    connect(
        mainMenu,
        &MainMenuPage::createArenaRequested,
        this,
        [this, arenaEditor]()
        {
            m_pages->setCurrentWidget(arenaEditor);
        }
    );

    connect(
        mainMenu,
        &MainMenuPage::simulatorRequested,
        this,
        [this, simulation]()
        {
            m_pages->setCurrentWidget(simulation);
        }
    );

    m_pages->setCurrentWidget(mainMenu);

    setCentralWidget(m_pages);
}

} // namespace creature_studio