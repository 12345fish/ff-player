object fmMain: TfmMain
  Left = 0
  Top = 0
  Caption = 'FFMPEG Player'
  ClientHeight = 600
  ClientWidth = 800
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PaintBox: TPaintBox
    Left = 0
    Top = 41
    Width = 800
    Height = 559
    Align = alClient
    Color = clBlack
    ParentColor = False
    ExplicitTop = 40
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 800
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitLeft = 260
    ExplicitTop = 208
    ExplicitWidth = 185
    object btOpen: TButton
      Left = 8
      Top = 10
      Width = 75
      Height = 25
      Caption = 'Open'
      TabOrder = 0
      OnClick = btOpenClick
    end
    object btClose: TButton
      Left = 89
      Top = 9
      Width = 75
      Height = 25
      Caption = 'Close'
      TabOrder = 1
      OnClick = btCloseClick
    end
    object btPlay: TButton
      Left = 170
      Top = 9
      Width = 75
      Height = 25
      Caption = 'Play'
      TabOrder = 2
      OnClick = btPlayClick
    end
    object btStop: TButton
      Left = 251
      Top = 9
      Width = 75
      Height = 25
      Caption = 'Stop'
      TabOrder = 3
      OnClick = btStopClick
    end
  end
  object OpenDialog: TOpenDialog
    Filter = 'All files|*.*'
    Left = 32
    Top = 100
  end
  object Timer: TTimer
    Interval = 10
    OnTimer = TimerTimer
    Left = 136
    Top = 104
  end
end
