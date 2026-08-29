#include <creature_studio/ui/pages/main_menu_page.hpp>

#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace creature_studio
{

MainMenuPage::MainMenuPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(12);

    auto* title = new QLabel("JANKY STUDIO", this);
    title->setAlignment(Qt::AlignCenter);

    QFont titleFont = title->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto* subtitle = new QLabel(
        "Creature creation and fighting studio",
        this
    );
    subtitle->setAlignment(Qt::AlignCenter);

    auto* createCreatureButton =
        new QPushButton("Create Creature", this);

    auto* openCreatureButton =
        new QPushButton("Open Creature", this);

    auto* createArenaButton =
        new QPushButton("Create Arena", this);

    auto* simulatorButton =
        new QPushButton("Simulator", this);

    auto* exitButton =
        new QPushButton("Exit", this);

    auto* buttonLayout = new QVBoxLayout();

    buttonLayout->setSpacing(10);
    buttonLayout->addWidget(createCreatureButton);
    buttonLayout->addWidget(openCreatureButton);
    buttonLayout->addWidget(createArenaButton);
    buttonLayout->addWidget(simulatorButton);
    buttonLayout->addWidget(exitButton);

    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(30);
    layout->addLayout(buttonLayout);
    layout->addStretch();

    connect(
        createCreatureButton,
        &QPushButton::clicked,
        this,
        &MainMenuPage::createCreatureRequested
    );

    connect(
        openCreatureButton,
        &QPushButton::clicked,
        this,
        &MainMenuPage::openCreatureRequested
    );

    connect(
        createArenaButton,
        &QPushButton::clicked,
        this,
        &MainMenuPage::createArenaRequested
    );

    connect(
        simulatorButton,
        &QPushButton::clicked,
        this,
        &MainMenuPage::simulatorRequested
    );

    connect(
        exitButton,
        &QPushButton::clicked,
        this,
        &QWidget::close
    );
}

} // namespace creature_studio