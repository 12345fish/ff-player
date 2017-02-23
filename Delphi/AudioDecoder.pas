unit AudioDecoder;

interface

uses
  RyuLibBase, RyuMPEG, SimpleThread, SuspensionQueue,
  SysUtils, Classes, SyncObjs;

type
  TAudioDecoder = class
  private
    FHandle: Pointer;
    FBuffer : pointer;
    FCS : TCriticalSection;
    FQueue : TSuspensionQueue<pointer>;
    FSimpleThread : TSimpleThread;
    procedure do_Decode(AFrame:pointer);
  private
    FOnDecoded: TDataEvent;
    function GetIsBusy: boolean;
  public
    constructor Create;
    destructor Destroy; override;

    procedure Open(AStream:pointer);
    procedure Close;

    procedure Decode(AFrame:pointer);
  public
    property Handle : Pointer read FHandle;
    property IsBusy : boolean read GetIsBusy;
    property OnDecoded : TDataEvent read FOnDecoded write FOnDecoded;
  end;

implementation

{ TAudioDecoder }

procedure TAudioDecoder.Close;
begin
  FCS.Acquire;
  try
    if FHandle = nil then Exit;

    close_audio(FHandle);
    FHandle := nil;
  finally
    FCS.Release;
  end;
end;

constructor TAudioDecoder.Create;
begin
  inherited;

  FHandle := nil;

  // I believe 16KB is enough for audio decoding.
  GetMem(FBuffer, 16 * 1024);

  FQueue := TSuspensionQueue<pointer>.Create;
  FCS := TCriticalSection.Create;

  FSimpleThread := TSimpleThread.Create(
    '',
    procedure (ASimpleThread:TSimpleThread)
    begin
      while ASimpleThread.Terminated = false do do_Decode(FQueue.Pop);
    end
  );
end;

procedure TAudioDecoder.Decode(AFrame: pointer);
begin
  if FHandle <> nil then FQueue.Push(AFrame);
end;

destructor TAudioDecoder.Destroy;
begin
  FSimpleThread.TerminateNow;

  Close;

  FreeMem(FBuffer);

  FreeAndNil(FQueue);
  FreeAndNil(FCS);
  FreeAndNil(FSimpleThread);

  inherited;
end;

procedure TAudioDecoder.do_Decode(AFrame:pointer);
var
  iSize : integer;
begin
  iSize := 0;

  FCS.Acquire;
  try
    if FHandle <> nil then decode_audio(FHandle, AFrame, FBuffer, iSize);
  finally
    FCS.Release;
    release_frame(AFrame);
  end;

  if (iSize > 0) and Assigned(FOnDecoded) then FOnDecoded(Self, FBuffer, iSize);
end;

function TAudioDecoder.GetIsBusy: boolean;
begin
  Result := FQueue.Count > 0;
end;

procedure TAudioDecoder.Open(AStream: pointer);
var
  iError : integer;
begin
  FCS.Acquire;
  try
    FHandle := open_audio(AStream, iError);
    if iError <> 0 then
      raise Exception.Create('Can''t open audio codec.');
  finally
    FCS.Release;
  end;
end;

end.
