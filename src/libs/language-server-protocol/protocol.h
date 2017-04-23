#pragma once
#include <QByteArray>
#include <QVariant>
#include <QUrl>
#include <QVector>
#include <QJsonObject>
#include <experimental/optional>

namespace LSP {


#define LSP_MESSAGE \
    const QString & method() const Q_DECL_OVERRIDE; \
    QJsonObject serializeParams() const Q_DECL_OVERRIDE;

#define LSP_MESSAGE_VOID \
    const QString & method() const Q_DECL_OVERRIDE; \


template <typename T>
using Optional = std::experimental::optional<T>;

using number = int;

struct Message {
    virtual const QString & method() const = 0;
    virtual QJsonObject serialize() const;
    virtual ~Message() = default;

protected:
    virtual QJsonObject serializeParams() const;

};


struct RequestMessage : public Message {
    QVariant id;
    QJsonObject serialize() const Q_DECL_OVERRIDE;
};

struct NotificationMessage : public Message {
    QJsonObject serialize() const Q_DECL_OVERRIDE;
};

struct ResponseError {
    qint16   code;
    QString  message;
    QVariant data;
};

template <typename Result>
struct ResponseMessage : public Message {
    bool hasResult() const {
        return result;
    }
    bool hasError() const {
        return error;
    }

    const Result* operator->() const {
        Q_ASSERT(result);
        return result;
    }
    number errorCode() const {
        if(!error)
            return 0;
        return error->code;
    }
    QString errorMessage() const {
        if(!error)
            return {};
        return error->message;
    }

private:
    Result* result = nullptr;
    ResponseError* error = nullptr;
};


struct Position {
    number line;
    number character;
};

struct Range {
    Position start;
    Position end;
};

struct Location {
    QUrl uri;
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
    number version;
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
    //QMap<QUrl, QVector<TextEdit>> changes;
    QVector<TextDocumentEdit> documentChanges;
};


struct TextDocumentItem {
    QUrl document;
    QByteArray languageId;
    number version;
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
    Optional<number> rangeLength;
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


struct InitializeParams {
    QVariant processId;
    QUrl rootUri;
    QVariantList initializationOptions;

    enum Workspace {
        WSCAP_APPLY_EDIT  = 0x01,
        WSCAP_VERSIONED_DOCUMENT_CHANGES = 0x02,
    };

    number workspaceCapabilities;
    number documentCapabilities;
};

/* C -> S */
struct InitializeRequest : public RequestMessage {
    LSP_MESSAGE
    InitializeParams params;
};


struct InitializeResult {
    //Todo : server capabilities

};

/* C -> S */
struct InitializedMessage : public NotificationMessage {
    LSP_MESSAGE_VOID
};

/* C -> S */
struct ShutdownRequest : public RequestMessage {
    LSP_MESSAGE_VOID
};

/* C -> S */
struct ExitMessage : public NotificationMessage {
    LSP_MESSAGE_VOID
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


struct MessageActionItem {
    QString title;
};

/* S -> C */
struct ShowMessageRequest : public NotificationMessage {
    LSP_MESSAGE

    using MessageType = ShowMessageNotification::MessageType;

    MessageType type;
    QString message;
    QVector<MessageActionItem> actions;
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
    Optional<number> activeSignature;
    Optional<number> activeParameter;
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
    number tabSize;
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


#undef LSP_MESSAGE

}
