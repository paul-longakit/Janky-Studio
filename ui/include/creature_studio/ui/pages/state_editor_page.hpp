#pragma once

#include <creature_studio/domain/creature.hpp>

#include <QWidget>

class QComboBox;
class QLineEdit;
class QListWidget;
class QPushButton;

namespace creature_studio
{

class StateEditorPage final : public QWidget
{
    Q_OBJECT

public:
    explicit StateEditorPage(
        domain::Creature& creature,
        QWidget* parent = nullptr);

signals:
    void backRequested();

private:
    void refreshStateList();
    void refreshStateDetails();

    void addState();
    void removeSelectedState();

    void updateSelectedStateName();
    void updateSelectedStateType();
    void updateSelectedStateAnimation();
    void setSelectedStateAsInitial();

    void refreshTransitionList();
    void addTransition();
    void removeSelectedTransition();
    void updateSelectedTransitionTarget();
    void updateSelectedTransitionCondition();

    domain::State* selectedState();
    domain::StateTransition* selectedTransition();

    domain::Creature& m_creature;

    QListWidget* m_stateList;
    QLineEdit* m_stateNameEdit;
    QComboBox* m_stateTypeCombo;
    QComboBox* m_stateAnimationCombo;
    QPushButton* m_setInitialStateButton;
    QPushButton* m_addStateButton;
    QPushButton* m_removeStateButton;

    QListWidget* m_transitionList;
    QComboBox* m_transitionTargetCombo;
    QLineEdit* m_transitionConditionEdit;
    QPushButton* m_addTransitionButton;
    QPushButton* m_removeTransitionButton;
    QPushButton* m_backButton;
};

} // namespace creature_studio