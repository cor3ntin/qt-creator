#include "protocol.h"
#include <QPair>
#include <QJsonArray>
namespace LSP {

QJsonObject serialize(const InitializeParams &);
QJsonObject serialize(const MessageActionItem &);
QJsonObject serialize(const Position &);
QJsonObject serialize(const Range &);
QJsonObject serialize(const Location &);
QJsonObject serialize(const Diagnostic &);
QJsonObject serialize(const TextDocumentIdentifier &);
QJsonObject serialize(const VersionedTextDocumentIdentifier &);
QJsonObject serialize(const TextEdit&);
QJsonObject serialize(const TextDocumentEdit&);
QJsonObject serialize(const WorkspaceEdit&);
QJsonObject serialize(const TextDocumentItem&);
QJsonObject serialize(const TextDocumentPositionParams&);
QJsonObject serialize(const DocumentFilter&);
QJsonObject serialize(const TextDocumentContentChangeEvent&);
QJsonObject serialize(const CompletionItem&);
QJsonObject serialize(const CompletionList&);
QJsonObject serialize(const FileEvent&);
QJsonValue serialize(const MarkedString&);
QJsonObject serialize(const ParameterInformation&);
QJsonObject serialize(const SignatureInformation&);
QJsonObject serialize(const SignatureHelp&);
QJsonObject serialize(const ReferenceContext&);
QJsonObject serialize(const ReferenceParams&);
QJsonObject serialize(const DocumentHighlight&);
QJsonObject serialize(const DocumentSymbolParams&);
QJsonObject serialize(const SymbolInformation&);
QJsonObject serialize(const WorkspaceSymbolParams&);
QJsonObject serialize(const FormattingOptions&);
QJsonObject serialize(const DocumentFormattingParams&);
QJsonObject serialize(const DocumentRangeFormattingParams&);
QJsonObject serialize(const DocumentOnTypeFormattingParams&);
QJsonObject serialize(const RenameParams&);
QJsonObject serialize(const ApplyWorkspaceEditParams&);
QJsonObject serialize(const DocumentFormattingRequest&);

template <typename Enum>
typename std::enable_if<std::is_enum<Enum>::value, QJsonValue>::type serialize(Enum e) {
    auto k = typename std::underlying_type<Enum>::type(e);
    return QJsonValue(typename std::make_signed<decltype (e)>::type(k));
}

template <typename U>
typename std::enable_if<std::is_same<QUrl, U>::value, QJsonValue>::type serialize(U url) {
    return url.toString();
}

template <typename U>
typename std::enable_if<std::is_same<QVariant, U>::value, QJsonValue>::type serialize(U variant) {
    return QJsonValue::fromVariant(variant);
}

template<typename T>
typename std::enable_if<
    std::is_same<T, bool>::value ||
    std::is_same<T, double>::value ||
    std::is_same<T, int>::value ||
    std::is_same<T, qint64>::value ||
    std::is_same<T, QString>::value ||
    std::is_same<T, QLatin1String>::value ||
    std::is_same<T, const char *>::value ||
    std::is_same<T, QJsonArray>::value ||
    std::is_same<T, QJsonObject>::value ||
    std::is_same<T, QJsonValue>::value ,
    QJsonValue>::type  serialize(T t) {
        return QJsonValue(t);
}

template <typename B>
typename std::enable_if<std::is_same<QByteArray, B>::value, QJsonValue>::type serialize(B b) {
    return serialize(QString::fromUtf8(b));
}


template <typename T>
QJsonArray serialize(const QVector<T> & vec) {
    QJsonArray array;
    for(const auto & v : vec) {
        array.append(std::move(serialize(v)));
    }
    return array;
}

template <typename T>
QJsonValue serialize(const Optional<T> & opt) {
    if(!opt)
        return QJsonValue(QJsonValue::Null);
    return serialize(*opt);

}


template <typename Value>
void do_build(QJsonObject & o, QString && key, Value && v) {
    QJsonValue serialized{serialize(v)};

    if(!serialized.isNull())
        o.insert(std::move(key), std::move(serialized));
}
template <typename Value, typename... Args>
void do_build(QJsonObject & o, QString && key, Value && v, Args... args) {

    do_build(o, std::forward<QString>(key), std::forward<Value>(v));
    do_build(o, std::forward<Args>(args)...);
}


#define VALUE(name, expression) QStringLiteral(#name) , (expression)

template <typename... Args>
QJsonObject JSON(QJsonObject && o, Args... args) {
    do_build(o, std::forward<Args>(args)...);
    return std::move(o);
}

template <typename... Args>
QJsonObject JSON(Args... args) {
    QJsonObject o;
    do_build(o, std::forward<Args>(args)...);
    return o;
}

QJsonObject Message::serialize() const {
    QJsonObject object;
    auto params = serializeParams();
    object.insert("params", params);
    object.insert(QStringLiteral("jsonrpc"), "2.0");
    object.insert(QStringLiteral("method"), this->method());
    return object;
}

QJsonObject Message::serializeParams() const {
    return QJsonObject{};
}

QJsonObject RequestMessage::serialize() const {

    QJsonObject object = Message::serialize();
    if(object.isEmpty())
        return object;
    object.insert(QStringLiteral("id"), QJsonValue::fromVariant(id));
    return object;
}

QJsonObject NotificationMessage::serialize() const {

    QJsonObject object = Message::serialize();
    return object;
}


QJsonObject InitializeRequest::serializeParams() const {
    return ::LSP::serialize(params);
}

QJsonObject CancelMessage::serializeParams() const {
    if(id.isNull() || !id.canConvert(QVariant::String))
        return {};
    return QJsonObject{{QStringLiteral("id"), QJsonValue::fromVariant(id)}};
}

QJsonObject ShowMessageNotification::serializeParams() const {
    return JSON(
        VALUE(message, message),
        VALUE(type, type)
    );
}

QJsonObject ShowMessageRequest::serializeParams() const {
    return JSON(
        VALUE(message, message),
        VALUE(type, type),
        VALUE(actions, actions)
    );
}

QJsonObject serialize(const InitializeParams &);
QJsonObject serialize(const MessageActionItem & item) {
    if(item.title.isEmpty())
        return {};
    return QJsonObject{{QStringLiteral("title"), item.title}};
}

QJsonObject serialize(const Position & pos) {
    return JSON(
        VALUE(line, pos.line),
        VALUE(ch, pos.character)
    );
}

QJsonObject serialize(const Range & range) {
    return JSON(
        VALUE(start, range.start),
        VALUE(end, range.end)
    );
}

QJsonObject serialize(const Location & loc) {
    return JSON(
        VALUE(uri, loc.uri),
        VALUE(range, loc.range)
    );
}

QJsonObject serialize(const Diagnostic & diag) {
    return JSON(
        VALUE(range, diag.range),
        VALUE(severity, diag.severity),
        VALUE(code, diag.code),
        VALUE(source, diag.code),
        VALUE(message, diag.message)
    );
}

QJsonObject serialize(const TextDocumentIdentifier & id) {
    return JSON(
        VALUE(uri, id.uri)
    );
}

QJsonObject serialize(const VersionedTextDocumentIdentifier & id) {
    return JSON(
        serialize((const TextDocumentIdentifier&)id),
        VALUE(uri, id.uri)
    );
}

QJsonObject serialize(const TextEdit& edit) {
    return JSON(
        VALUE(range, edit.range),
        VALUE(newText, edit.newText)
    );
}

QJsonObject serialize(const TextDocumentEdit& ted) {
    return JSON(
        VALUE(textDocument, ted.textDocument),
        VALUE(edits, ted.edits)
    );
}

QJsonObject serialize(const WorkspaceEdit& we) {
    return JSON(
        VALUE(documentChanges, we.documentChanges)
    );
}

QJsonObject serialize(const TextDocumentItem& item) {
    return JSON(
        VALUE(uri, item.document),
        VALUE(languageId, item.languageId),
        VALUE(version, item.version),
        VALUE(text, item.text)
    );
}

QJsonObject serialize(const TextDocumentPositionParams& params) {
    return JSON(
        VALUE(textDocument, params.textDocument),
        VALUE(position, params.position)
    );
}

QJsonObject serialize(const DocumentFilter& filter) {
    return JSON(
        VALUE(language, filter.language),
        VALUE(scheme, filter.scheme),
        VALUE(pattern, filter.pattern)
    );
}

QJsonObject serialize(const TextDocumentContentChangeEvent& event) {
    return JSON(
       VALUE(range, event.range),
       VALUE(rangeLenght, event.rangeLength),
       VALUE(text, event.text)
    );
}

QJsonObject serialize(const CompletionItem& item) {
    return JSON(
       VALUE(label, item.label),
       VALUE(kind, item.kind),
       VALUE(detail, item.detail),
       VALUE(documentation, item.documentation),
       VALUE(sortText, item.sortText),
       VALUE(filterText, item.filterText),
       VALUE(insertText, item.insertText),
       VALUE(insertTextFormat, item.insertTextFormat),
       VALUE(textEdit, item.textEdit),
       VALUE(additionalTextEdits, item.additionalTextEdits),
       //VALUE(command, item.command),
       VALUE(data, item.data)
    );
}

QJsonObject serialize(const CompletionList& lst) {
    return JSON(
        VALUE(isIncomplete, lst.isIncomplete),
        VALUE(items, lst.items)
    );
}

QJsonObject serialize(const FileEvent& event) {
    return JSON(
        VALUE(uri,  event.document),
        VALUE(type, event.type)
    );
}

QJsonValue serialize(const MarkedString& string) {
    if(string.language.isEmpty())
        return string.str;

    return JSON(
        VALUE(language,  string.language),
        VALUE(value, string.str)
    );
}

QJsonObject serialize(const ParameterInformation& param) {
    return JSON(
        VALUE(label, param.label),
        VALUE(documentation, param.documentation)
    );
}

QJsonObject serialize(const SignatureInformation& si) {
    return JSON(
        VALUE(label, si.label),
        VALUE(documentation, si.documentation),
        VALUE(parameters, si.parameters)
    );
}

QJsonObject serialize(const SignatureHelp& sh) {
    return JSON(
        VALUE(signatures, sh.signatures),
        VALUE(activeSignature, sh.activeSignature),
        VALUE(activeParameter, sh.activeParameter)
    );
}

QJsonObject serialize(const ReferenceContext& rc) {
    return JSON(
        VALUE(includeDeclaration, rc.includeDeclaration)
    );
}

QJsonObject serialize(const ReferenceParams&);



QJsonObject serialize(const DocumentHighlight& dh) {
    return JSON(
        VALUE(range, dh.range),
        VALUE(kind, dh.kind)
    );
}

QJsonObject serialize(const DocumentSymbolParams&);


QJsonObject serialize(const SymbolInformation& si) {
    return JSON(
        VALUE(name, si.name),
        VALUE(kind, si.kind),
        VALUE(location, si.location),
        VALUE(containerName, si.containerName)
    );
}

QJsonObject serialize(const WorkspaceSymbolParams&);
QJsonObject serialize(const FormattingOptions& fo) {
    return JSON(
        VALUE(tabSize, fo.tabSize),
        VALUE(insertSpace, fo.insertSpaces)
    );
}

QJsonObject serialize(const DocumentFormattingParams&);
QJsonObject serialize(const DocumentRangeFormattingParams&);
QJsonObject serialize(const DocumentOnTypeFormattingParams&);
QJsonObject serialize(const RenameParams&);
QJsonObject serialize(const ApplyWorkspaceEditParams&);
QJsonObject serialize(const DocumentFormattingRequest&);

#define LSP_MESSAGE_IMPL(Class, Method) \
    const QString & Class::method() const { \
        static QString name(QStringLiteral(Method)); \
        return name; \
    } \

LSP_MESSAGE_IMPL(InitializeRequest, "initialize")
LSP_MESSAGE_IMPL(InitializedMessage, "initialized")
LSP_MESSAGE_IMPL(ShutdownRequest, "shutdown")
LSP_MESSAGE_IMPL(ExitMessage, "exit")


LSP_MESSAGE_IMPL(CancelMessage, "$/cancelRequest")

LSP_MESSAGE_IMPL(ShowMessageNotification, "window/showMessage")
LSP_MESSAGE_IMPL(ShowMessageRequest, "window/showMessageRequest")
LSP_MESSAGE_IMPL(ShowLogNotification, "window/logMessage")


LSP_MESSAGE_IMPL(DidChangeConfigurationNotification, "workspace/didChangeConfiguration")
LSP_MESSAGE_IMPL(DidChangeWatchedFilesNotification, "workspace/didChangeWatchedFiles")

LSP_MESSAGE_IMPL(DidOpenTextDocumentNotification, "textDocument/didOpen")
LSP_MESSAGE_IMPL(DidChangeTextDocumentNotification, "textDocument/didChange")
LSP_MESSAGE_IMPL(WillSaveTextDocumentNotification, "textDocument/willSave")
LSP_MESSAGE_IMPL(WillSaveTextDocumentRequest, "textDocument/willSaveWaitUntil")
LSP_MESSAGE_IMPL(DidSaveTextDocumentNotification, "textDocument/didSave")
LSP_MESSAGE_IMPL(DidCloseTextDocumentNotification, "textDocument/didClose")


LSP_MESSAGE_IMPL(PublishDiagnosticsNotification, "textDocument/publishDiagnostics")
LSP_MESSAGE_IMPL(CompletionRequest, "textDocument/completion")
LSP_MESSAGE_IMPL(CompletionItemResolveRequest, "completionItem/resolve")
LSP_MESSAGE_IMPL(HoverRequest, "textDocument/hover")
LSP_MESSAGE_IMPL(SignatureHelpRequest, "textDocument/signatureHelp")
LSP_MESSAGE_IMPL(GotoDefinitionRequest, "textDocument/definition")
LSP_MESSAGE_IMPL(DocumentHighlightRequest, "textDocument/documentHighlight")
LSP_MESSAGE_IMPL(DocumentSymbolsRequest, "textDocument/documentSymbol")
LSP_MESSAGE_IMPL(WorkspaceSymbolsRequest, "workspace/symbol")
LSP_MESSAGE_IMPL(DocumentFormattingRequest, "textDocument/formatting")
LSP_MESSAGE_IMPL(DocumentRangeFormattingRequest, "textDocument/rangeFormatting")
LSP_MESSAGE_IMPL(DocumentOnTypeFormattingRequest, "textDocument/onTypeFormatting")
LSP_MESSAGE_IMPL(RenameRequest, "textDocument/rename")
LSP_MESSAGE_IMPL(WorkspaceEditRequest, "workspace/applyEdit")


}
