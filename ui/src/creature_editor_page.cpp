#include <creature_studio/ui/pages/creature_editor_page.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include <creature_studio/painting/animation_frame_converter.hpp>
#include <creature_studio/ui/widgets/paint_canvas.hpp>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSignalBlocker>
#include <QButtonGroup>

namespace creature_studio
{

CreatureEditorPage::CreatureEditorPage(
    domain::Creature& creature,
    QWidget* parent)
    : QWidget(parent)
    , m_creature(creature)
    , m_document(32, 32)
    , m_animationList(new QListWidget(this))
    , m_animationNameEdit(new QLineEdit(this))
    , m_fpsSpinBox(new QDoubleSpinBox(this))
    , m_loopingCheckBox(new QCheckBox("Looping", this))
    , m_addAnimationButton(new QPushButton("Add Animation", this))
    , m_removeAnimationButton(new QPushButton("Remove Animation", this))
    , m_saveFrameButton(new QPushButton("Save Frame", this))
{
    m_document.addLayer("Body");

    /*
     * Main workspace
     *
     * ┌──────────────┬──────────────────────────┬──────────────┐
     * │ Tools        │                          │ Animation    │
     * │              │          Canvas         │              │
     * ├──────────────┴──────────────────────────┴──────────────┤
     * │ Layers                                                   │
     * ├─────────────────────────────────────────────────────────┤
     * │ Timeline                                                 │
     * ├─────────────────────────────────────────────────────────┤
     * │ Navigation                                                │
     * └─────────────────────────────────────────────────────────┘
     */

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(6);

    // ---------------------------------------------------------
    // Header
    // ---------------------------------------------------------

    auto* headerLayout = new QHBoxLayout();

    auto* titleLabel =
        new QLabel("Creature Editor", this);

    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    auto* stateEditorButton =
        new QPushButton("State Editor", this);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(stateEditorButton);

    rootLayout->addLayout(headerLayout);

    // ---------------------------------------------------------
    // Main editor workspace
    // ---------------------------------------------------------

    auto* workspaceSplitter =
        new QSplitter(Qt::Horizontal, this);

    // ---------------------------------------------------------
    // Left: Tools
    // ---------------------------------------------------------

    auto* toolsPanel =
        new QGroupBox("Tools", this);

    auto* toolsLayout =
        new QVBoxLayout(toolsPanel);

    auto* pencilButton =
        new QToolButton(toolsPanel);

    pencilButton->setText("Pencil");
    pencilButton->setToolTip("Pencil");
    pencilButton->setCheckable(true);

    auto* brushButton =
        new QToolButton(toolsPanel);

    brushButton->setText("Brush");
    brushButton->setToolTip("Brush");
    brushButton->setChecked(true);
    brushButton->setCheckable(true);

    auto* eraserButton =
        new QToolButton(toolsPanel);

    eraserButton->setText("Eraser");
    eraserButton->setToolTip("Eraser");
    eraserButton->setCheckable(true);

    toolsLayout->addWidget(pencilButton);
    toolsLayout->addWidget(brushButton);
    toolsLayout->addWidget(eraserButton);
    toolsLayout->addStretch();

    workspaceSplitter->addWidget(toolsPanel);

    auto* toolGroup =
        new QButtonGroup(this);

    toolGroup->setExclusive(true);

    toolGroup->addButton(pencilButton);
    toolGroup->addButton(brushButton);
    toolGroup->addButton(eraserButton);

    // ---------------------------------------------------------
    // Center: Canvas
    // ---------------------------------------------------------

    auto* canvasPanel =
        new QFrame(this);

    canvasPanel->setFrameShape(QFrame::StyledPanel);

    auto* canvasLayout =
        new QVBoxLayout(canvasPanel);

    canvasLayout->setContentsMargins(4, 4, 4, 4);

    auto* canvas =
        new PaintCanvas(m_document, canvasPanel);

    canvasLayout->addWidget(canvas, 1);

    workspaceSplitter->addWidget(canvasPanel);

    // ---------------------------------------------------------
    // Right: Animation
    // ---------------------------------------------------------

    auto* animationPanel =
        new QGroupBox("Animation", this);

    auto* animationPanelLayout =
        new QVBoxLayout(animationPanel);

    animationPanelLayout->addWidget(
        new QLabel("Animations", animationPanel));

    animationPanelLayout->addWidget(
        m_animationList,
        1);

    auto* animationButtonsLayout =
        new QHBoxLayout();

    animationButtonsLayout->addWidget(
        m_addAnimationButton);

    animationButtonsLayout->addWidget(
        m_removeAnimationButton);

    animationPanelLayout->addLayout(
        animationButtonsLayout);

    auto* animationForm =
        new QFormLayout();

    animationForm->addRow(
        "Name:",
        m_animationNameEdit);

    animationForm->addRow(
        "FPS:",
        m_fpsSpinBox);

    animationForm->addRow(
        "",
        m_loopingCheckBox);

    animationPanelLayout->addLayout(
        animationForm);

    workspaceSplitter->addWidget(animationPanel);

    workspaceSplitter->setStretchFactor(0, 0);
    workspaceSplitter->setStretchFactor(1, 1);
    workspaceSplitter->setStretchFactor(2, 0);

    workspaceSplitter->setSizes({
        120,
        600,
        240
    });

    rootLayout->addWidget(
        workspaceSplitter,
        1);

    // ---------------------------------------------------------
    // Layers
    // ---------------------------------------------------------

    auto* layersPanel =
        new QGroupBox("Layers", this);

    auto* layersLayout =
        new QVBoxLayout(layersPanel);

    auto* layerList =
        new QListWidget(layersPanel);

    layerList->addItem("Body");
    layerList->setCurrentRow(0);

    layersLayout->addWidget(layerList);

    rootLayout->addWidget(
        layersPanel);

    // ---------------------------------------------------------
    // Timeline
    // ---------------------------------------------------------

    auto* timelinePanel =
        new QGroupBox("Timeline", this);

    auto* timelineLayout =
        new QHBoxLayout(timelinePanel);

    auto* frameLabel =
        new QLabel("No frames", timelinePanel);

    timelineLayout->addWidget(frameLabel);
    timelineLayout->addStretch();

    timelineLayout->addWidget(
        m_saveFrameButton);

    rootLayout->addWidget(
        timelinePanel);

    // ---------------------------------------------------------
    // Navigation
    // ---------------------------------------------------------

    auto* navigationLayout =
        new QHBoxLayout();

    auto* backButton =
        new QPushButton(
            "Back to Main Menu",
            this);

    navigationLayout->addWidget(
        backButton);

    navigationLayout->addStretch();

    rootLayout->addLayout(
        navigationLayout);

    // ---------------------------------------------------------
    // Initial state
    // ---------------------------------------------------------

    m_fpsSpinBox->setRange(0.1, 240.0);
    m_fpsSpinBox->setSingleStep(1.0);

    refreshAnimationList();

    // ---------------------------------------------------------
    // Existing behavior
    // ---------------------------------------------------------

    connect(
        pencilButton,
        &QToolButton::clicked,
        this,
        [canvas]()
        {
            canvas->setTool(
                PaintCanvas::Tool::Pencil);
        });

    connect(
        brushButton,
        &QToolButton::clicked,
        this,
        [canvas]()
        {
            canvas->setTool(
                PaintCanvas::Tool::Brush);
        });

    connect(
        eraserButton,
        &QToolButton::clicked,
        this,
        [canvas]()
        {
            canvas->setTool(
                PaintCanvas::Tool::Eraser);
        });

    connect(
        m_animationList,
        &QListWidget::currentRowChanged,
        this,
        [this](int)
        {
            refreshAnimationDetails();
        });

    connect(
        m_animationNameEdit,
        &QLineEdit::editingFinished,
        this,
        [this]()
        {
            updateSelectedAnimationName();
        });

    connect(
        m_fpsSpinBox,
        &QDoubleSpinBox::valueChanged,
        this,
        [this](double)
        {
            updateSelectedAnimationFps();
        });

    connect(
        m_loopingCheckBox,
        &QCheckBox::toggled,
        this,
        [this](bool)
        {
            updateSelectedAnimationLooping();
        });

    connect(
        m_addAnimationButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addAnimation();
        });

    connect(
        m_removeAnimationButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            removeSelectedAnimation();
        });

    connect(
        stateEditorButton,
        &QPushButton::clicked,
        this,
        &CreatureEditorPage::stateEditorRequested);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &CreatureEditorPage::backRequested);

    connect(
        m_saveFrameButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            saveFrame();
        });
}

void CreatureEditorPage::saveFrame()
{
    auto* animation = selectedAnimation();

    if (animation == nullptr)
    {
        return;
    }

    if (animation->fps <= 0.0)
    {
        return;
    }

    const auto frameIndex =
        static_cast<std::uint32_t>(
            animation->frames.size());

    animation->frames.push_back(
        painting::createAnimationFrame(
            m_document.layer(0),
            frameIndex,
            1.0 / animation->fps));
}

void CreatureEditorPage::refreshAnimationList()
{
    const int previousRow =
        m_animationList->currentRow();

    QSignalBlocker blocker(m_animationList);

    m_animationList->clear();

    for (const auto& animation : m_creature.animations)
    {
        m_animationList->addItem(
            QString::fromStdString(animation.name));
    }

    if (!m_creature.animations.empty())
    {
        const int row = std::clamp(
            previousRow,
            0,
            static_cast<int>(
                m_creature.animations.size()) - 1);

        m_animationList->setCurrentRow(row);
    }

    refreshAnimationDetails();
}

void CreatureEditorPage::refreshAnimationDetails()
{
    const auto* animation = selectedAnimation();

    if (animation == nullptr)
    {
        m_animationNameEdit->clear();
        m_fpsSpinBox->setValue(12.0);
        m_loopingCheckBox->setChecked(true);

        m_animationNameEdit->setEnabled(false);
        m_fpsSpinBox->setEnabled(false);
        m_loopingCheckBox->setEnabled(false);
        m_removeAnimationButton->setEnabled(false);
        m_saveFrameButton->setEnabled(false);

        return;
    }

    m_animationNameEdit->setEnabled(true);
    m_fpsSpinBox->setEnabled(true);
    m_loopingCheckBox->setEnabled(true);
    m_removeAnimationButton->setEnabled(true);
    m_saveFrameButton->setEnabled(true);

    {
        QSignalBlocker blocker(m_animationNameEdit);

        m_animationNameEdit->setText(
            QString::fromStdString(animation->name));
    }

    {
        QSignalBlocker blocker(m_fpsSpinBox);

        m_fpsSpinBox->setValue(animation->fps);
    }

    {
        QSignalBlocker blocker(m_loopingCheckBox);

        m_loopingCheckBox->setChecked(animation->looping);
    }
}

domain::Animation* CreatureEditorPage::selectedAnimation()
{
    const int row =
        m_animationList->currentRow();

    if (row < 0)
    {
        return nullptr;
    }

    const auto index =
        static_cast<std::size_t>(row);

    if (index >= m_creature.animations.size())
    {
        return nullptr;
    }

    return &m_creature.animations[index];
}

void CreatureEditorPage::addAnimation()
{
    constexpr const char* baseName = "New Animation";

    std::string name = baseName;

    std::size_t suffix = 2;

    while (
        std::any_of(
            m_creature.animations.begin(),
            m_creature.animations.end(),
            [&name](const domain::Animation& animation)
            {
                return animation.name == name;
            }))
    {
        name =
            std::string(baseName) +
            " " +
            std::to_string(suffix++);
    }

    domain::Animation animation;
    animation.name = name;
    animation.fps = 12.0;
    animation.looping = true;

    m_creature.animations.push_back(
        std::move(animation));

    refreshAnimationList();

    m_animationList->setCurrentRow(
        static_cast<int>(
            m_creature.animations.size()) - 1);
}

void CreatureEditorPage::removeSelectedAnimation()
{
    const int row =
        m_animationList->currentRow();

    if (row < 0)
    {
        return;
    }

    const auto index =
        static_cast<std::size_t>(row);

    if (index >= m_creature.animations.size())
    {
        return;
    }

    const std::string removedName =
        m_creature.animations[index].name;

    m_creature.animations.erase(
        m_creature.animations.begin() +
        static_cast<std::ptrdiff_t>(index));

    for (auto& state : m_creature.stateMachine.states)
    {
        if (state.animationName == removedName)
        {
            state.animationName.clear();
        }
    }

    refreshAnimationList();

    if (!m_creature.animations.empty())
    {
        const int newRow = std::min(
            row,
            static_cast<int>(
                m_creature.animations.size()) - 1);

        m_animationList->setCurrentRow(newRow);
    }
}

void CreatureEditorPage::updateSelectedAnimationName()
{
    auto* animation = selectedAnimation();

    if (animation == nullptr)
    {
        return;
    }

    const std::string newName =
        m_animationNameEdit->text()
            .trimmed()
            .toStdString();

    if (newName.empty())
    {
        refreshAnimationDetails();
        return;
    }

    const std::string oldName =
        animation->name;

    const bool duplicate =
        std::any_of(
            m_creature.animations.begin(),
            m_creature.animations.end(),
            [&newName, animation](
                const domain::Animation& candidate)
            {
                return
                    &candidate != animation &&
                    candidate.name == newName;
            });

    if (duplicate)
    {
        refreshAnimationDetails();
        return;
    }

    animation->name = newName;

    for (auto& state : m_creature.stateMachine.states)
    {
        if (state.animationName == oldName)
        {
            state.animationName = newName;
        }
    }

    refreshAnimationList();

    const int row =
        m_animationList->currentRow();

    if (row >= 0)
    {
        m_animationList->setCurrentRow(row);
    }
}

void CreatureEditorPage::updateSelectedAnimationFps()
{
    auto* animation = selectedAnimation();

    if (animation == nullptr)
    {
        return;
    }

    const double fps =
        m_fpsSpinBox->value();

    if (fps <= 0.0)
    {
        return;
    }

    animation->fps = fps;
}

void CreatureEditorPage::updateSelectedAnimationLooping()
{
    auto* animation = selectedAnimation();

    if (animation == nullptr)
    {
        return;
    }

    animation->looping =
        m_loopingCheckBox->isChecked();
}

} // namespace creature_studio