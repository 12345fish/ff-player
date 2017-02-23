unit _fmMain;

interface

uses
  FFPlayer,
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls;

type
  TfmMain = class(TForm)
    Panel1: TPanel;
    btOpen: TButton;
    btClose: TButton;
    btPlay: TButton;
    btStop: TButton;
    OpenDialog: TOpenDialog;
    Timer: TTimer;
    PaintBox: TPaintBox;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormDestroy(Sender: TObject);
    procedure btOpenClick(Sender: TObject);
    procedure btCloseClick(Sender: TObject);
    procedure btPlayClick(Sender: TObject);
    procedure btStopClick(Sender: TObject);
    procedure TimerTimer(Sender: TObject);
  private
    FPlayer : TFFPlayer;
    FBitmap : TBitmap;
  public
  end;

var
  fmMain: TfmMain;

implementation

{$R *.dfm}

procedure TfmMain.btCloseClick(Sender: TObject);
begin
  FPlayer.Close;
end;

procedure TfmMain.btOpenClick(Sender: TObject);
begin
  if OpenDialog.Execute then FPlayer.Open(OpenDialog.FileName);
end;

procedure TfmMain.btPlayClick(Sender: TObject);
begin
  FPlayer.Play;
end;

procedure TfmMain.btStopClick(Sender: TObject);
begin
  FPlayer.Stop;
end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  FPlayer.Close;
end;

procedure TfmMain.FormCreate(Sender: TObject);
begin
  FBitmap := TBitmap.Create;
  FPlayer := TFFPlayer.Create;
end;

procedure TfmMain.FormDestroy(Sender: TObject);
begin
  FreeAndNil(FBitmap);
  FreeAndNil(FPlayer);
end;

procedure TfmMain.TimerTimer(Sender: TObject);
begin
  if FPlayer.GetBitmap(FBitmap) then begin
    PaintBox.Canvas.Draw(0, 0, FBitmap);
  end;
end;

end.
