#pragma once
#include <QByteArray>
#include <QVariant>
#include <QUrl>
#include <QVector>

#include <experimental/optional>

namespace LSP {


#define LSP_MESSAGE \
    const QByteArray & method() const Q_DECL_OVERRIDE;


#define LSP_MESSAGE_IMPL(Class, Method) \
    const QByteArray & Class::method() const { \
        static QByteArray name(QByteArrayLiteral(Method)); \
        return name; \
    }


template <typename T>
using Optional = std::experimental::optional<T>;

struct Message {
    virtual ~Message() = default;
};


struct RequestMessage : public Message {
    QVariant id;
    virtual const QByteArray & method() const = 0;
};

struct NotificationMessage : public Message {
    virtual const QByteArray & method() const = 0;
};

struct ResponseError {
    qint16   code;
    QString  message;
    QVariant data;
};

template <typename Result>
struct ResponseMessage : public Message {
    bool hasResult() const;
    bool hasError() const;

    const Result* operator->() const;
    quint16 errorCode() const;
    QString errorMessage() const;

private:
    Result* result = nullptr;
    ResponseError* error = nullptr;
};


struct Position {
    quint32 line;
    quint32 character;
};

struct Range {
    Position start;
    Position end;
};

struct Location {
    QUrl location;
    Range range;
};

struct Diagnostic {
    enum class Severity {
        Error = 1,
        Warning = 2,
        Information = 3,
        Hint = 4
    };


    Range range;
    QString message;

    Optional<QVariant> code;
    Optional<QString> source;
    Optional<Severity> severity;
};

struct TextDocumentIdentifier {
    QUrl uri;
};

struct VersionedTextDocumentIdentifier : public TextDocumentIdentifier {
    QUrl uri;
    quint16 version;
};

struct TextEdit {
    Range range;
    QString newText;
};

struct TextDocumentEdit {
    VersionedTextDocumentIdentifier textDocument;
    QVector<TextEdit> edits;
};

struct WorkspaceEdit {
    QMap<QUrl, QVector<TextEdit>> changes;
    QVector<TextDocumentEdit> documentChanges;
};


struct TextDocumentItem {
    QUrl document;
    QByteArray languageId;
    quint16 version;
    QString text;
};

struct TextDocumentPositionParams {
    TextDocumentIdentifier textDocument;
    Position position;
};

struct DocumentFilter {
    Optional<QByteArray> language;
    Optional<QByteArray> scheme;
    Optional<QString> pattern;
};

struct TextDocumentContentChangeEvent {
    Optional<Range> range;
    Optional<quint32> rangeLength;
    QString text;
};

using DocumentSelector = QVector<DocumentFilter>;


struct CompletionItem {
    QString label;
    QString detail;
    QString documentation;
    QString sortText;
    QString filterText;
    QString insertText;
    Optional<TextEdit> textEdit;
    QVector<TextEdit> additionalTextEdits;
    QVariant data;

    enum InsertTextFormat {
        FormatPlainText = 1,
        FormatSnippet = 2
    };
    Optional<InsertTextFormat> insertTextFormat;


    enum Kind {
        Text = 1,
        Method = 2,
        Function = 3,
        Constructor = 4,
        Field = 5,
        Variable = 6,
        Class = 7,
        Interface = 8,
        Module = 9,
        Property = 10,
        Unit = 11,
        Value = 12,
        Enum = 13,
        Keyword = 14,
        Snippet = 15,
        Color = 16,
        File = 17,
        Reference = 18,
    };
    Optional<Kind> kind;
};

struct CompletionList {
    bool isIncomplete;
    QVector<CompletionItem> items;
};

/* C -> S */
struct CancelMessage : public NotificationMessage {
    LSP_MESSAGE
    QVariant id;
};

/* C -> S */
struct InitializeRequest : public RequestMessage {
    LSP_MESSAGE
};

struct InitializeParams {
    QVariant processId;
    QUrl rootUri;
    QVariantList initializationOptions;

    enum Workspace {
        WSCAP_APPLY_EDIT  = 0x01,
        WSCAP_VERSIONED_DOCUMENT_CHANGES = 0x02,
    };

    quint32 workspaceCapabilities;
    quint32 documentCapabilities;
};


struct InitializeResult {
    //Todo : server capabilities

};

/* C -> S */
struct InitializedMessage : public NotificationMessage {
    LSP_MESSAGE
    /* void */
};

/* C -> S */
struct ShutdownRequest : public RequestMessage {
    LSP_MESSAGE
};

/* C -> S */
struct ExitMessage : public NotificationMessage {
    LSP_MESSAGE
};


/* S -> C */
struct ShowMessageNotification : public NotificationMessage {
    LSP_MESSAGE

    enum MessageType {
        Error = 1,
        Warning = 2,
        Info = 3,
        Log = 4
    } type;

    QString message;
};


/* S -> C */
struct ShowLogNotification : public ShowMessageNotification {
    LSP_MESSAGE
};

/* S -> C */
struct ShowMessageRequest : public NotificationMessage {
    LSP_MESSAGE

    using MessageType = ShowMessageNotification::MessageType;

    MessageType type;
    QString message;
    QStringList actions;
};

//TODO Telemetry
//TODO Dynamic registration

/* C -> S */
struct DidChangeConfigurationNotification : public NotificationMessage {
    LSP_MESSAGE
    QVariantList settings;
};

/* C -> S */
struct DidOpenTextDocumentNotification : public NotificationMessage {
    LSP_MESSAGE
    TextDocumentItem textDocument;
};


/* C -> S */
struct DidChangeTextDocumentNotification : public NotificationMessage {
    LSP_MESSAGE
    VersionedTextDocumentIdentifier textDocument;
    QVector<TextDocumentContentChangeEvent> contentChanges;
};

/* C -> S */
struct WillSaveTextDocumentNotification : public NotificationMessage {
    LSP_MESSAGE
    TextDocumentIdentifier textDocument;
    enum Reason {
        Manual = 1,
        AfterDelay = 2,
        FocusOut = 3,
    } reason;
};

/* C -> S */
struct WillSaveTextDocumentRequest : public RequestMessage {
    LSP_MESSAGE
    TextDocumentIdentifier textDocument;

    using Reason = WillSaveTextDocumentNotification::Reason;
    Reason reason;
};

/* C -> S */
struct DidSaveTextDocumentNotification : public RequestMessage {
    LSP_MESSAGE
    TextDocumentIdentifier textDocument;
    Optional<QString> text;
};

/* C -> S */
struct DidCloseTextDocumentNotification : public RequestMessage {
    LSP_MESSAGE
    TextDocumentIdentifier textDocument;
};


struct FileEvent {
    QUrl document;
    enum Type {
        Created = 1,
        Changed = 2,
        Deleted = 3,
    } type;
};

/* C -> S */
struct DidChangeWatchedFilesNotification : public RequestMessage {
    LSP_MESSAGE
    QVector<FileEvent> changes;
};

/* S -> C */
struct PublishDiagnosticsNotification : public RequestMessage {
    LSP_MESSAGE
    QUrl document;
    QVector<Diagnostic> diagnostics;
};


/* C -> S */
struct CompletionRequest : public RequestMessage {
    LSP_MESSAGE
    TextDocumentPositionParams position;
};


/* C -> S */
struct CompletionItemResolveRequest : public RequestMessage {
    LSP_MESSAGE
    CompletionItem item;
};

/* C -> S */
struct HoverRequest : public RequestMessage {
    LSP_MESSAGE
};


struct MarkedString {
    QString str;
    QString language;
};

struct Hover {
    QVector<MarkedString> content;
    Optional<Range> range;
};

/* C -> S */
struct SignatureHelpRequest : public RequestMessage {
    LSP_MESSAGE
    TextDocumentPositionParams params;
};

struct ParameterInformation {
    QString label;
    QString documentation;
};

struct SignatureInformation {
    QString label;
    QString documentation;
    Optional<QVector<ParameterInformation>> parameters;
};

struct SignatureHelp {
    Optional<quint16> activeSignature;
    Optional<quint8> activeParameter;
    QVector<SignatureInformation> signatures;
};

struct ReferenceContext {
    bool includeDeclaration;
};

struct ReferenceParams : public TextDocumentPositionParams {
    ReferenceContext context;
};

/* C -> S */
struct GotoDefinitionRequest : public RequestMessage {
    LSP_MESSAGE
    ReferenceParams params;
};

/* C -> S */
struct DocumentHighlightRequest : public RequestMessage {
    LSP_MESSAGE
    TextDocumentPositionParams params;
};

struct DocumentHighlight {
    enum Kind {
        Text  = 1,
        Read  = 2,
        Write = 3,
    };
    Range range;
    Optional<Kind> kind;
};

struct DocumentSymbolParams {
    TextDocumentIdentifier textDocument;
};

struct DocumentSymbolsRequest : public RequestMessage {
    LSP_MESSAGE
    DocumentSymbolParams params;
};

struct SymbolInformation {
    QString name;
    Location location;
    QString containerName;

    enum SymbolKind {
        File = 1,
        Module = 2,
        Namespace = 3,
        Package = 4,
        Class = 5,
        Method = 6,
        Property = 7,
        Field = 8,
        Constructor = 9,
        Enum = 10,
        Interface = 11,
        Function = 12,
        Variable = 13,
        Constant = 14,
        String = 15,
        Number = 16,
        Boolean = 17,
        Array = 18,
    } kind;
};


struct WorkspaceSymbolParams {
    QString query;
};

/* C -> S */
struct WorkspaceSymbolsRequest : public RequestMessage {
    LSP_MESSAGE
    DocumentSymbolParams params;
};


//TODO : code lens

//TODO : Links


struct FormattingOptions {
    quint8 tabSize;
    bool insertSpaces;
};

struct DocumentFormattingParams {
    TextDocumentIdentifier textDocuments;
    FormattingOptions options;
};

struct DocumentRangeFormattingParams : public DocumentFormattingParams {
    Range range;
};

struct DocumentOnTypeFormattingParams : public DocumentFormattingParams {
    Position pos;
    QChar ch;
};


/* C -> S */
struct DocumentFormattingRequest : public RequestMessage {
    LSP_MESSAGE
    DocumentFormattingParams params;
};



/* C -> S */
struct DocumentRangeFormattingRequest : public RequestMessage {
    LSP_MESSAGE
    DocumentRangeFormattingParams params;
};

/* C -> S */
struct DocumentOnTypeFormattingRequest : public RequestMessage {
    LSP_MESSAGE
    DocumentOnTypeFormattingParams params;
};


struct RenameParams {
    TextDocumentIdentifier textDocument;
    Position position;
    QString newName;
};

/* C -> S */
struct RenameRequest : public RequestMessage {
    LSP_MESSAGE
    RenameParams params;
};


// TODO Execute commands


struct ApplyWorkspaceEditParams {
    WorkspaceEdit edit;
};

/* S -> C */
struct WorkspaceEditRequest : public RequestMessage {
    LSP_MESSAGE
    ApplyWorkspaceEditParams edit;
};

struct ApplyWorkspaceEditResponse {
    bool applied;
};

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
