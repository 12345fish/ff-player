unit FFPlayer;

interface

uses
  RyuMPEG,
  FFStream, Scheduler, AudioDecoder, VideoDecoder, AudioRenderer,
  SysUtils, Classes, Graphics;

type
  TFFPlayer = class
  private
    FStream : TFFStream;
    FVideoDecoder : TVideoDecoder;
    FAudioRenderer : TAudioRenderer;
  private
    FScheduler : TScheduler;
    procedure on_FScheduler_Time(ASender:TObject);
  private
    FAudioDecoder : TAudioDecoder;
    procedure on_FAudioDecoder_Decoded(ASender:TObject; AData:pointer; ASize:integer);
  public
    constructor Create;
    destructor Destroy; override;

    procedure Open(const AFilename:string);
    procedure Close;

    procedure Play;
    procedure Stop;

    function GetBitmap(ABitmap:TBitmap):boolean;
  end;

implementation

{ TFFPlayer }

procedure TFFPlayer.Close;
begin
  Stop;

  FAudioRenderer.Close;
  FAudioDecoder.Close;
  FVideoDecoder.Close;
  FStream.Close;
end;

constructor TFFPlayer.Create;
begin
  inherited;

  FStream := TFFStream.Create;
  FVideoDecoder := TVideoDecoder.Create;
  FAudioRenderer := TAudioRenderer.Create;

  FScheduler := TScheduler.Create;
  FScheduler.OnTime := on_FScheduler_Time;

  FAudioDecoder := TAudioDecoder.Create;
  FAudioDecoder.OnDecoded := on_FAudioDecoder_Decoded;
end;

destructor TFFPlayer.Destroy;
begin
  Close;

  FreeAndNil(FScheduler);
  FreeAndNil(FAudioRenderer);
  FreeAndNil(FVideoDecoder);
  FreeAndNil(FAudioDecoder);
  FreeAndNil(FStream);

  inherited;
end;

function TFFPlayer.GetBitmap(ABitmap: TBitmap): boolean;
begin
  Result := FVideoDecoder.GetBitmap(ABitmap);
end;

procedure TFFPlayer.on_FAudioDecoder_Decoded(ASender: TObject; AData: pointer;
  ASize: integer);
begin
  FAudioRenderer.Play(AData, ASize);
end;

procedure TFFPlayer.on_FScheduler_Time(ASender: TObject);
var
  pFrame : pointer;
begin
  if FAudioDecoder.IsBusy or FAudioRenderer.IsBusy then Exit;

  pFrame := FStream.Read;

  case get_frame_type(pFrame) of
    AUDIO_PACKET: FAudioDecoder.Decode(pFrame);
    VIDEO_PACKET: FVideoDecoder.Decode(pFrame);
    else release_frame(pFrame);
  end;
end;

procedure TFFPlayer.Open(const AFilename: string);
begin
  FStream.Open(AFilename);
  FAudioDecoder.Open(FStream.Handle);
  FVideoDecoder.Open(FStream.Handle);
  FAudioRenderer.Open(FAudioDecoder.Handle);
end;

procedure TFFPlayer.Play;
begin
  FScheduler.Play;
end;

procedure TFFPlayer.Stop;
begin
  FScheduler.Stop;
end;

end.
