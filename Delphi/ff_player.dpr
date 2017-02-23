program ff_player;

uses
  Vcl.Forms,
  _fmMain in '_fmMain.pas' {fmMain},
  FFPlayer in 'FFPlayer.pas',
  FFStream in 'FFStream.pas',
  Scheduler in 'Scheduler.pas',
  AudioDecoder in 'AudioDecoder.pas',
  VideoDecoder in 'VideoDecoder.pas',
  AudioRenderer in 'AudioRenderer.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
