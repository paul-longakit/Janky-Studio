#include <creature_studio/ui/pages/state_editor_page.hpp>

#include <algorithm>
#include <cstddef>
#include <limits>

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSignalBlocker>
#include <QGroupBox>
#include <QVariant>

namespace creature_studio
{

namespace
{

QString stateTypeToString(domain::StateType type)
{
    switch (type)
    {
    case domain::StateType::Idle:
        return QStringLiteral("Idle");

    case domain::StateType::Movement:
        return QStringLiteral("Movement");

    case domain::StateType::Attack:
        return QStringLiteral("Attack");

    case domain::StateType::Defense:
        return QStringLiteral("Defense");

    case domain::StateType::Dodge:
        return QStringLiteral("Dodge");

    case domain::StateType::Hit:
        return QStringLiteral("Hit");

    case domain::StateType::Death:
        return QStringLiteral("Death");

    case domain::StateType::Special:
        return QStringLiteral("Special");
    }

    return QStringLiteral("Unknown");
}

domain::StateType stringToStateType(int index)
{
    switch (index)
    {
    case 0:
        return domain::StateType::Idle;

    case 1:
        return domain::StateType::Movement;

    case 2:
        return domain::StateType::Attack;

    case 3:
        return domain::StateType::Defense;

    case 4:
        return domain::StateType::Dodge;

    case 5:
        return domain::StateType::Hit;

    case 6:
        return domain::StateType::Death;

    case 7:
        return domain::StateType::Special;
    }

    return domain::StateType::Idle;
}

} // namespace

StateEditorPage::StateEditorPage(
    domain::Creature& creature,
    QWidget* parent)
    : QWidget(parent)
    , m_creature(creature)
    , m_stateList(new QListWidget(this))
    , m_stateNameEdit(new QLineEdit(this))
    , m_stateTypeCombo(new QComboBox(this))
    , m_stateAnimationCombo(new QComboBox(this))
    , m_setInitialStateButton(new QPushButton(
          "Set as Initial State",
          this))
    , m_addStateButton(new QPushButton("Add State", this))
    , m_removeStateButton(new QPushButton("Remove State", this))
    , m_transitionList(new QListWidget(this))
    , m_transitionTargetCombo(new QComboBox(this))
    , m_transitionConditionEdit(new QLineEdit(this))
    , m_addTransitionButton(new QPushButton("Add Transition", this))
    , m_removeTransitionButton(new QPushButton("Remove Transition", this))
{
    auto* mainLayout = new QHBoxLayout(this);

    auto* statePanel = new QVBoxLayout();

    statePanel->addWidget(m_stateList);
    statePanel->addWidget(m_addStateButton);
    statePanel->addWidget(m_removeStateButton);

    auto* detailsPanel = new QVBoxLayout();

    auto* formLayout = new QFormLayout();

    formLayout->addRow(
        "Name:",
        m_stateNameEdit);

    m_stateTypeCombo->addItem("Idle");
    m_stateTypeCombo->addItem("Movement");
    m_stateTypeCombo->addItem("Attack");
    m_stateTypeCombo->addItem("Defense");
    m_stateTypeCombo->addItem("Dodge");
    m_stateTypeCombo->addItem("Hit");
    m_stateTypeCombo->addItem("Death");
    m_stateTypeCombo->addItem("Special");

    formLayout->addRow(
        "Type:",
        m_stateTypeCombo);

    formLayout->addRow(
        "Animation:",
        m_stateAnimationCombo);

    detailsPanel->addLayout(formLayout);
    detailsPanel->addWidget(m_setInitialStateButton);

    auto* transitionGroup = new QGroupBox(
        "Transitions",
        this);

    auto* transitionLayout = new QVBoxLayout(
        transitionGroup);

    transitionLayout->addWidget(
        m_transitionList);

    transitionLayout->addWidget(
        m_transitionTargetCombo);

    transitionLayout->addWidget(
        m_transitionConditionEdit);

    transitionLayout->addWidget(
        m_addTransitionButton);

    transitionLayout->addWidget(
        m_removeTransitionButton);

    detailsPanel->addWidget(
        transitionGroup);

    detailsPanel->addStretch();

    mainLayout->addLayout(statePanel, 1);
    mainLayout->addLayout(detailsPanel, 2);

    connect(
        m_stateList,
        &QListWidget::currentRowChanged,
        this,
        [this](int)
        {
            refreshStateDetails();
        });

    connect(
        m_stateNameEdit,
        &QLineEdit::editingFinished,
        this,
        [this]()
        {
            updateSelectedStateName();
        });

    connect(
        m_stateTypeCombo,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            updateSelectedStateType();
        });

    connect(
        m_stateAnimationCombo,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            updateSelectedStateAnimation();
        });

    connect(
        m_setInitialStateButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            setSelectedStateAsInitial();
        });

    connect(
        m_addStateButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addState();
        });

    connect(
        m_removeStateButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            removeSelectedState();
        });

    connect(
        m_transitionList,
        &QListWidget::currentRowChanged,
        this,
        [this](int)
        {
            const auto* transition = selectedTransition();

            if (transition == nullptr)
            {
                m_transitionTargetCombo->setCurrentIndex(-1);
                m_transitionConditionEdit->clear();
                return;
            }

            const int targetIndex =
                m_transitionTargetCombo->findData(
                    QVariant::fromValue(
                        static_cast<qulonglong>(
                            transition->targetStateId)));

            QSignalBlocker blocker(
                m_transitionTargetCombo);

            m_transitionTargetCombo->setCurrentIndex(
                targetIndex);

            m_transitionConditionEdit->setText(
                QString::fromStdString(
                    transition->conditionTag));
        });

    connect(
        m_transitionTargetCombo,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            updateSelectedTransitionTarget();
        });

    connect(
        m_transitionConditionEdit,
        &QLineEdit::editingFinished,
        this,
        [this]()
        {
            updateSelectedTransitionCondition();
        });

    connect(
        m_addTransitionButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addTransition();
        });

    connect(
        m_removeTransitionButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            removeSelectedTransition();
        });

    refreshStateList();
}

void StateEditorPage::refreshStateList()
{
    const int previousRow = m_stateList->currentRow();

    m_stateList->clear();

    for (const auto& state : m_creature.stateMachine.states)
    {
        QString name = state.name.empty()
            ? QStringLiteral("Unnamed State")
            : QString::fromStdString(state.name);

        if (state.id == m_creature.stateMachine.initialStateId)
        {
            name += QStringLiteral("  [Initial]");
        }

        m_stateList->addItem(name);
    }

    if (!m_creature.stateMachine.states.empty())
    {
        const int row = std::clamp(
            previousRow,
            0,
            static_cast<int>(
                m_creature.stateMachine.states.size()) - 1);

        m_stateList->setCurrentRow(row);
    }

    refreshStateDetails();
}

void StateEditorPage::refreshStateDetails()
{
    const auto* state = selectedState();

    if (state == nullptr)
    {
        m_stateNameEdit->clear();
        m_stateTypeCombo->setCurrentIndex(-1);
        m_stateAnimationCombo->clear();
        m_stateAnimationCombo->setCurrentIndex(-1);

        m_stateNameEdit->setEnabled(false);
        m_stateTypeCombo->setEnabled(false);
        m_stateAnimationCombo->setEnabled(false);
        m_setInitialStateButton->setEnabled(false);

        m_transitionList->clear();
        m_transitionTargetCombo->clear();
        m_transitionConditionEdit->clear();

        m_transitionTargetCombo->setEnabled(false);
        m_transitionConditionEdit->setEnabled(false);
        m_addTransitionButton->setEnabled(false);
        m_removeTransitionButton->setEnabled(false);

        return;
    }

    m_stateNameEdit->setEnabled(true);
    m_stateTypeCombo->setEnabled(true);
    m_stateAnimationCombo->setEnabled(true);
    m_setInitialStateButton->setEnabled(
        state->id != m_creature.stateMachine.initialStateId);

    m_stateNameEdit->setText(
        QString::fromStdString(state->name));

    const int typeIndex =
        m_stateTypeCombo->findText(stateTypeToString(state->type));

    {
        QSignalBlocker blocker(m_stateTypeCombo);
        m_stateTypeCombo->setCurrentIndex(typeIndex);
    }

    {
        QSignalBlocker blocker(m_stateAnimationCombo);

        m_stateAnimationCombo->clear();

        for (const auto& animation : m_creature.animations)
        {
            m_stateAnimationCombo->addItem(
                QString::fromStdString(animation.name));
        }

        const int animationIndex =
            m_stateAnimationCombo->findText(
                QString::fromStdString(state->animationName));

        m_stateAnimationCombo->setCurrentIndex(animationIndex);
    }

    refreshTransitionList();
}

domain::State* StateEditorPage::selectedState()
{
    const int row = m_stateList->currentRow();

    if (row < 0)
    {
        return nullptr;
    }

    const auto index = static_cast<std::size_t>(row);

    if (index >= m_creature.stateMachine.states.size())
    {
        return nullptr;
    }

    return &m_creature.stateMachine.states[index];
}

void StateEditorPage::addState()
{
    if (m_creature.stateMachine.states.size() >=
        m_creature.stateMachine.maxStates)
    {
        return;
    }

    core::UniqueId nextId = 1;

    for (const auto& state : m_creature.stateMachine.states)
    {
        if (state.id >= nextId)
        {
            if (state.id == std::numeric_limits<core::UniqueId>::max())
            {
                return;
            }

            nextId = state.id + 1;
        }
    }

    domain::State state;
    state.id = nextId;
    state.name = "New State";

    m_creature.stateMachine.states.push_back(state);

    if (m_creature.stateMachine.states.size() == 1)
    {
        m_creature.stateMachine.initialStateId = state.id;
    }

    refreshStateList();

    m_stateList->setCurrentRow(
        static_cast<int>(
            m_creature.stateMachine.states.size()) - 1);
}

void StateEditorPage::removeSelectedState()
{
    const int row = m_stateList->currentRow();

    if (row < 0)
    {
        return;
    }

    const auto index = static_cast<std::size_t>(row);

    if (index >= m_creature.stateMachine.states.size())
    {
        return;
    }

    const auto removedId =
        m_creature.stateMachine.states[index].id;

    m_creature.stateMachine.states.erase(
        m_creature.stateMachine.states.begin() +
        static_cast<std::ptrdiff_t>(index));

    for (auto& state : m_creature.stateMachine.states)
    {
        state.transitions.erase(
            std::remove_if(
                state.transitions.begin(),
                state.transitions.end(),
                [removedId](const domain::StateTransition& transition)
                {
                    return transition.targetStateId == removedId;
                }),
            state.transitions.end());
    }

    if (removedId ==
        m_creature.stateMachine.initialStateId)
    {
        if (m_creature.stateMachine.states.empty())
        {
            m_creature.stateMachine.initialStateId = 0;
        }
        else
        {
            m_creature.stateMachine.initialStateId =
                m_creature.stateMachine.states.front().id;
        }
    }

    refreshStateList();

    if (!m_creature.stateMachine.states.empty())
    {
        const int newRow = std::min(
            row,
            static_cast<int>(
                m_creature.stateMachine.states.size()) - 1);

        m_stateList->setCurrentRow(newRow);
    }
}

void StateEditorPage::updateSelectedStateName()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return;
    }

    state->name =
        m_stateNameEdit->text().toStdString();

    refreshStateList();

    const int row = m_stateList->currentRow();

    if (row >= 0)
    {
        m_stateList->setCurrentRow(row);
    }
}

void StateEditorPage::updateSelectedStateType()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return;
    }

    state->type =
        stringToStateType(
            m_stateTypeCombo->currentIndex());

    refreshStateList();
}

void StateEditorPage::updateSelectedStateAnimation()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return;
    }

    state->animationName =
        m_stateAnimationCombo->currentText().toStdString();
}

void StateEditorPage::setSelectedStateAsInitial()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return;
    }

    m_creature.stateMachine.initialStateId =
        state->id;

    refreshStateList();

    const int row = m_stateList->currentRow();

    if (row >= 0)
    {
        m_stateList->setCurrentRow(row);
    }
}

void StateEditorPage::refreshTransitionList()
{
    const auto* state = selectedState();

    m_transitionList->clear();
    m_transitionTargetCombo->clear();

    if (state == nullptr)
    {
        m_transitionTargetCombo->setEnabled(false);
        m_transitionConditionEdit->setEnabled(false);
        m_addTransitionButton->setEnabled(false);
        m_removeTransitionButton->setEnabled(false);
        return;
    }

    m_transitionTargetCombo->setEnabled(true);
    m_transitionConditionEdit->setEnabled(true);
    m_addTransitionButton->setEnabled(true);

    for (const auto& candidate : m_creature.stateMachine.states)
    {
        m_transitionTargetCombo->addItem(
            QString::fromStdString(candidate.name),
            QVariant::fromValue(
                static_cast<qulonglong>(candidate.id)));
    }

    for (const auto& transition : state->transitions)
    {
        QString targetName =
            QStringLiteral("Unknown State");

        for (const auto& candidate : m_creature.stateMachine.states)
        {
            if (candidate.id == transition.targetStateId)
            {
                targetName =
                    QString::fromStdString(candidate.name);
                break;
            }
        }

        QString text =
            targetName +
            QStringLiteral(" : ") +
            QString::fromStdString(
                transition.conditionTag);

        m_transitionList->addItem(text);
    }

    m_removeTransitionButton->setEnabled(
        !state->transitions.empty());

    if (!state->transitions.empty())
    {
        m_transitionList->setCurrentRow(0);
    }
    else
    {
        m_transitionTargetCombo->setCurrentIndex(-1);
        m_transitionConditionEdit->clear();
    }
}

domain::StateTransition* StateEditorPage::selectedTransition()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return nullptr;
    }

    const int row =
        m_transitionList->currentRow();

    if (row < 0)
    {
        return nullptr;
    }

    const auto index =
        static_cast<std::size_t>(row);

    if (index >= state->transitions.size())
    {
        return nullptr;
    }

    return &state->transitions[index];
}

void StateEditorPage::addTransition()
{
    auto* state = selectedState();

    if (state == nullptr ||
        m_creature.stateMachine.states.empty())
    {
        return;
    }

    domain::StateTransition transition;

    transition.targetStateId =
        m_creature.stateMachine.states.front().id;

    transition.conditionTag =
        "condition";

    state->transitions.push_back(
        transition);

    refreshTransitionList();

    m_transitionList->setCurrentRow(
        static_cast<int>(
            state->transitions.size()) - 1);
}

void StateEditorPage::removeSelectedTransition()
{
    auto* state = selectedState();

    if (state == nullptr)
    {
        return;
    }

    const int row =
        m_transitionList->currentRow();

    if (row < 0)
    {
        return;
    }

    const auto index =
        static_cast<std::size_t>(row);

    if (index >= state->transitions.size())
    {
        return;
    }

    state->transitions.erase(
        state->transitions.begin() +
        static_cast<std::ptrdiff_t>(index));

    refreshTransitionList();
}

void StateEditorPage::updateSelectedTransitionTarget()
{
    auto* transition =
        selectedTransition();

    if (transition == nullptr)
    {
        return;
    }

    const QVariant value =
        m_transitionTargetCombo->currentData();

    if (!value.isValid())
    {
        return;
    }

    transition->targetStateId =
        static_cast<core::UniqueId>(
            value.toULongLong());

    refreshTransitionList();
}

void StateEditorPage::updateSelectedTransitionCondition()
{
    auto* transition =
        selectedTransition();

    if (transition == nullptr)
    {
        return;
    }

    transition->conditionTag =
        m_transitionConditionEdit
            ->text()
            .toStdString();

    refreshTransitionList();
}

} // namespace creature_studio