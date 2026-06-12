@echo off
setlocal EnableDelayedExpansion

rem ============================================================================
rem run.bat -- recompile CompileCC.oap -> CompileCC.ind and feed an input
rem            file through the resulting compiler. Prints stdout/stderr.
rem
rem Usage:    run.bat [--fast] <input.txt>
rem Example:  run.bat sample.txt          (full recompile + run)
rem           run.bat --fast sample.txt   (skip recompile, run existing .ind)
rem ============================================================================

set "OAPDIR=%~dp0"
set "ROOT=%OAPDIR%.."
set "DELPHI=E:\miemp\Millicom\Millicom\millicom.exe"
set "DELPHIDIR=E:\miemp\Millicom\Millicom"
set "EXE=%ROOT%\x64\Debug\Millicom.exe"
set "OAP=%OAPDIR%CompileCC.oap"
set "IND=%OAPDIR%CompileCC.ind"
set "EMPTY=%ROOT%\tests\empty.oap"

rem ---- Parse args ------------------------------------------------------------
set "FAST="
set "INPUT="
:parse_args
if "%~1"=="" goto args_done
if /i "%~1"=="--fast" (
  set "FAST=1"
  shift
  goto parse_args
)
if /i "%~1"=="-f" (
  set "FAST=1"
  shift
  goto parse_args
)
if "%INPUT%"=="" (
  set "INPUT=%~f1"
  shift
  goto parse_args
)
echo Error: unexpected argument %1
exit /b 1
:args_done

if "%INPUT%"=="" (
  echo Usage: %~nx0 [--fast^|-f] ^<input.txt^>
  echo.
  echo   Without --fast:  recompile CompileCC.oap -^> CompileCC.ind, then run
  echo                    the .ind with ^<input^> as argv[2] and print stdout/stderr.
  echo   With --fast:     skip the recompile and reuse the existing .ind.
  exit /b 1
)

if not exist "%INPUT%" (
  echo Error: input file not found: %INPUT%
  exit /b 1
)
if not exist "%EXE%" (
  echo Error: C++ Millicom.exe not found at %EXE%
  echo Build with MSBuild "%ROOT%\Millicom.vcxproj" /p:Configuration=Debug /p:Platform=x64
  exit /b 1
)

rem ---- 1. Recompile .oap -> .ind (unless --fast) -----------------------------
if defined FAST (
  if not exist "%IND%" (
    echo Error: --fast given but %IND% does not exist. Run without --fast first.
    exit /b 2
  )
  echo [skip] Recompile skipped ^(--fast^); reusing existing %IND%
) else (
  if not exist "%DELPHI%" (
    echo Error: Delphi millicom.exe not found at %DELPHI%
    exit /b 1
  )
  if not exist "%OAP%" (
    echo Error: %OAP% not found
    exit /b 1
  )
  if not exist "%EMPTY%" (
    echo Error: %EMPTY% not found ^(needed as --run dummy for --compile-to^)
    exit /b 1
  )

  echo [1/2] Recompiling
  echo        from: %OAP%
  echo        to:   %IND%
  rem Drop any prior .ind so we can detect compile failures by its absence.
  if exist "%IND%" del /q "%IND%"
  rem The Delphi millicom.exe AV-loops in --exit mode if CWD isn't its install
  rem directory (CursorTimer hits an empty TPageControl). pushd before invoking.
  pushd "%DELPHIDIR%"
  start "millicom-compile" /wait "%DELPHI%" --bootstrap "%OAP%" --run "%EMPTY%" --compile-to "%IND%" --exit-delay 8000 --exit
  popd
  if not exist "%IND%" (
    echo Error: compile failed, %IND% was not produced.
    exit /b 2
  )
)

rem ---- 2. Run the input through CompileCC.ind -- ONE stage. The .ind mutes the
rem        compiler's trace console off stdout (TraceCon.OutFileSet -> trace.log),
rem        so stdout is just the program's own output. No intermediate .ind.
if defined FAST (
  echo [run] %INPUT%
) else (
  echo [2/2] Running %INPUT%
)
echo ---- stdout ----
"%EXE%" "%IND%" "%INPUT%"
set "RC=!ERRORLEVEL!"
echo ---- end (exit=!RC!) ----
exit /b !RC!
