unit Scheduler;

interface

uses
  SimpleThread,
  SysUtils, Classes;

type
  TScheduler = class
  private
    FIsRunning : boolean;
    FSimpleThread : TSimpleThread;
  private
    FOnTime: TNotifyEvent;
  public
    constructor Create;
    destructor Destroy; override;

    procedure Play;
    procedure Stop;
  public
    property OnTime : TNotifyEvent read FOnTime write FOnTime;
  end;

implementation

{ TScheduler }

constructor TScheduler.Create;
begin
  inherited;

  FIsRunning := false;

  FSimpleThread := TSimpleThread.Create(
    'TScheduler',
    procedure (ASimpleThread:TSimpleThread)
    begin
      while ASimpleThread.Terminated = false do begin
        if FIsRunning and Assigned(FOnTime) then FOnTime(Self);
        ASimpleThread.Sleep(1);
      end;
    end
  );
end;

destructor TScheduler.Destroy;
begin
  FSimpleThread.TerminateNow;

  FreeAndNil(FSimpleThread);

  inherited;
end;

procedure TScheduler.Play;
begin
  FIsRunning := true;
end;

procedure TScheduler.Stop;
begin
  FIsRunning := false;
end;

end.
