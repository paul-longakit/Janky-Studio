#pragma once

#include <creature_studio/domain/creature.hpp>
#include <creature_studio/painting/paint_document.hpp>

#include <QWidget>

class QCheckBox;
class QDoubleSpinBox;
class QLineEdit;
class QListWidget;
class QPushButton;

namespace creature_studio
{

class CreatureEditorPage final : public QWidget
{
    Q_OBJECT

public:
    explicit CreatureEditorPage(
        domain::Creature& creature,
        QWidget* parent = nullptr);

signals:
    void stateEditorRequested();
    void backRequested();

private:
    void refreshPartList();

    void refreshAnimationList();
    void refreshAnimationDetails();

    void addAnimation();
    void removeSelectedAnimation();

    void updateSelectedAnimationName();
    void updateSelectedAnimationFps();
    void updateSelectedAnimationLooping();

    void saveFrame();
    void commitPart();

    domain::Animation* selectedAnimation();

    domain::Creature& m_creature;
    painting::PaintDocument m_document;

    QListWidget* m_partList;
    QListWidget* m_animationList;
    QLineEdit* m_animationNameEdit;
    QDoubleSpinBox* m_fpsSpinBox;
    QCheckBox* m_loopingCheckBox;

    QPushButton* m_addAnimationButton;
    QPushButton* m_removeAnimationButton;
    QPushButton* m_saveFrameButton;
    QPushButton* m_commitPartButton;
};

} // namespace creature_studio