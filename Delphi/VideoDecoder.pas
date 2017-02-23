unit VideoDecoder;

interface

uses
  RyuLibBase, RyuMPEG, SimpleThread, SuspensionQueue, RyuGraphics,
  SysUtils, Classes, SyncObjs, Graphics;

type
  TVideoDecoder = class
  private
    FHandle: Pointer;
    FBitmap : TBitmap;
    FBitmapIsReady : boolean;
    FCS : TCriticalSection;
    FQueue : TSuspensionQueue<pointer>;
    FSimpleThread : TSimpleThread;
    procedure do_Decode(AFrame:pointer);
  public
    constructor Create;
    destructor Destroy; override;

    procedure Open(AStream:pointer);
    procedure Close;

    procedure Decode(AFrame:pointer);

    function GetBitmap(ABitmap:TBitmap):boolean;
  public
    property Handle : Pointer read FHandle;
  end;

implementation

{ TVideoDecoder }

procedure TVideoDecoder.Close;
begin
  FCS.Acquire;
  try
    if FHandle = nil then Exit;

    close_video(FHandle);
    FHandle := nil;
  finally
    FCS.Release;
  end;
end;

constructor TVideoDecoder.Create;
begin
  inherited;

  FHandle := nil;
  FBitmapIsReady := false;

  FBitmap := TBitmap.Create;
  FBitmap.PixelFormat := pf32bit;

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

procedure TVideoDecoder.Decode(AFrame: pointer);
begin
  if FHandle <> nil then FQueue.Push(AFrame);
end;

destructor TVideoDecoder.Destroy;
begin
  FSimpleThread.TerminateNow;

  Close;

  FreeAndNil(FBitmap);
  FreeAndNil(FQueue);
  FreeAndNil(FCS);
  FreeAndNil(FSimpleThread);
  inherited;
end;

procedure TVideoDecoder.do_Decode(AFrame: pointer);
var
  iResult : integer;
begin
  iResult := 0;

  FCS.Acquire;
  try
    if FHandle <> nil then
      iResult := decode_video(FHandle, AFrame, FBitmap.ScanLine[FBitmap.Height-1]);
  finally
    FCS.Release;
    release_frame(AFrame);
  end;

  if iResult > 0 then FBitmapIsReady := true;
end;

function TVideoDecoder.GetBitmap(ABitmap: TBitmap): boolean;
var
  Loop: Integer;
begin
  Result := false;

  FCS.Acquire;
  try
    if FBitmapIsReady = false then Exit;

    FBitmapIsReady := false;
    Result := true;

    if ABitmap.PixelFormat <> pf32bit then ABitmap.PixelFormat := pf32bit;

    if FBitmap.Width  <> ABitmap.Width  then ABitmap.Width  := FBitmap.Width;
    if FBitmap.Height <> ABitmap.Height then ABitmap.Height := FBitmap.Height;

    for Loop := 0 to FBitmap.Height-1 do
      Move(FBitmap.ScanLine[Loop]^, ABitmap.ScanLine[FBitmap.Height-Loop-1]^, FBitmap.Width * 4);
  finally
    FCS.Release;
  end;
end;

procedure TVideoDecoder.Open(AStream: pointer);
var
  iError : integer;
begin
  FCS.Acquire;
  try
    FHandle := open_video(AStream, iError);
    if iError <> 0 then
      raise Exception.Create('Can''t open video codec.');

    FBitmap.Width  := get_video_width(FHandle);
    FBitmap.Height := get_video_height(FHandle);
  finally
    FCS.Release;
  end;
end;

end.
