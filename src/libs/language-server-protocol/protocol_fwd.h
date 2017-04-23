#ifndef PROTOCOL_FWD_H
#define PROTOCOL_FWD_H

namespace LSP {

struct Message;
struct RequestMessage;
struct ResponseError;
struct NotificationMessage;
template <typename Result>
struct ResponseMessage;

struct Position;
struct Range;
struct Location;
struct Diagnostic;
struct TextDocumentIdentifier;
struct VersionedTextDocumentIdentifier;
struct TextEdit;
struct TextDocumentEdit;
struct WorkspaceEdit;
struct TextDocumentItem;
struct TextDocumentPositionParams;
struct DocumentFilter;
struct TextDocumentContentChangeEvent;
struct CompletionItem;
struct CompletionList;

struct CancelMessage;
struct InitializeRequest;
struct InitializeParams;
struct InitializeResult;
struct InitializedMessage;
struct ShutdownRequest;
struct ExitMessage;
struct ShowMessageNotification;
struct ShowLogNotification;
struct ShowMessageRequest;
struct DidChangeConfigurationNotification;
struct DidOpenTextDocumentNotification;
struct DidChangeTextDocumentNotification;
struct WillSaveTextDocumentNotification;
struct WillSaveTextDocumentRequest;
struct DidSaveTextDocumentNotification;
struct DidSaveTextDocumentNotification;
struct DidCloseTextDocumentNotification;

struct FileEvent;
struct DidChangeWatchedFilesNotification;
struct PublishDiagnosticsNotification;
struct CompletionRequest;
struct CompletionItemResolveRequest;
struct HoverRequest;
struct MarkedString;
struct SignatureHelpRequest;
struct ParameterInformation;
struct SignatureInformation;
struct SignatureHelp;
struct ReferenceContext;
struct ReferenceParams;

struct GotoDefinitionRequest;
struct DocumentHighlightRequest;
struct DocumentHighlight;

struct DocumentSymbolParams;
struct DocumentSymbolsRequest;
struct SymbolInformation;
struct WorkspaceSymbolParams;
struct WorkspaceSymbolsRequest;

struct FormattingOptions;
struct DocumentFormattingParams;
struct DocumentRangeFormattingParams;
struct DocumentOnTypeFormattingParams;
struct DocumentFormattingRequest;
struct DocumentRangeFormattingRequest;
struct DocumentOnTypeFormattingRequest;

struct RenameParams;
struct RenameReques;


struct ApplyWorkspaceEditParams;
struct WorkspaceEditRequest;
struct ApplyWorkspaceEditResponse;

}

#endif
