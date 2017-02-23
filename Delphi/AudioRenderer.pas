unit AudioRenderer;

interface

uses
  RyuMPEG, WaveOut,
  SysUtils, Classes;

type
  TAudioRenderer = class
  private
    FWaveOut : TWaveOut;
  private
    function GetIsBusy: boolean;
  public
    constructor Create;
    destructor Destroy; override;

    procedure Open(AHandle:pointer);
    procedure Close;

    procedure Play(AData:pointer; ASize:integer);
  public
    property IsBusy : boolean read GetIsBusy;
  end;

implementation

{ TAudioRenderer }

procedure TAudioRenderer.Close;
begin
  FWaveOut.Stop;
end;

constructor TAudioRenderer.Create;
begin
  inherited;

  FWaveOut := TWaveOut.Create(nil);
end;

destructor TAudioRenderer.Destroy;
begin
  Close;

  FreeAndNil(FWaveOut);

  inherited;
end;

function TAudioRenderer.GetIsBusy: boolean;
begin
  Result := FWaveOut.DataInBuffer > 3;
end;

procedure TAudioRenderer.Open(AHandle: pointer);
begin
  Close;

  FWaveOut.Channels   := get_channels(AHandle);
  FWaveOut.SampleRate := get_sample_rate(AHandle);

  FWaveOut.Start;
end;

procedure TAudioRenderer.Play(AData: pointer; ASize: integer);
begin
  FWaveOut.Play(AData, ASize);
end;

end.
