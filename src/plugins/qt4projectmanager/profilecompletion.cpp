#include "profilecompletion.h"
#include "profileeditor.h"
#include "profilekeywords.h"
#include <texteditor/itexteditor.h>
#include <texteditor/completionsettings.h>
#include <cplusplus/Icons.h>
#include <QDebug>

using namespace Qt4ProjectManager::Internal;

ProFileCompletion::ProFileCompletion(QObject *parent) :
    TextEditor::ICompletionCollector(parent),
    m_editor(0),
    m_startPosition(-1),
    m_variableIcon(CPlusPlus::Icons().iconForType(CPlusPlus::Icons::VarPublicIconType)),
    m_functionIcon(CPlusPlus::Icons().iconForType(CPlusPlus::Icons::FuncPublicIconType))
{
}

ProFileCompletion::~ProFileCompletion()
{

}

QList<TextEditor::CompletionItem> ProFileCompletion::getCompletions()
{
    QList<TextEditor::CompletionItem> completionItems;
    completions(&completionItems);

    return completionItems;
}

bool ProFileCompletion::shouldRestartCompletion()
{
    return false;
}

TextEditor::ITextEditable *ProFileCompletion::editor() const
{
    return m_editor;
}

int ProFileCompletion::startPosition() const
{
    return m_startPosition;
}

bool ProFileCompletion::supportsEditor(TextEditor::ITextEditable *editor)
{
    if (qobject_cast<ProFileEditorEditable *>(editor))
        return true;
    return false;
}

bool ProFileCompletion::triggersCompletion(TextEditor::ITextEditable *editor)
{
    m_editor = editor;
    const int pos = editor->position();

    if (completionSettings().m_completionTrigger == TextEditor::AutomaticCompletion) {
        QChar characterUnderCursor = editor->characterAt(pos);
        if (!characterUnderCursor.isLetterOrNumber()) {
            m_startPosition = findStartOfName();
            if (pos - m_startPosition >= 3 && !isInComment())
                return true;
        }
    }
    return false;
}

int ProFileCompletion::findStartOfName(int pos) const
{
    if (pos == -1)
        pos = m_editor->position();
    QChar chr;

    // Skip to the start of a name
    do {
        chr = m_editor->characterAt(--pos);
    } while (chr.isLetterOrNumber() || chr == QLatin1Char('_'));

    return pos + 1;
}

bool ProFileCompletion::isInComment() const
{
    const int beginOfLinePosition = m_editor->position(TextEditor::ITextEditor::StartOfLine);
    const QString lineBeginning = m_editor->textAt(beginOfLinePosition,
                                  m_startPosition - beginOfLinePosition);
    if (lineBeginning.contains(QLatin1Char('#')))
        return true;
    return false;
}

int ProFileCompletion::startCompletion(TextEditor::ITextEditable *editor)
{
    m_editor = editor;
    m_startPosition = findStartOfName();

    return m_startPosition;
}

void ProFileCompletion::completions(QList<TextEditor::CompletionItem> *completions)
{
    const int length = m_editor->position() - m_startPosition;
    if (length < 0)
        return;

    if (isInComment())
        return;

    const QString key = m_editor->textAt(m_startPosition, length);

    QList<TextEditor::CompletionItem> items;
    QStringList keywords = ProFileKeywords::variables()
            + ProFileKeywords::functions();
//    qSort(keywords);
    for (int i = 0; i < keywords.count(); i++) {
        TextEditor::CompletionItem item(this);
        item.text = keywords[i];
        item.data = QVariant::fromValue(item.text);
        item.icon = ProFileKeywords::isFunction(item.text)
                ? m_functionIcon : m_variableIcon;
        items.append(item);
    }

    filter(items, completions, key);
}

bool ProFileCompletion::typedCharCompletes(const TextEditor::CompletionItem &item, QChar typedChar)
{
    // only '(' in case of a function
    if (typedChar == QLatin1Char('(') && ProFileKeywords::isFunction(item.text))
        return true;
    return false;
}

void ProFileCompletion::complete(const TextEditor::CompletionItem &item, QChar typedChar)
{
    Q_UNUSED(typedChar)

    int replaceLength = m_editor->position() - m_startPosition;
    if (replaceLength < 0)
        return;

    QString toInsert = item.text;
    int cursorOffset = 0;
    if (ProFileKeywords::isFunction(toInsert)
            && completionSettings().m_autoInsertBrackets) {
        if (completionSettings().m_spaceAfterFunctionName) {
            if (m_editor->textAt(m_editor->position(), 2) == QLatin1String(" (")) {
                cursorOffset = 2;
            } else if (m_editor->characterAt(m_editor->position()) == QLatin1Char('(')
                       || m_editor->characterAt(m_editor->position()) == QLatin1Char(' ')) {
                replaceLength += 1;
                toInsert += QLatin1String(" (");
            } else {
                toInsert += QLatin1String(" ()");
                cursorOffset = -1;
            }
        } else {
            if (m_editor->characterAt(m_editor->position()) == QLatin1Char('(')) {
                cursorOffset = 1;
            } else {
                toInsert += QLatin1String("()");
                cursorOffset = -1;
            }
        }
    }

    m_editor->setCurPos(m_startPosition);
    m_editor->replace(replaceLength, toInsert);
    if (cursorOffset)
        m_editor->setCurPos(m_editor->position() + cursorOffset);
}

bool ProFileCompletion::partiallyComplete(const QList<TextEditor::CompletionItem> &completionItems)
{
    if (completionItems.count() == 1) {
        complete(completionItems.first(), QChar());
        return true;
    }

    return TextEditor::ICompletionCollector::partiallyComplete(completionItems);
}

void ProFileCompletion::cleanup()
{
}
