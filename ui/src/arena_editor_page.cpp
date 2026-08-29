#include <creature_studio/ui/pages/arena_editor_page.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace creature_studio
{

ArenaEditorPage::ArenaEditorPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);

    auto* label = new QLabel("Arena Editor", this);
    label->setAlignment(Qt::AlignCenter);

    layout->addWidget(label);
}

} // namespace creature_studio
