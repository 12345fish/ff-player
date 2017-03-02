unit FFStream;

interface

uses
  RyuMPEG,
  SysUtils, Classes, SyncObjs;

type
  TFFStream = class
  private
    FCS : TCriticalSection;
    FHandle : pointer;
  public
    constructor Create;
    destructor Destroy; override;

    procedure Open(const AFilename:string);
    procedure Close;

    function Read:pointer;
  public
    property Handle : pointer read FHandle;
  end;

implementation

{ TFFStream }

procedure TFFStream.Close;
begin
  FCS.Acquire;
  try
    if FHandle = nil then Exit;

    close_stream(FHandle);
    FHandle := nil;
  finally
    FCS.Release;
  end;
end;

constructor TFFStream.Create;
begin
  inherited;

  FHandle := nil;

  FCS := TCriticalSection.Create;
end;

destructor TFFStream.Destroy;
begin
  Close;

  FreeAndNil(FCS);

  inherited;
end;

procedure TFFStream.Open(const AFilename: string);
var
  iError : integer;
begin
  FCS.Acquire;
  try
    if FHandle <> nil then close_stream(FHandle);

    FHandle := OpenStream(AFilename, iError);
    if iError <> 0 then
      raise Exception.Create('Can''t open file - ' + AFilename);
  finally
    FCS.Release;
  end;
end;

function TFFStream.Read: pointer;
begin
  Result := nil;

  FCS.Acquire;
  try
    if FHandle <> nil then Result := read_frame(FHandle);
  finally
    FCS.Release;
  end;
end;

end.
